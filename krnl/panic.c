/**
 * LuxeOS (c) 2023 by Jozef Nagy
 *
 * LuxeOS is licensed under a
 * Creative Commons Attribution-NoDerivatives 4.0 International License.
 *
 * You should have received a copy of the license along with this
 * work. If not, see <http://creativecommons.org/licenses/by-nd/4.0/>.
 */

#include <cpu/cpu.h>
#include <cpu/smp.h>

#include <luxe.h>

// clang-format off
char *g_error_msg[] = {
	"Out of Physical Memory",
	"Assertion Failed",
	"Kernel Stack Corrupted",
	"Task has corrupted ID",
	"Nighterm failed to initialize",
	"Unknown"
};

char *g_error_name[] = {
	"PHYS_MM_OUT_OF_MEMORY",
	"ASSERT_FAILED",
	"PANIC_KERNEL_STACK_CORRUPT",
	"TASK_HAS_CORRUPT_ID",
	"NIGHTERM_INIT_ERROR",
	"ERR_UNKNOWN"
};
// clang-format on

__attribute__((noreturn)) void __panic(cpu_regs_t regs, int err)
{
	_klog_lock();

	cpu_t *cpu = smp_cur_cpu(false);
	int cpu_num = 1;
	if (cpu != NULL) {
		cpu_num += cpu->cpu_id;
	}

	_klog("\npanic(cpu %d, 0x%8.llx): \"%s\" (%s), registers:\n", cpu_num,
		  regs.rip, g_error_msg[err], g_error_name[err]);

	cpu_dump_regs(regs);
	luxeos_print_ver_str();

	for (;;) {
		__asm__ volatile("cli;hlt");
	}
}

void luxeos_print_ver_str(void)
{
	_klog("\nLuxeOS version:\n%s\n", g_luxeos_rel_str);
	_klog("\nKernel version:\n%s (%s/%s)\n", g_kernel_ver_str,
		  g_kernel_configuration_str, g_kernel_arch_str);
}