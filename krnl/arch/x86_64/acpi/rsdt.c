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
#include <acpi/rsdt.h>
#include <acpi/madt.h>
#include <acpi/hpet.h>
#include <mem/phys.h>

#include <luxe.h>

xsdt_t *g_xsdt;
rsdt_t *g_rsdt;

void rsdt_init()
{
	rsdp_t *rsdp = (rsdp_t *)rsdp_request.response->address;

	g_rsdt = (rsdt_t *)(uintptr_t)PHYS_TO_VIRT(rsdp->rsdt_addr);
	if (_use_xsdt()) {
		xsdp_t *xsdp = (xsdp_t *)rsdp_request.response->address;
		g_xsdt = (xsdt_t *)(uintptr_t)PHYS_TO_VIRT(xsdp->xsdt_addr);
	}

	// we'll find all sdt's here
	//
	// the general idea of parsing is like this:
	//	- find table
	//	- if the table is not found, panic
	//	- fill the appropriate structure
	//	- call the table's init function
	//
	// table = (table_t *)_find_sdt(use_xsdt, "SIG_");
	// assert(table != NULL);
	// table_init(table);
	madt_t *madt = _find_sdt("APIC");
	assert(madt != NULL);
	madt_init(madt);

	acpi_hpet_t *hpet = _find_sdt("HPET");
	assert(hpet != NULL);
	hpet_init(hpet);

	klog("done");
}