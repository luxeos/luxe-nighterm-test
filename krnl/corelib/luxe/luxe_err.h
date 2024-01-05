/**
 * LuxeOS (c) 2023 by Jozef Nagy
 *
 * LuxeOS is licensed under a
 * Creative Commons Attribution-NoDerivatives 4.0 International License.
 *
 * You should have received a copy of the license along with this
 * work. If not, see <http://creativecommons.org/licenses/by-nd/4.0/>.
 */

#ifndef __LUXE_ERR_H_
#define __LUXE_ERR_H_

// clang-format off
typedef enum {
	PHYS_MM_OUT_OF_MEMORY = 0,
	ASSERT_FAILED,
	PANIC_KERNEL_STACK_CORRUPT,
	TASK_HAS_CORRUPT_ID,
	ERR_UNKNOWN
} err_msg_t;
// clang-format on

#endif /* __LUXE_ERR_H_ */