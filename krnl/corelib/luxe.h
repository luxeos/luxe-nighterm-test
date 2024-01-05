/**
 * LuxeOS (c) 2023 by Jozef Nagy
 *
 * LuxeOS is licensed under a
 * Creative Commons Attribution-NoDerivatives 4.0 International License.
 *
 * You should have received a copy of the license along with this
 * work. If not, see <http://creativecommons.org/licenses/by-nd/4.0/>.
 */

#ifndef __LUXE_H_
#define __LUXE_H_

#include <config.h>
#include <arch.h>

#include <debug/klog.h>

#include <assert.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include <luxe/luxe_err.h>

#include <luxe/luxe_malloc.h>
#include <luxe/luxe_vector.h>
#include <luxe/luxe_lock.h>
#include <luxe/luxe_version.h>

#include <panic.h>

#if defined(CONFIG_X86_64)
#include <luxe/x86_64/luxe_asm.h>
#endif

#define MAX(a, b)               \
	({                          \
		__typeof__(a) _a = (a); \
		__typeof__(b) _b = (b); \
		_a > _b ? _a : _b;      \
	})

#define MIN(a, b)               \
	({                          \
		__typeof__(a) _a = (a); \
		__typeof__(b) _b = (b); \
		_a < _b ? _a : _b;      \
	})

#endif /* __LUXE_H_ */