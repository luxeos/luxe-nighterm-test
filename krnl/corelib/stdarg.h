/**
 * LuxeOS (c) 2023 by Jozef Nagy
 *
 * LuxeOS is licensed under a
 * Creative Commons Attribution-NoDerivatives 4.0 International License.
 *
 * You should have received a copy of the license along with this
 * work. If not, see <http://creativecommons.org/licenses/by-nd/4.0/>.
 */

#ifndef __STDARG_H_
#define __STDARG_H_

#ifndef __GNUC_VA_LIST
#define __GNUC_VA_LIST
typedef __builtin_va_list __gnuc_va_list;
#endif

#define va_start(ap, last) __builtin_va_start((ap), last)
#define va_end(ap) __builtin_va_end((ap))
#define va_arg(ap, type) __builtin_va_arg((ap), type)
#define __va_copy(dst, src) __builtin_va_copy((dst), (src))

typedef __gnuc_va_list va_list;

#endif /* __STDARG_H_ */