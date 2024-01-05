/**
 * LuxeOS (c) 2023 by Jozef Nagy
 *
 * LuxeOS is licensed under a
 * Creative Commons Attribution-NoDerivatives 4.0 International License.
 *
 * You should have received a copy of the license along with this
 * work. If not, see <http://creativecommons.org/licenses/by-nd/4.0/>.
 */

#ifndef __TIMER_H_
#define __TIMER_H_

#include <luxe.h>

enum {
	APIC_TIMER_PERIODIC = 1,
	APIC_TIMER_ONESHOT,
};

void apic_timer_init(void);
void apic_timer_start(void);

void apic_timer_set_handler(void (*handler)(void *));

void apic_timer_set_mode(int mode);
void apic_timer_set_freq(uint64_t ns);

void apic_timer_handler(void);

#endif /* __TIMER_H_ */