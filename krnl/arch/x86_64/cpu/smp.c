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
#include <boot/boot.h>
#include <cpu/gdt.h>
#include <cpu/tss.h>
#include <cpu/smp.h>
#include <dd/apic/lapic.h>
#include <dd/pit/pit.h>
#include <mem/phys.h>
#include <mem/virt.h>
#include <proc/scheduler.h>

#include <luxe.h>

static volatile int *g_running_cpus = (volatile int *)PHYS_TO_VIRT(0xff0);
static smp_t *g_smp_info = NULL;
static bool g_smp_init_done = false;

void smp_init(void)
{
	g_smp_info = (smp_t *)kmalloc(sizeof(smp_t));
	memset(g_smp_info, 0, sizeof(smp_t));

	virt_map(NULL, 0, 0, NUM_BLOCKS(0x100000), VIRT_FLAGS_DEFAULT);

	uint64_t tramp_size = (uint64_t)&smp_tramp_end - (uint64_t)&smp_tramp_start;
	memcpy((void *)PHYS_TO_VIRT(0x1000), &smp_tramp_start, tramp_size);

	uint64_t *cr3_ptr = (uint64_t *)PHYS_TO_VIRT(0xfd0);
	__asm__ volatile("mov %%cr3, %%rax\n"
					 "mov %%rax, %0"
					 : "=g"(*(cr3_ptr))::"rax");
	uint64_t *idt_ptr = (uint64_t *)PHYS_TO_VIRT(0xfa0);
	__asm__ volatile("sidt %0" : "=m"(*(idt_ptr))::);
	*((uint64_t *)PHYS_TO_VIRT(0xfc0)) = (uint64_t)&smp_ap_entry_point;

	g_smp_info->cpu_count = 0;

	for (uint64_t i = 0; i < g_acpi_cpu_count; i++) {
		memset(&(g_smp_info->cpu[g_smp_info->cpu_count].tss), 0, sizeof(tss_t));
		int ap_cntr = *g_running_cpus;

		if (!(g_acpi_lapic[i]->flags & LAPIC_FLAG_ONLINE) &&
			!(g_acpi_lapic[i]->flags & LAPIC_FLAG_ENABLED)) {
			klog("core %d cannot be brought up, skipping",
				 g_acpi_lapic[i]->acpi_proc_id);
			continue;
		}

		g_smp_info->cpu[g_smp_info->cpu_count].lapic_id =
			g_acpi_lapic[i]->apic_id;
		g_smp_info->cpu[g_smp_info->cpu_count].cpu_id =
			g_acpi_lapic[i]->acpi_proc_id;

		if (_lapic_in(0x20) == g_acpi_lapic[i]->apic_id) {
			klog("core %d is bsp", g_acpi_lapic[i]->acpi_proc_id);
			g_smp_info->cpu[g_smp_info->cpu_count].cpu_is_bsp = true;
			wrmsr(MSR_GS,
				  (uint64_t) & (g_smp_info->cpu[g_smp_info->cpu_count]));
			wrmsr(MSR_KERNEL_GS,
				  (uint64_t) & (g_smp_info->cpu[g_smp_info->cpu_count]));
			for (uint64_t to = 0; to < 100; to++) {
				__asm__ volatile("nop");
			}
			tss_init(&(g_smp_info->cpu[g_smp_info->cpu_count]));
			g_smp_info->cpu_count++;
			continue;
		}

		klog("init core %d", g_acpi_lapic[i]->acpi_proc_id);

		void *stack = kmalloc(STACK_SIZE);
		*((uint64_t *)PHYS_TO_VIRT(0xfb0)) = (uint64_t)stack + STACK_SIZE;
		*((uint64_t *)PHYS_TO_VIRT(0xfe0)) =
			(uint64_t) & (g_smp_info->cpu[g_smp_info->cpu_count]);

		lapic_send_ipi(g_acpi_lapic[i]->apic_id, 0, 0b101);
		hpet_wait(10);

		klog("startup core %d", g_acpi_lapic[i]->acpi_proc_id);
		bool is_up = false;
		for (size_t ipi_count = 0; ipi_count < 2; ipi_count++) {
			lapic_send_ipi(g_acpi_lapic[i]->apic_id, 0x1000 / BLOCK_SIZE,
						   0b110);
			for (size_t wait = 0; wait < 20; wait++) {
				int current_counter = *g_running_cpus;
				if (current_counter != ap_cntr) {
					is_up = true;
					break;
				}
				hpet_wait(10);
			}
			if (is_up) {
				break;
			}
		}

		if (is_up) {
			klog("core %d is up", g_acpi_lapic[i]->acpi_proc_id);
			g_smp_info->cpu[g_smp_info->cpu_count].cpu_is_bsp = false;
		} else {
			kfree(stack);
		}
		g_smp_info->cpu_count++;
	}

	for (;;) {
		if (sched_get_cpu_count() == g_smp_info->cpu_count - 1) {
			break;
		}
		hpet_wait(1);
	}

	virt_unmap(NULL, 0, NUM_BLOCKS(0x100000));
	g_smp_init_done = true;
	klog("done");
}

__attribute__((noreturn)) void smp_ap_entry_point(cpu_t *cpu_info)
{
	gdt_init(cpu_info);
	tss_init(cpu_info);

	wrmsr(MSR_GS, (uint64_t)cpu_info);
	wrmsr(MSR_KERNEL_GS, (uint64_t)cpu_info);

	_lapic_out(0xf0, (1 << 8) | 0xff);

	hpet_wait(10);

	sched_init("init", cpu_info->cpu_id);

	sti();

	for (;;) {
		__asm__ volatile("hlt");
	}
}

cpu_t *smp_cur_cpu(bool force)
{
	if (g_smp_init_done || force) {
		cpu_t *cpu = (cpu_t *)rdmsr(MSR_KERNEL_GS);
		if (cpu == NULL) {
			cpu = (cpu_t *)rdmsr(MSR_GS);
		}
		return cpu;
	} else {
		return NULL;
	}
}

const smp_t *_smp_get_info(void)
{
	return g_smp_info;
}