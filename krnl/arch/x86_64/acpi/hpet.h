/**
 * LuxeOS (c) 2023 by Jozef Nagy
 *
 * LuxeOS is licensed under a
 * Creative Commons Attribution-NoDerivatives 4.0 International License.
 *
 * You should have received a copy of the license along with this
 * work. If not, see <http://creativecommons.org/licenses/by-nd/4.0/>.
 */

#ifndef __HPET_H_
#define __HPET_H_

#include <acpi/acpi.h>

#include <luxe.h>

typedef struct {
	sdt_t header;

	uint8_t hw_rev;
	uint8_t comp_num : 5;
	uint8_t counter_size : 1;
	uint8_t reserved0 : 1;
	uint8_t legacy_repl : 1;
	uint16_t pci_vend_id;

	uint8_t as_id;
	uint8_t reg_width;
	uint8_t reg_off;
	uint8_t reserved1;
	uint64_t addr;

	uint8_t hpet_num;
	uint16_t min_tick;
	uint8_t page_prot;
} __attribute__((packed)) acpi_hpet_t;

typedef struct {
	volatile uint64_t conf_capab;
	volatile uint64_t cmp_val;
	volatile uint64_t fsb_int_route;
	volatile uint64_t unused;
} __attribute__((packed)) hpet_timer_t;

typedef struct {
	volatile uint64_t capab;
	volatile uint64_t unused0;
	volatile uint64_t config;
	volatile uint64_t unused1;
	volatile uint64_t int_status;
	volatile uint64_t unused2;
	volatile uint64_t unused3[2][12];
	volatile uint64_t main_ctr_val;
	volatile uint64_t unused4;
	hpet_timer_t timers[];
} __attribute__((packed)) hpet_t;

extern hpet_t *g_hpet;

void hpet_init(acpi_hpet_t *hpet);
void hpet_wait(uint64_t ms);
uint64_t hpet_get_ns(void);

#endif /* __HPET_H_ */