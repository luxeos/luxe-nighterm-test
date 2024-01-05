/**
 * LuxeOS (c) 2023 by Jozef Nagy
 *
 * LuxeOS is licensed under a
 * Creative Commons Attribution-NoDerivatives 4.0 International License.
 *
 * You should have received a copy of the license along with this
 * work. If not, see <http://creativecommons.org/licenses/by-nd/4.0/>.
 */

#include <dd/pit/pit.h>
#include <int/irq.h>

#include <luxe.h>

volatile uint32_t g_pit_ticks;

void pit_init(void)
{
	uint32_t divisor = 1193182 / 1000;
	outb(PIT_CMD, 0x76);
	outb(PIT_COUNTER0, divisor);
	outb(PIT_COUNTER0, divisor >> 8);

	irq_register(0, pit_handler);

	klog("done");
}

void pit_handler(void)
{
	g_pit_ticks++;
}

uint64_t pit_get_ticks(void)
{
	return g_pit_ticks;
}

void pit_wait(uint32_t ms)
{
	uint32_t now = g_pit_ticks;
	++ms;

	while (g_pit_ticks - now < ms)
		;
}