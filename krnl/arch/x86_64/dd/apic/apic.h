/**
 * LuxeOS (c) 2023 by Jozef Nagy
 *
 * LuxeOS is licensed under a
 * Creative Commons Attribution-NoDerivatives 4.0 International License.
 *
 * You should have received a copy of the license along with this
 * work. If not, see <http://creativecommons.org/licenses/by-nd/4.0/>.
 */

#ifndef __APIC_H_
#define __APIC_H_

#define IA32_APIC_BASE_MSR 0x1b

void apic_init(void);
void apic_eoi(void);

#endif /* __APIC_H_ */