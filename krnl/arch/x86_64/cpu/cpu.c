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

#include <luxe.h>

void cpu_dump_regs(cpu_regs_t regs)
{
	_klog("cr0: 0x%.16llx, cr2: 0x%.16llx, cr3: 0x%.16llx, cr4: 0x%.16llx\n",
		  read_cr0(), read_cr2(), read_cr3(), read_cr4());
	_klog("rax: 0x%.16llx, rbx: 0x%.16llx, rcx: 0x%.16llx, rdx: 0x%.16llx\n",
		  regs.rax, regs.rbx, regs.rcx, regs.rdx);
	_klog("rsp: 0x%.16llx, rbp: 0x%.16llx, rsi: 0x%.16llx, rdi: 0x%.16llx\n",
		  regs.rsp, regs.rbp, regs.rsi, regs.rdi);
	_klog("r8:  0x%.16llx, r9:  0x%.16llx, r10: 0x%.16llx, r11: 0x%.16llx\n",
		  regs.r8, regs.r9, regs.r10, regs.r11);
	_klog("r12: 0x%.16llx, r13: 0x%.16llx, r14: 0x%.16llx, r15: 0x%.16llx\n",
		  regs.r12, regs.r13, regs.r14, regs.r15);
	_klog("rfl: 0x%.16llx, rip: 0x%.16llx, cs:  0x%.16llx, ss:  0x%.16llx\n",
		  regs.rflags, regs.rip, regs.cs, regs.ss);
}