/**
 * LuxeOS (c) 2023 by Jozef Nagy
 *
 * LuxeOS is licensed under a
 * Creative Commons Attribution-NoDerivatives 4.0 International License.
 *
 * You should have received a copy of the license along with this
 * work. If not, see <http://creativecommons.org/licenses/by-nd/4.0/>.
 */

#ifndef __PHYS_H_
#define __PHYS_H_

#include <mem/virt.h>

#include <luxe.h>

#define HHDM_OFFSET 0xffff800000000000

#define BLOCK_SIZE 0x1000
#define BLOCKS_PER_BITMAP 8

#define NUM_BLOCKS(num) (((num) + BLOCK_SIZE - 1) / BLOCK_SIZE)

#define VIRT_TO_PHYS(addr) ((uint64_t)(addr)-0xffff800000000000)
#define PHYS_TO_VIRT(addr) ((uint64_t)(addr) + 0xffff800000000000)

void bitmap_set(uint64_t addr, uint64_t blocks);
void bitmap_clear(uint64_t addr, uint64_t blocks);
bool bitmap_test(uint64_t addr, uint64_t blocks);

void phys_init();

uint64_t phys_alloc(uint64_t base, uint64_t blocks);
void phys_free(uint64_t addr, uint64_t blocks);

void phys_dump();

uint64_t phys_get_total_memory();
uint64_t phys_get_free_memory();
uint64_t phys_get_highest_block();

char *_phys_get_type(uint64_t type);

#endif /* __PHYS_H_ */