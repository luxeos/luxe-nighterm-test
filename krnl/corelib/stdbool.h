/**
 * LuxeOS (c) 2023 by Jozef Nagy
 *
 * LuxeOS is licensed under a
 * Creative Commons Attribution-NoDerivatives 4.0 International License.
 *
 * You should have received a copy of the license along with this
 * work. If not, see <http://creativecommons.org/licenses/by-nd/4.0/>.
 */

#ifndef __STDBOOL_H_
#define __STDBOOL_H_

#ifndef __cplusplus

// C99 has _Bool already built-in
#define bool _Bool
#define false 0
#define true 1

#endif // __cplusplus

#endif /* __STDBOOL_H_ */