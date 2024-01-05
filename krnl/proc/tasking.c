/**
 * LuxeOS (c) 2023 by Jozef Nagy
 *
 * LuxeOS is licensed under a
 * Creative Commons Attribution-NoDerivatives 4.0 International License.
 *
 * You should have received a copy of the license along with this
 * work. If not, see <http://creativecommons.org/licenses/by-nd/4.0/>.
 */

#include <acpi/madt.h>
#include <acpi/hpet.h>
#include <cpu/smp.h>
#include <mem/phys.h>
#include <mem/virt.h>
#include <proc/tasking.h>

#include <luxe.h>

static uint64_t g_current_taskid = 1;

task_t *task_create(const char *name, void (*entry)(uint64_t), uint8_t priority,
					task_mode_t mode, addr_space_t *pas)
{
	if (g_current_taskid == TASKID_MAX) {
		klog("reached maximum amount of tasks");
		return NULL;
	}
	task_t *new = kmalloc(sizeof(task_t));
	memset(new, 0, sizeof(task_t));

	new->id = g_current_taskid;

	task_regs_t *new_regs = NULL;
	addr_space_t *as = create_ads();

	if (mode == USER_TASK) {
		new->kern_stack_limit = (void *)kmalloc(STACK_SIZE);
		new->kern_stack_top = new->kern_stack_limit + STACK_SIZE;

		new->user_stack_limit = (void *)VIRT_TO_PHYS(kmalloc(STACK_SIZE));
		new->user_stack_top = new->user_stack_limit + STACK_SIZE;

		new->stack_top = new->user_stack_top;
		new->stack_limit = new->user_stack_limit;

		/* Notice that the below should be unmapped at the end of this func */
		virt_map(pas, (uint64_t) new->user_stack_limit,
				 (uint64_t) new->user_stack_limit, NUM_BLOCKS(STACK_SIZE),
				 VIRT_FLAGS_DEFAULT | VIRT_FLAGS_USERMODE);

		virt_map(as, (uint64_t) new->user_stack_limit,
				 (uint64_t) new->user_stack_limit, NUM_BLOCKS(STACK_SIZE),
				 VIRT_FLAGS_DEFAULT | VIRT_FLAGS_USERMODE);

		mem_map_t m;

		m.virt_addr = (uint64_t) new->user_stack_limit;
		m.phys_addr = (uint64_t) new->user_stack_limit;
		m.np = NUM_BLOCKS(STACK_SIZE);
		m.flags = VIRT_FLAGS_DEFAULT | VIRT_FLAGS_USERMODE;

		vector_pushback(&new->memmap_list, m);

		new_regs = new->user_stack_top - sizeof(task_regs_t);

		new_regs->cs = 0x43;
		new_regs->ss = 0x3b;
	} else {
		new->kern_stack_limit = kmalloc(STACK_SIZE);
		new->kern_stack_top = new->kern_stack_limit + STACK_SIZE;

		new->user_stack_limit = NULL;
		new->user_stack_top = NULL;

		new->stack_top = new->kern_stack_top;
		new->stack_limit = new->kern_stack_limit;

		new_regs = new->kern_stack_top - sizeof(task_regs_t);

		new_regs->cs = 0x28;
		new_regs->ss = 0x30;
	}

	/* If temporarily set to NULL, CR3 switch will be disabled */
	new->ads = as;

	new_regs->rsp = (uint64_t) new->stack_top;
	new_regs->rflags = 0x202;
	new_regs->rip = (uint64_t)entry;
	new_regs->rdi = g_current_taskid;

	new->mode = mode;
	new->stack_top = new_regs;
	new->pid = TASKID_MAX;
	new->priority = priority;
	new->last_tick = 0;
	new->status = TASK_READY;

	// strcpy(new->cwd, "/");
	strncpy(new->name, name, sizeof(new->name));

	klog("created task %d (\"%s\") 0x%x\n", new->id, name, new);

	g_current_taskid++;

	if (mode == USER_TASK) {
		virt_unmap(pas, (uint64_t) new->user_stack_limit,
				   NUM_BLOCKS(STACK_SIZE));
	}

	/* MEMMAP: hpet should be visible for all kernel tasks */
	virt_map(new->ads, (uint64_t)g_hpet, VIRT_TO_PHYS(g_hpet), 1,
			 VIRT_FLAGS_MMIO);

	/* MEMMAP: lapic_base should be visible for all kernel tasks */
	virt_map(new->ads, (uint64_t)g_lapic_addr, VIRT_TO_PHYS(g_lapic_addr), 1,
			 VIRT_FLAGS_MMIO);

	return new;
}