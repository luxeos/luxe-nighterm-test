/**
 * LuxeOS (c) 2023 by Jozef Nagy
 *
 * LuxeOS is licensed under a
 * Creative Commons Attribution-NoDerivatives 4.0 International License.
 *
 * You should have received a copy of the license along with this
 * work. If not, see <http://creativecommons.org/licenses/by-nd/4.0/>.
 */

#ifndef __ASSERT_H_
#define __ASSERT_H_

#include <luxe/luxe_err.h>
#include <panic.h>

#define assert(expr)                                                \
	if (!(expr)) {                                                  \
		_klog("\n\nAssertion failed at %s:%d", __FILE__, __LINE__); \
		panic(ASSERT_FAILED);                                       \
	}

#endif /* __ASSERT_H_ */