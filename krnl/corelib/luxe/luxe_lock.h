/**
 * LuxeOS (c) 2023 by Jozef Nagy
 *
 * LuxeOS is licensed under a
 * Creative Commons Attribution-NoDerivatives 4.0 International License.
 *
 * You should have received a copy of the license along with this
 * work. If not, see <http://creativecommons.org/licenses/by-nd/4.0/>.
 */

#ifndef __LUXE_LOCK_H_
#define __LUXE_LOCK_H_

#include <stdint.h>

typedef volatile struct {
	int lock;
	uint64_t rflags;
} lock_t;

#define lock_create() ((lock_t){ 0, 0 })

void lock_acquire(lock_t *lock);
void lock_release(lock_t *lock);

#endif /* __LUXE_LOCK_H_ */