/**
 * LuxeOS (c) 2023 by Jozef Nagy
 *
 * LuxeOS is licensed under a
 * Creative Commons Attribution-NoDerivatives 4.0 International License.
 *
 * You should have received a copy of the license along with this
 * work. If not, see <http://creativecommons.org/licenses/by-nd/4.0/>.
 */

#include <boot/boot.h>
#include <mem/phys.h>
#include <mem/virt.h>

#include <luxe.h>

static addr_space_t g_kern_ads = { 0 };

vector_struct(mem_map_t) mmap_list = { 0 };

void virt_init()
{
	struct limine_memmap_response *mmap = memmap_request.response;
	struct limine_kernel_address_response *kernel_addr =
		kernel_addr_request.response;

	// just a quick sanity check because I had to
	// hardcode this address for SMP
	assert(hhdm_request.response->offset == 0xffff800000000000);

	g_kern_ads.pml4 = kmalloc(8 * BLOCK_SIZE);
	memset(g_kern_ads.pml4, 0, 8 * BLOCK_SIZE);

	virt_map(NULL, MEM_VIRT_OFF, 0, NUM_BLOCKS(phys_get_highest_block()),
			 VIRT_FLAGS_DEFAULT);
	klog("mapped %llx -> 0x%llx", phys_get_highest_block(), MEM_VIRT_OFF);

	for (size_t i = 0; i < mmap->entry_count; i++) {
		struct limine_memmap_entry *entry = mmap->entries[i];

		switch (entry->type) {
		case LIMINE_MEMMAP_KERNEL_AND_MODULES: {
			uint64_t virt_addr = kernel_addr->virtual_base + entry->base -
								 kernel_addr->physical_base;
			virt_map(NULL, virt_addr, entry->base, NUM_BLOCKS(entry->length),
					 VIRT_FLAGS_DEFAULT);
			klog("mapped kernel 0x%llx -> 0x%llx", entry->base, virt_addr,
				 entry->length);
			break;
		}
		case LIMINE_MEMMAP_FRAMEBUFFER: {
			virt_map(NULL, PHYS_TO_VIRT(entry->base), entry->base,
					 NUM_BLOCKS(entry->length),
					 VIRT_FLAGS_DEFAULT | VIRT_FLAG_WCOMB);
			klog("mapped framebuffer 0x%llx -> 0x%llx", entry->base,
				 PHYS_TO_VIRT(entry->base));
			break;
		}
		case LIMINE_MEMMAP_BOOTLOADER_RECLAIMABLE: {
			break;
		}
		case LIMINE_MEMMAP_RESERVED: {
			klog("refusing to map reserved memory at 0x%llx", entry->base);
			break;
		}
		default: {
			virt_map(NULL, PHYS_TO_VIRT(entry->base), entry->base,
					 NUM_BLOCKS(entry->length),
					 VIRT_FLAGS_DEFAULT | VIRT_FLAGS_USERMODE);
			klog("mapped 0x%llx -> 0x%llx", entry->base,
				 PHYS_TO_VIRT(entry->base));
			break;
		}
		}
	}

	klog("replacing cr3 0x%llx with new value 0x%llx", read_cr3(),
		 VIRT_TO_PHYS(g_kern_ads.pml4));
	write_cr3(VIRT_TO_PHYS(g_kern_ads.pml4));
	klog("done");
}

void virt_map(addr_space_t *ads, uint64_t virt_addr, uint64_t phys_addr,
			  uint64_t np, uint64_t flags)
{
	if (ads == NULL) {
		mem_map_t mm = { virt_addr, phys_addr, flags, np };
		vector_pushback(&mmap_list, mm);
	}

	for (size_t i = 0; i < np * BLOCK_SIZE; i += BLOCK_SIZE) {
		__virt_map(ads, virt_addr + i, phys_addr + i, flags);
	}
}

void virt_unmap(addr_space_t *ads, uint64_t virt_addr, uint64_t np)
{
	if (ads == NULL) {
		size_t len = vector_length(&mmap_list);
		for (size_t i = 0; i < len; i++) {
			mem_map_t m = vector_at(&mmap_list, i);
			if (m.virt_addr != virt_addr) {
				vector_erase(&mmap_list, i);
				break;
			}
		}
	}

	for (size_t i = 0; i < np * BLOCK_SIZE; i += BLOCK_SIZE) {
		__virt_unmap(ads, virt_addr + i);
	}
}

addr_space_t *create_ads()
{
	addr_space_t *as = kmalloc(sizeof(addr_space_t));
	if (!as)
		return NULL;

	memset(as, 0, sizeof(addr_space_t));
	as->pml4 = kmalloc(BLOCK_SIZE * 8);
	if (!as->pml4) {
		kfree(as);
		return NULL;
	}
	memset(as->pml4, 0, BLOCK_SIZE * 8);
	as->lock = lock_create();

	size_t len = vector_length(&mmap_list);
	for (size_t i = 0; i < len; i++) {
		mem_map_t m = vector_at(&mmap_list, i);
		virt_map(as, m.virt_addr, m.phys_addr, m.np, m.flags);
	}

	return as;
}

