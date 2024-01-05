/**
 * LuxeOS (c) 2023 by Jozef Nagy
 *
 * LuxeOS is licensed under a
 * Creative Commons Attribution-NoDerivatives 4.0 International License.
 *
 * You should have received a copy of the license along with this
 * work. If not, see <http://creativecommons.org/licenses/by-nd/4.0/>.
 */

#include <luxe/luxe_lock.h>

void lock_acquire(lock_t *lock)
{
	__asm__ volatile("pushfq;"
					 "cli;"
					 "lock btsl $0, %[lock];"
					 "jnc 2f;"
					 "1:"
					 "pause;"
					 "btl $0, %[lock];"
					 "jc 1b;"
					 "lock btsl $0, %[lock];"
					 "jc 1b;"
					 "2:"
					 "pop %[flags]"
					 : [lock] "=m"((lock)->lock), [flags] "=m"((lock)->rflags)
					 :
					 : "memory", "cc");
}

void lock_release(lock_t *lock)
{
	__asm__ volatile("push %[flags];"
					 "lock btrl $0, %[lock];"
					 "popfq;"
					 : [lock] "=m"((lock)->lock)
					 : [flags] "m"((lock)->rflags)
					 : "memory", "cc");
}