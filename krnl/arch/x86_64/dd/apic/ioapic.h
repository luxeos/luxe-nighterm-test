/**
 * LuxeOS (c) 2023 by Jozef Nagy
 *
 * LuxeOS is licensed under a
 * Creative Commons Attribution-NoDerivatives 4.0 International License.
 *
 * You should have received a copy of the license along with this
 * work. If not, see <http://creativecommons.org/licenses/by-nd/4.0/>.
 */

#ifndef __IOAPIC_H_
#define __IOAPIC_H_

#include <luxe.h>

#define IOREGSEL 0x00
#define IOWIN 0x10

#define IOAPICID 0x00
#define IOAPICVER 0x01
#define IOAPICARB 0x02
#define IOREDTBL 0x10

void ioapic_init();
void ioapic_set_entry(uint8_t index, uint64_t data);

uint32_t _ioapic_in(uint8_t reg);
void _ioapic_out(uint8_t reg, uint32_t val);

#endif /* __IOAPIC_H_ */