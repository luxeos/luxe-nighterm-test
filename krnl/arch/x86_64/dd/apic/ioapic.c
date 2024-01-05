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
#include <dd/apic/ioapic.h>
#include <mem/mmio.h>
#include <mem/phys.h>
#include <mem/virt.h>

#include <luxe.h>

void ioapic_init()
{
	virt_map(NULL, (uint64_t)g_ioapic_addr, VIRT_TO_PHYS(g_ioapic_addr), 1,
			 VIRT_FLAGS_MMIO);

	uint32_t max_redir_entries = ((_ioapic_in(IOAPICVER) >> 16) & 0xff) + 1;

	klog("ioapic pins: %d", max_redir_entries);
	for (uint32_t i = 0; i < max_redir_entries; ++i) {
		ioapic_set_entry(i, 1 << 16);
	}

	klog("done");
}

void ioapic_set_entry(uint8_t index, uint64_t data)
{
	_ioapic_out(IOREDTBL + index * 2, (uint32_t)data);
	_ioapic_out(IOREDTBL + index * 2 + 1, (uint32_t)(data >> 32));
}

uint32_t _ioapic_in(uint8_t reg)
{
	mmio_write((void *)(g_ioapic_addr + IOREGSEL), reg);
	return mmio_read((void *)(g_ioapic_addr + IOWIN));
}

void _ioapic_out(uint8_t reg, uint32_t val)
{
	mmio_write((void *)(g_ioapic_addr + IOREGSEL), reg);
	mmio_write((void *)(g_ioapic_addr + IOWIN), val);
}