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

#include <luxe.h>

uint64_t g_highest_block = 0;
uint64_t g_total_size = 0;
uint64_t g_free_size = 0;

uint8_t *g_bitmap = NULL;

void bitmap_set(uint64_t addr, uint64_t blocks)
{
	for (uint64_t i = addr; i < addr + (blocks * BLOCK_SIZE); i += BLOCK_SIZE) {
		g_bitmap[i / (BLOCK_SIZE * BLOCKS_PER_BITMAP)] &=
			~((1 << ((i / BLOCK_SIZE) % BLOCKS_PER_BITMAP)));
	}
}

void bitmap_clear(uint64_t addr, uint64_t blocks)
{
	for (uint64_t i = addr; i < addr + (blocks * BLOCK_SIZE); i += BLOCK_SIZE) {
		g_bitmap[i / (BLOCK_SIZE * BLOCKS_PER_BITMAP)] |=
			1 << ((i / BLOCK_SIZE) % BLOCKS_PER_BITMAP);
	}
}

bool bitmap_test(uint64_t addr, uint64_t blocks)
{
	for (uint64_t i = addr; i < addr + (blocks * BLOCK_SIZE); i += BLOCK_SIZE) {
		if (!(g_bitmap[i / (BLOCK_SIZE * BLOCKS_PER_BITMAP)] &
			  (1 << ((i / BLOCK_SIZE) % BLOCKS_PER_BITMAP)))) {
			return false;
		}
	}
	return true;
}

void phys_init()
{
	struct limine_memmap_response *mmap = memmap_request.response;

	for (size_t i = 0; i < mmap->entry_count; i++) {
		struct limine_memmap_entry *entry = mmap->entries[i];

		klog("entry %i base 0x%llx, length %llu, type %s", i, entry->base,
			 entry->length, _phys_get_type(entry->type));

		if (entry->type == LIMINE_MEMMAP_RESERVED) {
			continue;
		}

		if (entry->type == LIMINE_MEMMAP_USABLE ||
			entry->type == LIMINE_MEMMAP_BOOTLOADER_RECLAIMABLE ||
			entry->type == LIMINE_MEMMAP_ACPI_RECLAIMABLE ||
			entry->type == LIMINE_MEMMAP_KERNEL_AND_MODULES) {
			g_total_size += entry->length;
		}

		uint64_t new_limit = entry->base + entry->length;
		if (new_limit > g_highest_block) {
			g_highest_block = new_limit;
		}
	}

	uint64_t bm_size = g_highest_block / (BLOCK_SIZE * BLOCKS_PER_BITMAP);
	for (size_t i = 0; i < mmap->entry_count; i++) {
		struct limine_memmap_entry *entry = mmap->entries[i];

		if (entry->base + entry->length <= 0x100000)
			continue;

		if (entry->length >= bm_size && entry->type == LIMINE_MEMMAP_USABLE) {
			g_bitmap = (uint8_t *)PHYS_TO_VIRT(entry->base);
			break;
		}
	}

	memset(g_bitmap, 0, bm_size);
	klog("memory bitmap address: 0x%llx", g_bitmap);

	for (size_t i = 0; i < mmap->entry_count; i++) {
		struct limine_memmap_entry *entry = mmap->entries[i];

		if (entry->base + entry->length <= 0x100000)
			continue;

		if (entry->type == LIMINE_MEMMAP_USABLE) {
			phys_free(entry->base, NUM_BLOCKS(entry->length));
		}
	}

	phys_alloc(VIRT_TO_PHYS(g_bitmap), NUM_BLOCKS(bm_size));
	klog("allocated %llu blocks for bitmap", NUM_BLOCKS(bm_size));

	klog("total mem: %llu KiB (%llu MiB)", g_total_size / 1024,
		 g_total_size / (1024 * 1024));
	klog("free mem: %llu KiB (%llu MiB)", g_free_size / 1024,
		 g_free_size / (1024 * 1024));
	klog("used mem: %llu KiB (%llu MiB)", (g_total_size - g_free_size) / 1024,
		 (g_total_size - g_free_size) / (1024 * 1024));

	klog("done");
}

void phys_free(uint64_t addr, uint64_t blocks)
{
	for (uint64_t i = addr; i < addr + (blocks * BLOCK_SIZE); i += BLOCK_SIZE) {
		if (!bitmap_test(i, 1)) {
			g_free_size += BLOCK_SIZE;
		}

		bitmap_clear(i, 1);
	}
}

uint64_t phys_alloc(uint64_t base, uint64_t blocks)
{
	for (uint64_t i = base; i < g_highest_block; i += BLOCK_SIZE) {
		if (bitmap_test(i, blocks)) {
			bitmap_set(i, blocks);
			g_free_size -= blocks * BLOCK_SIZE;
			return i;
		}
	}

	panic(PHYS_MM_OUT_OF_MEMORY);
	return 0; // to make GCC happy
}

void phys_dump()
{
	uint64_t t = g_total_size, f = g_free_size, u = t - f;

	klog("Physical memory usage:\n"
		 "  Total: %8d KB (%4d MB)\n"
		 "  Free : %8d KB (%4d MB)\n"
		 "  Used : %8d KB (%4d MB)",
		 t / 1024, t / (1024 * 1024), f / 1024, f / (1024 * 1024), u / 1024,
		 u / (1024 * 1024));
}

uint64_t phys_get_total_memory()
{
	return g_total_size;
}

uint64_t phys_get_free_memory()
{
	return g_free_size;
}

uint64_t phys_get_highest_block()
{
	return g_highest_block;
}

char *_phys_get_type(uint64_t type)
{
	switch (type) {
	case LIMINE_MEMMAP_USABLE:
		return "Usable";
	case LIMINE_MEMMAP_RESERVED:
		return "Reserved";
	case LIMINE_MEMMAP_ACPI_RECLAIMABLE:
		return "ACPI Reclaimable";
	case LIMINE_MEMMAP_ACPI_NVS:
		return "ACPI NVS";
	case LIMINE_MEMMAP_BAD_MEMORY:
		return "Bad Memory";
	case LIMINE_MEMMAP_BOOTLOADER_RECLAIMABLE:
		return "Bootloader Reclaimable";
	case LIMINE_MEMMAP_KERNEL_AND_MODULES:
		return "Kernel and Modules";
	case LIMINE_MEMMAP_FRAMEBUFFER:
		return "Framebuffer";
	default:
		return "???";
	}
}