void __virt_map(addr_space_t *ads, uint64_t virt_addr, uint64_t phys_addr,
				uint64_t flags)
{
	addr_space_t *as = (ads == NULL ? &g_kern_ads : ads);

	uint64_t *pml4 = as->pml4;
	uint16_t pml4e = (virt_addr >> 39) & 0x1ff;
	uint64_t *pdpt = (uint64_t *)PHYS_TO_VIRT(pml4[pml4e] & ~(0xfff));
	if (!(pml4[pml4e] & VIRT_FLAG_PRESENT)) {
		pdpt = (uint64_t *)PHYS_TO_VIRT(phys_alloc(0x0, 8));
		memset(pdpt, 0, BLOCK_SIZE * 8);
		pml4[pml4e] = ((VIRT_TO_PHYS(pdpt) & ~(0xfff)) | VIRT_FLAGS_USERMODE);
		vector_pushback(&as->mem_list, VIRT_TO_PHYS(pdpt));
	}

	uint16_t pdpe = (virt_addr >> 30) & 0x1ff;
	uint64_t *pd = (uint64_t *)PHYS_TO_VIRT(pdpt[pdpe] & ~(0xfff));
	if (!(pdpt[pdpe] & VIRT_FLAG_PRESENT)) {
		pd = (uint64_t *)PHYS_TO_VIRT(phys_alloc(0x0, 8));
		memset(pd, 0, BLOCK_SIZE * 8);
		pdpt[pdpe] = ((VIRT_TO_PHYS(pd) & ~(0xfff)) | VIRT_FLAGS_USERMODE);
		vector_pushback(&as->mem_list, VIRT_TO_PHYS(pd));
	}

	uint16_t pde = (virt_addr >> 21) & 0x1ff;
	uint64_t *pt = (uint64_t *)PHYS_TO_VIRT(pd[pde] & ~(0xfff));
	if (!(pd[pde] & VIRT_FLAG_PRESENT)) {
		pt = (uint64_t *)PHYS_TO_VIRT(phys_alloc(0x0, 8));
		memset(pt, 0, BLOCK_SIZE * 8);
		pd[pde] = ((VIRT_TO_PHYS(pt) & ~(0xfff)) | VIRT_FLAGS_USERMODE);
		vector_pushback(&as->mem_list, VIRT_TO_PHYS(pt));
	}

	uint16_t pte = (virt_addr >> 12) & 0x1ff;
	pt[pte] = (phys_addr | flags);

	uint64_t cr3 = read_cr3();
	if (cr3 == (uint64_t)(VIRT_TO_PHYS(as->pml4))) {
		invlpg((void *)virt_addr);
	}
}

void __virt_unmap(addr_space_t *ads, uint64_t virt_addr)
{
	addr_space_t *as = (ads == NULL ? &g_kern_ads : ads);

	uint64_t *pml4 = as->pml4;
	uint16_t pml4e = (virt_addr >> 39) & 0x1ff;
	if (!(pml4[pml4e] & VIRT_FLAG_PRESENT)) {
		return;
	}

	uint64_t *pdpt = (uint64_t *)PHYS_TO_VIRT(pml4[pml4e] & ~(0x1ff));
	uint16_t pdpe = (virt_addr >> 30) & 0x1ff;
	if (!(pdpt[pdpe] & VIRT_FLAG_PRESENT)) {
		return;
	}

	uint64_t *pd = (uint64_t *)PHYS_TO_VIRT(pdpt[pdpe] & ~(0x1ff));
	uint16_t pde = (virt_addr >> 21) & 0x1ff;
	if (!(pd[pde] & VIRT_FLAG_PRESENT)) {
		return;
	}

	uint64_t *pt = (uint64_t *)PHYS_TO_VIRT(pd[pde] & ~(0x1ff));
	uint16_t pte = (virt_addr >> 12) & 0x1ff;
	if (!(pt[pte] & VIRT_FLAG_PRESENT)) {
		return;
	}

	pt[pte] = 0;
	uint64_t cr3 = read_cr3();
	if (cr3 == (uint64_t)(VIRT_TO_PHYS(as->pml4))) {
		invlpg((void *)virt_addr);
	}

	for (int i = 0; i < 512 * 8; i++) {
		if (pt[i] != 0) {
			return;
		}
	}

	pd[pde] = 0;
	phys_free(VIRT_TO_PHYS(pt), 8);

	for (int i = 0; i < 512 * 8; i++) {
		if (pd[i] != 0) {
			return;
		}
	}

	pdpt[pdpe] = 0;
	phys_free(VIRT_TO_PHYS(pd), 8);

	for (int i = 0; i < 512 * 8; i++) {
		if (pdpt[i] != 0) {
			return;
		}
	}

	pml4[pml4e] = 0;
	phys_free(VIRT_TO_PHYS(pdpt), 8);
}

uint64_t virt_get_phys_addr(addr_space_t *ads, uint64_t virt_addr)
{
	addr_space_t *as = (ads == NULL ? &g_kern_ads : ads);

	uint64_t *pml4 = as->pml4;
	uint16_t pml4e = (virt_addr >> 39) & 0x1ff;
	if (!(pml4[pml4e] & VIRT_FLAG_PRESENT))
		return (uint64_t)NULL;

	uint64_t *pdpt = (uint64_t *)PHYS_TO_VIRT(pml4[pml4e] & ~(0x1ff));
	uint16_t pdpe = (virt_addr >> 30) & 0x1ff;
	if (!(pdpt[pdpe] & VIRT_FLAG_PRESENT))
		return (uint64_t)NULL;

	uint64_t *pd = (uint64_t *)PHYS_TO_VIRT(pdpt[pdpe] & ~(0x1ff));
	uint16_t pde = (virt_addr >> 21) & 0x1ff;
	if (!(pd[pde] & VIRT_FLAG_PRESENT))
		return (uint64_t)NULL;

	uint64_t *pt = (uint64_t *)PHYS_TO_VIRT(pd[pde] & ~(0x1ff));
	uint16_t pte = (virt_addr >> 12) & 0x1ff;
	if (!(pt[pte] & VIRT_FLAG_PRESENT))
		return (uint64_t)NULL;

	return (pt[pte] & ~(0xFFF));
}