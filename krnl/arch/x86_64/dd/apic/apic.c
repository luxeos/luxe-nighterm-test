/**
 * LuxeOS (c) 2023 by Jozef Nagy
 *
 * LuxeOS is licensed under a
 * Creative Commons Attribution-NoDerivatives 4.0 International License.
 *
 * You should have received a copy of the license along with this
 * work. If not, see <http://creativecommons.org/licenses/by-nd/4.0/>.
 */

#include <acpi/madt.h>
#include <dd/apic/apic.h>
#include <dd/apic/lapic.h>
#include <dd/apic/ioapic.h>
#include <mem/mmio.h>
#include <mem/phys.h>
#include <int/irq.h>

#include <luxe.h>

void apic_init(void)
{
	lapic_init();
	ioapic_init();

	// this is already done on qemu, but still better to enable it just in case
	uint64_t msr = rdmsr(IA32_APIC_BASE_MSR);
	msr |= (1 << 11);
	wrmsr(IA32_APIC_BASE_MSR, msr);

	ioapic_set_entry(madt_get_iso(IRQ_PIT), (0x20 + IRQ_PIT));

	_lapic_out(0xf0, (1 << 8) | 0xff);

	klog("done");
}

void apic_eoi(void)
{
	_lapic_out(0xb0, 0);
}