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
#include <mem/mmio.h>
#include <mem/phys.h>
#include <mem/virt.h>

#include <luxe.h>

void lapic_init(void)
{
	virt_map(NULL, (uint64_t)g_lapic_addr, VIRT_TO_PHYS(g_lapic_addr), 1,
			 VIRT_FLAGS_MMIO);

	_lapic_out(LAPIC_TPR, 0);
	_lapic_out(LAPIC_DFR, 0xffffffff);
	_lapic_out(LAPIC_LDR, 0x01000000);
	_lapic_out(LAPIC_SVR, 0x100 | 0xff);

	klog("done");
}

uint32_t lapic_get_id(void)
{
	return _lapic_in(LAPIC_ID) >> 24;
}

void lapic_send_init(uint32_t apic_id)
{
	_lapic_out(LAPIC_ICR_HI, apic_id << 24);
	_lapic_out(LAPIC_ICR_LO,
			   ICR_INIT | ICR_PHYS | ICR_ASSERT | ICR_EDGE | ICR_NO_SHORTHAND);
	while (_lapic_in(LAPIC_ICR_LO) & ICR_SEND_PENDING)
		;
}

void lapic_send_ipi(uint8_t dest, uint8_t vector, uint32_t mtype)
{
	_lapic_out(LAPIC_ICR_HI, (uint32_t)dest << 24);
	_lapic_out(LAPIC_ICR_LO, (mtype << 8) | vector);
}

void lapic_send_startup(uint32_t apic_id, uint32_t vector)
{
	_lapic_out(LAPIC_ICR_HI, apic_id << 24);
	_lapic_out(LAPIC_ICR_LO, vector | ICR_STARTUP | ICR_PHYS | ICR_ASSERT |
								 ICR_EDGE | ICR_NO_SHORTHAND);
	while (_lapic_in(LAPIC_ICR_LO) & ICR_SEND_PENDING)
		;
}

uint32_t _lapic_in(uint16_t reg)
{
	return mmio_read((void *)(g_lapic_addr + reg));
}

void _lapic_out(uint16_t reg, uint32_t value)
{
	mmio_write((void *)(g_lapic_addr + reg), value);
}