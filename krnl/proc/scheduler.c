/**
 * LuxeOS (c) 2023 by Jozef Nagy
 *
 * LuxeOS is licensed under a
 * Creative Commons Attribution-NoDerivatives 4.0 International License.
 *
 * You should have received a copy of the license along with this
 * work. If not, see <http://creativecommons.org/licenses/by-nd/4.0/>.
 */

#include <acpi/hpet.h>
#include <cpu/cpu.h>
#include <cpu/smp.h>
#include <dd/apic/apic.h>
#include <dd/apic/timer.h>
#include <proc/eb.h>
#include <proc/scheduler.h>
#include <proc/tasking.h>

#include <luxe.h>

lock_t sched_lock = lock_create();

static task_t *g_task_running[CONFIG_CPU_MAX] = { 0 };
static task_t *g_task_idling[CONFIG_CPU_MAX] = { 0 };
static uint64_t g_task_coord[CONFIG_CPU_MAX] = { 0 };
static vector_struct(task_t *) g_task_active = { 0 };

static volatile uint16_t g_cpu_num = 0;

void sched_init(const char *name, uint16_t cpu_id)
{
	lock_acquire(&sched_lock);
	g_task_idling[cpu_id] =
		task_create(name, task_idle, 255, KERNEL_TASK, NULL);
	lock_release(&sched_lock);

	apic_timer_init();
	apic_timer_set_freq(CONFIG_TIMESLICE);
	apic_timer_set_mode(APIC_TIMER_PERIODIC);
	apic_timer_set_handler(sched_ctx_switch_enter);
	apic_timer_start();

	g_cpu_num++;

	klog("done for cpu %d (idle %d)", cpu_id, g_task_idling[cpu_id]->id);
}

task_t *sched_new(const char *name, void (*entry)(uint64_t), bool usermode)
{
	lock_acquire(&sched_lock);
	task_t *t =
		task_create(name, entry, 0, usermode ? USER_TASK : KERNEL_TASK, NULL);
	lock_release(&sched_lock);
	return t;
}

void sched_add(task_t *t)
{
	lock_acquire(&sched_lock);
	vector_pushback(&g_task_active, t);
	lock_release(&sched_lock);
}

void sched_ctx_switch(void *stack, int64_t force_switch)
{
	const smp_t *smp_info = _smp_get_info();
	if (smp_info == NULL) {
		return;
	}
	if (smp_info->cpu_count != g_cpu_num) {
		return;
	}

	eb_dispatch();

	lock_acquire(&sched_lock);

	cpu_t *cpu = smp_cur_cpu(true);
	if (cpu == NULL) {
		lock_release(&sched_lock);
		return;
	}

	uint16_t cpu_id = cpu->cpu_id;
	uint64_t ticks = g_task_coord[cpu_id];

	task_t *cur = g_task_running[cpu_id];
	task_t *next = NULL;

	if (cur) {
		cur->stack_top = stack;
		cur->last_tick = ticks;

		if (cur->status == TASK_RUNNING) {
			cur->status = TASK_READY;
		}

		if ((uint64_t)cur != (uint64_t)g_task_idling[cpu_id]) {
			vector_pushback(&g_task_active, cur);
		}
	}

	g_task_running[cpu_id] = NULL;
	cur = NULL;

	uint64_t loop_sz = 0;
	for (;;) {
		if (vector_length(&g_task_active) > 0) {
			next = vector_at(&g_task_active, 0);
			vector_erase(&g_task_active, 0);
		} else {
			next = NULL;
			break;
		}

		if (next->status == TASK_READY) {
			break;
		}
		if (next->status == TASK_SLEEPING) {
			if ((hpet_get_ns() >= next->wake_time) && (next->wake_time > 0)) {
				{
					break;
				}
			}
		}
		vector_pushback(&g_task_active, next);

		loop_sz++;
		if (loop_sz >= vector_length(&g_task_active)) {
			next = NULL;
			break;
		}
	}

	if (next == NULL) {
		next = g_task_idling[cpu_id];
	}

	next->status = TASK_RUNNING;
	g_task_running[cpu_id] = next;

	cpu->tss.rsp0 = (uint64_t)(next->kern_stack_limit + STACK_SIZE);
	g_task_coord[cpu_id]++;

	if (!force_switch) {
		apic_eoi();
	}

	lock_release(&sched_lock);

	if (!(cpu->tss.rsp0 & 0xFFFF000000000000) || next->id < 1) {
#ifdef CONFIG_DEBUG
		// @todo: print debug information before panic
#endif
		panic(PANIC_KERNEL_STACK_CORRUPT);
	}

	sched_ctx_switch_exit(
		next->stack_top,
		(next->ads == NULL) ? 0 : VIRT_TO_PHYS((uint64_t)next->ads->pml4));
}

