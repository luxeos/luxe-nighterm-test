/**
 * LuxeOS (c) 2023 by Jozef Nagy
 *
 * LuxeOS is licensed under a
 * Creative Commons Attribution-NoDerivatives 4.0 International License.
 *
 * You should have received a copy of the license along with this
 * work. If not, see <http://creativecommons.org/licenses/by-nd/4.0/>.
 */

#ifndef __ACPI_H_
#define __ACPI_H_

#include <luxe.h>

typedef struct {
	char signature[8]; // "RSDT PTR "
	uint8_t checksum;
	char oem_id[6];
	uint8_t revision;
	uint32_t rsdt_addr;
} __attribute__((packed)) rsdp_t;

typedef struct {
	rsdp_t rsdp;
	uint32_t length;
	uint64_t xsdt_addr;
	uint8_t checksum;
	uint8_t reserved[3];
} __attribute__((packed)) xsdp_t;

typedef struct {
	char signature[4];
	uint32_t length;
	uint8_t revision;
	uint8_t checksum;
	char oem_id[6];
	char oem_tableid[8];
	uint32_t oem_revision;
	uint32_t creator_id;
	uint32_t creator_revision;
	// the specific tables follow here
} __attribute__((packed)) sdt_t;

void acpi_init();

void *_find_sdt(char *signature);
bool _use_xsdt();

#endif /* __ACPI_H_ */