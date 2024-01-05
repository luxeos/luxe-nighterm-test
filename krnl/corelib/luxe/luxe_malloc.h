/**
 * LuxeOS (c) 2023 by Jozef Nagy
 *
 * LuxeOS is licensed under a
 * Creative Commons Attribution-NoDerivatives 4.0 International License.
 *
 * You should have received a copy of the license along with this
 * work. If not, see <http://creativecommons.org/licenses/by-nd/4.0/>.
 */

#ifndef __LUXE_MALLOC_H_
#define __LUXE_MALLOC_H_

#include <stddef.h>
#include <stdint.h>

typedef struct {
	size_t numblocks;
	size_t size;
} memory_metadata_t;

void *kmalloc(uint64_t size);
void kfree(void *addr);
void *krealloc(void *addr, size_t newsize);

#endif /* __LUXE_MALLOC_H_ */