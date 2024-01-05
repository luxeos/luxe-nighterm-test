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
#include <stdint.h>

#include "luxe_malloc.h"

void *kmalloc(uint64_t size)
{
	memory_metadata_t *alloc = (memory_metadata_t *)PHYS_TO_VIRT(
		phys_alloc(0x0, NUM_BLOCKS(size) + 1));

	alloc->numblocks = NUM_BLOCKS(size);
	alloc->size = size;

	klog("allocated %i bytes of memory", size);

	return ((uint8_t *)alloc) + BLOCK_SIZE;
}

void kfree(void *addr)
{
	memory_metadata_t *d = (memory_metadata_t *)((uint8_t *)addr - BLOCK_SIZE);

	phys_free(VIRT_TO_PHYS(d), d->numblocks + 1);
	klog("freed %i bytes of memory", d->size);
}

void *krealloc(void *addr, size_t newsize)
{
	if (!addr)
		return kmalloc(newsize);

	memory_metadata_t *d = (memory_metadata_t *)((uint8_t *)addr - BLOCK_SIZE);

	if (NUM_BLOCKS(d->size) == NUM_BLOCKS(newsize)) {
		d->size = newsize;
		d->numblocks = NUM_BLOCKS(newsize);
		return addr;
	}

	void *new = kmalloc(newsize);
	if (d->size > newsize)
		memcpy(new, addr, newsize);
	else
		memcpy(new, addr, d->size);

	kfree(addr);
	klog("reallocated %i bytes of memory", newsize);
	return new;
}