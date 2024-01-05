/**
 * LuxeOS (c) 2023 by Jozef Nagy
 *
 * LuxeOS is licensed under a
 * Creative Commons Attribution-NoDerivatives 4.0 International License.
 *
 * You should have received a copy of the license along with this
 * work. If not, see <http://creativecommons.org/licenses/by-nd/4.0/>.
 */

#ifndef __IRQ_H_
#define __IRQ_H_

#include <int/idt.h>

#include <luxe.h>

#define IRQ_PIT 0

void irq_register(uint8_t irq, interrupt_handler handler);
void irq_deregister(uint8_t irq);

#endif /* __IRQ_H_ */