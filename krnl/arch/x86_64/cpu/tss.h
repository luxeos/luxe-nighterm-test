/**
 * LuxeOS (c) 2023 by Jozef Nagy
 *
 * LuxeOS is licensed under a
 * Creative Commons Attribution-NoDerivatives 4.0 International License.
 *
 * You should have received a copy of the license along with this
 * work. If not, see <http://creativecommons.org/licenses/by-nd/4.0/>.
 */

#ifndef __TSS_H_
#define __TSS_H_

#include <luxe.h>

typedef struct {
	uint32_t limit_low : 16;
	uint32_t base_low : 16;
	uint32_t base_mid : 8;
	uint32_t type : 4;
	uint32_t zero0 : 1;
	uint32_t dpl : 2;
	uint32_t present : 1;
	uint32_t limit_high : 4;
	uint32_t avail : 1;
	uint32_t zero1 : 2;
	uint32_t gran : 1;
	uint32_t base_hi : 8;
	uint32_t base_highest : 32;
	uint32_t reserved0 : 8;
	uint32_t zero2 : 5;
	uint32_t reserved1 : 19;
} __attribute__((packed)) tss_desc_t;

typedef struct {
	uint32_t reserved0;
	uint64_t rsp0;
	uint64_t rsp1;
	uint64_t rsp2;
	uint64_t reserved1;
	uint64_t ist[7];
	uint64_t reserved2;
	uint16_t reserved3;
	uint16_t iopb;
} __attribute__((packed)) tss_t;

#endif /* __TSS_H_ */