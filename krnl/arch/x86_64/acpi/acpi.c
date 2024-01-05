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

#include <boot/boot.h>
#include <mem/phys.h>

#include <luxe.h>

bool _xsdt_is_available;

void acpi_init()
{
	klog("rsdp is at 0x%8llx", rsdp_request.response->address);

	rsdp_t *rsdp = (rsdp_t *)rsdp_request.response->address;
	if (rsdp->revision == 0) {
		_xsdt_is_available = false;
		rsdt_init();
	} else if (rsdp->revision >= 2) {
		_xsdt_is_available = true;
		rsdt_init();
	}

	klog("done");
}

void *_find_sdt(char *signature)
{
	int entry_divisor = (_use_xsdt() ? 8 : 4);
	int entries =
		(g_rsdt->header.length - sizeof(g_xsdt->header)) / entry_divisor;
	for (int i = 0; i < entries; i++) {
		sdt_t *header = NULL;
		if (_use_xsdt()) {
			header = (sdt_t *)(uintptr_t)PHYS_TO_VIRT(g_xsdt->sdt[i]);
		} else {
			header = (sdt_t *)(uintptr_t)PHYS_TO_VIRT(g_rsdt->sdt[i]);
		}
		if (!strncmp(header->signature, signature, 4)) {
			klog("found header with signature %.4s", header->signature);
			return (void *)header;
		}
	}

	return NULL;
}

bool _use_xsdt()
{
	return _xsdt_is_available;
}