task_event_t sched_wait_for_event(task_event_t event)
{
	task_event_t e = { 0 };
	cpu_t *cpu = smp_cur_cpu(false);
	if (cpu == NULL) {
		return e;
	}

	lock_acquire(&sched_lock);

	uint16_t cpu_id = cpu->cpu_id;
	task_t *cur = g_task_running[cpu_id];
	cur->wake_time = 0;
	cur->wake_event = event;
	cur->status = TASK_SLEEPING;

	if (cur->id < 1) {
		panic(TASK_HAS_CORRUPT_ID);
	}

	lock_release(&sched_lock);

	sched_ctx_switch_force();
	return cur->wake_event;
}

bool sched_resume_event(task_event_t event)
{
	bool ret = false;

	lock_acquire(&sched_lock);

	for (size_t i = 0; i < vector_length(&g_task_active); i++) {
		task_t *t = vector_at(&g_task_active, i);
		if (t) {
			if (t->status == TASK_SLEEPING &&
				t->wake_event.type == event.type) {
				t->status = TASK_READY;
				t->wake_event.para = event.para;
				ret = true;
			}
		}
	}

	lock_release(&sched_lock);
	return ret;
}

uint64_t _sched_get_ticks()
{
	cpu_t *cpu = smp_cur_cpu(false);
	if (cpu == NULL) {
		return 0;
	}

	return g_task_coord[cpu->cpu_id];
}

__attribute__((noreturn)) void task_idle(uint64_t id)
{
	(void)id;

	for (;;) {
		task_t *t = NULL;
		lock_acquire(&sched_lock);

		size_t task_count = vector_length(&g_task_active);
		if (task_count > 0) {
			for (size_t i = 0; i < task_count; i++) {
				t = vector_at(&g_task_active, i);
				if (t->status == TASK_DEAD) {
					vector_erase(&g_task_active, i);
					break;
				} else {
					t = NULL;
				}
			}
		}
		lock_release(&sched_lock);

		if (t != NULL) {
			size_t mmap_count = vector_length(&t->memmap_list);
			for (size_t i = 0; i < mmap_count; i++) {
				mem_map_t m = vector_at(&t->memmap_list, i);
				virt_unmap(t->ads, m.virt_addr, m.np);
				kfree((void *)PHYS_TO_VIRT(m.phys_addr));
			}
			vector_erase_all(&t->memmap_list);

			klog("removed dead task %d", t->id);

			kfree((void *)t->kern_stack_limit);

			size_t mem_count = vector_length(&t->ads->mem_list);
			for (size_t i = 0; i < mem_count; i++) {
				uint64_t m = vector_at(&t->ads->mem_list, i);
				phys_free(m, 8);
			}
			vector_erase_all(&t->ads->mem_list);

			kfree((void *)t->ads->pml4);
			kfree((void *)t->ads);
			kfree(t);
		} else {
			// no dead tasks found
			__asm__ volatile("hlt");
		}
	}
}

void sched_sleep(uint64_t millis)
{
	cpu_t *cpu = smp_cur_cpu(false);
	if (cpu == NULL) {
		hpet_wait(millis);
		return;
	}

	lock_acquire(&sched_lock);

	uint16_t cpu_id = cpu->cpu_id;
	task_t *curr = g_task_running[cpu_id];
	if (curr) {
		curr->wake_time = hpet_get_ns() + (millis * 1000000);
		curr->wake_event.type = EVENT_UNDEFINED;
		curr->status = TASK_SLEEPING;
		if (curr->id < 1) {
			panic(ERR_UNKNOWN);
		}
	}

	lock_release(&sched_lock);
	sched_ctx_switch_force();
}

uint16_t sched_get_cpu_count()
{
	return g_cpu_num;
}