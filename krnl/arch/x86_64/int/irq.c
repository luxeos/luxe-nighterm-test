/**
 * LuxeOS (c) 2023 by Jozef Nagy
 *
 * LuxeOS is licensed under a
 * Creative Commons Attribution-NoDerivatives 4.0 International License.
 *
 * You should have received a copy of the license along with this
 * work. If not, see <http://creativecommons.org/licenses/by-nd/4.0/>.
 */

#include <int/idt.h>
#include <int/irq.h>

#include <luxe.h>

void irq_register(uint8_t irq, interrupt_handler handler)
{
	g_int_handlers[irq] = handler;
	klog("registered handler for irq %i", irq);
}

void irq_deregister(uint8_t irq)
{
	g_int_handlers[irq] = NULL;
	klog("deregistered handler for irq %i", irq);
}