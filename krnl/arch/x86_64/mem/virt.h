/**
 * LuxeOS (c) 2023 by Jozef Nagy
 *
 * LuxeOS is licensed under a
 * Creative Commons Attribution-NoDerivatives 4.0 International License.
 *
 * You should have received a copy of the license along with this
 * work. If not, see <http://creativecommons.org/licenses/by-nd/4.0/>.
 */

#ifndef __VIRT_H_
#define __VIRT_H_

#include <boot/boot.h>

#include <luxe.h>

#define MEM_VIRT_OFF hhdm_request.response->offset

#define VIRT_FLAG_PRESENT (1 << 0)
#define VIRT_FLAG_RW (1 << 1)
#define VIRT_FLAG_USER (1 << 2)
#define VIRT_FLAG_WTHRU (1 << 3)
#define VIRT_FLAG_CACHE_DISABLE (1 << 4)
#define VIRT_FLAG_WCOMB (1 << 7)

#define VIRT_FLAGS_DEFAULT (VIRT_FLAG_PRESENT | VIRT_FLAG_RW)
#define VIRT_FLAGS_MMIO (VIRT_FLAGS_DEFAULT | VIRT_FLAG_CACHE_DISABLE)
#define VIRT_FLAGS_USERMODE (VIRT_FLAGS_DEFAULT | VIRT_FLAG_USER)

typedef struct {
	uint64_t virt_addr;
	uint64_t phys_addr;
	uint64_t flags;
	uint64_t np;
} mem_map_t;

typedef struct {
	uint64_t *pml4;
	vector_struct(uint64_t) mem_list;
	lock_t lock;
} addr_space_t;

typedef struct {
	void *virt_start;
	void *phys_start;
	size_t size;
	int block_flags;
} __attribute__((packed)) addr_space_node_t;

void virt_init();

void virt_map(addr_space_t *ads, uint64_t virt_addr, uint64_t phys_addr,
			  uint64_t np, uint64_t flags);
void virt_unmap(addr_space_t *ads, uint64_t virt_addr, uint64_t np);

addr_space_t *create_ads();

void __virt_map(addr_space_t *ads, uint64_t virt_addr, uint64_t phys_addr,
				uint64_t flags);
void __virt_unmap(addr_space_t *ads, uint64_t virt_addr);
uint64_t virt_get_phys_addr(addr_space_t *ads, uint64_t virt_addr);

#endif /* __VIRT_H_ */