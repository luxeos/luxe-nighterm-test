/**
 * LuxeOS (c) 2023 by Jozef Nagy
 *
 * LuxeOS is licensed under a
 * Creative Commons Attribution-NoDerivatives 4.0 International License.
 *
 * You should have received a copy of the license along with this
 * work. If not, see <http://creativecommons.org/licenses/by-nd/4.0/>.
 */

#include <acpi/hpet.h>
#include <dd/apic/lapic.h>
#include <dd/apic/timer.h>
#include <int/idt.h>
#include <int/irq.h>

#include <luxe.h>

static uint8_t g_irq = 0x81;

static uint64_t g_freq = 0;
static uint8_t g_divisor = 0;
static uint8_t g_vector = 0;

void apic_timer_init(void)
{
	g_divisor = 4;
	g_vector = g_irq++;
	klog("g_vector: %d", g_vector);
	assert(g_vector != 0);

	irq_register(g_vector, &apic_timer_handler);

	_lapic_out(0x320, (1 << 16) | g_vector);
	_lapic_out(0x3e0, 1);
	_lapic_out(0x380, UINT32_MAX);

	hpet_wait(100);

	g_freq = ((UINT32_MAX - _lapic_in(0x390)) * 2) * g_divisor;

	klog("base frequency: %d hz, divisor: %d, irq: %d", g_freq, g_divisor,
		 g_vector);
}

void apic_timer_set_handler(void (*handler)(void *))
{
	idt_set_entry(g_irq, (uint64_t)handler, IDT_INT_GATE);
}

void apic_timer_handler(void)
{
}

void apic_timer_start(void)
{
	uint32_t lvt = _lapic_in(0x320);
	_lapic_out(0x320, lvt & ~(1 << 16));
}

void apic_timer_set_mode(int mode)
{
	uint32_t lvt = _lapic_in(0x320);
	if (mode == APIC_TIMER_PERIODIC) {
		_lapic_out(0x320, lvt | (1 << 17));
	} else {
		_lapic_out(0x320, lvt & ~(1 << 17));
	}
}

void apic_timer_set_freq(uint64_t ns)
{
	uint64_t freq = 1000000000 / ns;
	_lapic_out(0x380, g_freq / (freq * g_divisor));
	klog("set apic timer frequency to %d hz", freq);
}