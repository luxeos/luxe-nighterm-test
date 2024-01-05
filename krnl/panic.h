/**
 * LuxeOS (c) 2023 by Jozef Nagy
 *
 * LuxeOS is licensed under a
 * Creative Commons Attribution-NoDerivatives 4.0 International License.
 *
 * You should have received a copy of the license along with this
 * work. If not, see <http://creativecommons.org/licenses/by-nd/4.0/>.
 */

#ifndef __PANIC_H_
#define __PANIC_H_

#include <cpu/cpu.h>

extern void panic(int err);

void __panic(cpu_regs_t regs, int err);

void luxeos_print_ver_str(void);

#endif /* __PANIC_H_ */