/**
 * LuxeOS (c) 2023 by Jozef Nagy
 *
 * LuxeOS is licensed under a
 * Creative Commons Attribution-NoDerivatives 4.0 International License.
 *
 * You should have received a copy of the license along with this
 * work. If not, see <http://creativecommons.org/licenses/by-nd/4.0/>.
 */

#include <acpi/acpi.h>
#include <boot/bootlogo.h>
#include <cpu/gdt.h>
#include <cpu/smp.h>
#include <int/idt.h>
#include <dd/uart/uart.h>
#include <dd/apic/apic.h>
#include <dd/apic/timer.h>
#include <dd/pit/pit.h>
#include <dd/fb/fb.h>
#include <mem/phys.h>
#include <mem/virt.h>

#include <luxe.h>

void arch_init()
{
	uart_init();
	fb_init();
	display_bootlogo();

	gdt_init(NULL);
	idt_init();

	phys_init();
	virt_init();

	// irq devices init
	pit_init();

	acpi_init();
	apic_init();

	sti();

	// smp_init();
}