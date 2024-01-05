/**
 * LuxeOS (c) 2023 by Jozef Nagy
 *
 * LuxeOS is licensed under a
 * Creative Commons Attribution-NoDerivatives 4.0 International License.
 *
 * You should have received a copy of the license along with this
 * work. If not, see <http://creativecommons.org/licenses/by-nd/4.0/>.
 */

#ifndef __PIT_H_
#define __PIT_H_

#include <luxe.h>

#define PIT_COUNTER0 0x40
#define PIT_CMD 0x43

void pit_init(void);
void pit_handler(void);
uint64_t pit_get_ticks(void);
void pit_wait(uint32_t ms);

#endif /* __PIT_H_*/