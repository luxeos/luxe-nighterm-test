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
#include <dd/apic/apic.h>
#include <dd/apic/pic.h>
#include <dd/fb/fb.h>
#include <int/excp.h>
#include <int/idt.h>

#include <luxe.h>

static const char *g_exception_str[32] = { "division by zero",
										   "debug",
										   "nonmaskable interrupt",
										   "breakpoint",
										   "overflow",
										   "bound range exceeded",
										   "invalid opcode",
										   "device not available",
										   "double fault",
										   "coprocessor segment overrun",
										   "invalid tss",
										   "segment not present",
										   "stack segment fault",
										   "general protection fault",
										   "page fault",
										   "reserved",
										   "x87 fpu error"
										   "alignment check",
										   "machine check",
										   "simd exception",
										   "virtualization exception",
										   "control protection exception",
										   "reserved",
										   "reserved",
										   "reserved",
										   "reserved",
										   "reserved",
										   "reserved",
										   "hypervisor injection exception",
										   "vmm communication exception",
										   "security exception",
										   "reserved" };

void excp_handler(int_frame_t frame)
{
	if (frame.vector < 0x20) {
		_klog_lock();

		cpu_t *cpu = smp_cur_cpu(false);
		int cpu_num = 1;
		if (cpu != NULL) {
			cpu_num += cpu->cpu_id;
		}

		_klog("\npanic(cpu %d, 0x%.16llx): type %i (%s), registers:\n", cpu_num,
			  frame.rip, frame.vector, g_exception_str[frame.vector]);

		// we don't need to push the registers onto the stack again,
		// so we just pass them
		cpu_regs_t regs = { frame.r15, frame.r14,	 frame.r13, frame.r12,
							frame.r11, frame.r10,	 frame.r9,	frame.r8,
							frame.rbp, frame.rdi,	 frame.rsi, frame.rdx,
							frame.rcx, frame.rbx,	 frame.rax, frame.rip,
							frame.cs,  frame.rflags, frame.rsp, frame.ss };

		cpu_dump_regs(regs);
		luxeos_print_ver_str();

		for (;;) {
			__asm__ volatile("hlt");
		}
	} else if (frame.vector >= 0x20 && frame.vector <= 0x2f) {
		int irq = frame.vector - 0x20;
		if (g_int_handlers[irq] != NULL) {
			g_int_handlers[irq]();
		}

		apic_eoi();
	}
}