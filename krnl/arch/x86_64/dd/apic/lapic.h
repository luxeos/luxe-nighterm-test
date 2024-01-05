/**
 * LuxeOS (c) 2023 by Jozef Nagy
 *
 * LuxeOS is licensed under a
 * Creative Commons Attribution-NoDerivatives 4.0 International License.
 *
 * You should have received a copy of the license along with this
 * work. If not, see <http://creativecommons.org/licenses/by-nd/4.0/>.
 */

#ifndef __LAPIC_H_
#define __LAPIC_H_

#include <luxe.h>

#define LAPIC_ID 0x20
#define LAPIC_TPR 0x80
#define LAPIC_DFR 0xe0
#define LAPIC_LDR 0xd0
#define LAPIC_SVR 0xf0
#define LAPIC_ICR_LO 0x300
#define LAPIC_ICR_HI 0x310

#define ICR_INIT 0x500
#define ICR_STARTUP 0x600
#define ICR_PHYS 0x00
#define ICR_SEND_PENDING 0x1000
#define ICR_ASSERT 0x4000
#define ICR_EDGE 0x00
#define ICR_NO_SHORTHAND 0x00

#define LAPIC_FLAG_ENABLED (1 << 0)
#define LAPIC_FLAG_ONLINE (1 << 1)

void lapic_init(void);

uint32_t lapic_get_id(void);

void lapic_send_init(uint32_t apic_id);

void lapic_send_ipi(uint8_t dest, uint8_t vector, uint32_t mtype);
void lapic_send_startup(uint32_t apic_id, uint32_t vector);

uint32_t _lapic_in(uint16_t reg);
void _lapic_out(uint16_t reg, uint32_t value);

#endif /* __LAPIC_H_ */