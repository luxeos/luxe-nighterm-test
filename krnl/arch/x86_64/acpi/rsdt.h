/**
 * LuxeOS (c) 2023 by Jozef Nagy
 *
 * LuxeOS is licensed under a
 * Creative Commons Attribution-NoDerivatives 4.0 International License.
 *
 * You should have received a copy of the license along with this
 * work. If not, see <http://creativecommons.org/licenses/by-nd/4.0/>.
 */

#ifndef __RSDT_H_
#define __RSDT_H_

#include <acpi/acpi.h>

typedef struct {
	sdt_t header;
	uint32_t sdt[];
} __attribute__((packed)) rsdt_t;

typedef struct {
	sdt_t header;
	uint64_t sdt[];
} __attribute__((packed)) xsdt_t;

extern xsdt_t *g_xsdt;
extern rsdt_t *g_rsdt;

void rsdt_init();

#endif /* __RSDT_H_ */