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
#include <acpi/hpet.h>
#include <dd/pit/pit.h>
#include <mem/phys.h>
#include <mem/virt.h>

#include <luxe.h>

hpet_t *g_hpet = NULL;

static uint64_t g_hpet_period = 0;

void hpet_init(acpi_hpet_t *hpet)
{
	virt_map(NULL, PHYS_TO_VIRT(hpet->addr), (uint64_t)hpet->addr, 1,
			 VIRT_FLAGS_MMIO);

	g_hpet = (hpet_t *)PHYS_TO_VIRT(hpet->addr);

	if (!(g_hpet->capab & (1 << 15))) {
		klog("hpet is not capable of legacy replacement");
		g_hpet = NULL;
		return;
	}

	uint64_t ctr_clk_per = g_hpet->capab >> 32;
	uint64_t freq = 1000000000000000 / ctr_clk_per;

	klog("hpet frequency %llu hz", freq);
	g_hpet_period = ctr_clk_per / 1000000;

	g_hpet->config = g_hpet->config | 1;

	klog("done");
}

void hpet_wait(uint64_t ms)
{
	uint64_t ns = ms * 1000000;

	uint64_t now = hpet_get_ns();
	uint64_t target = hpet_get_ns() + ns;

	while (hpet_get_ns() >= target && hpet_get_ns() <= now) {
		__asm__ volatile("hlt");
	}
}

uint64_t hpet_get_ns()
{
	if (g_hpet == NULL) {
		return pit_get_ticks();
	}

	return g_hpet->main_ctr_val * g_hpet_period;
}