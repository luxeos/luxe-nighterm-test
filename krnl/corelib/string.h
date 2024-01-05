/**
 * LuxeOS (c) 2023 by Jozef Nagy
 *
 * LuxeOS is licensed under a
 * Creative Commons Attribution-NoDerivatives 4.0 International License.
 *
 * You should have received a copy of the license along with this
 * work. If not, see <http://creativecommons.org/licenses/by-nd/4.0/>.
 */

#ifndef __STRING_H_
#define __STRING_H_

#include <stdint.h>

void *memset(void *dest, int val, uint64_t n);
void memcpy(void *dest, const void *src, size_t n);

int strncmp(const char *s1, const char *s2, size_t n);
char *strncpy(char *dest, const char *src, size_t n);
int strlen(const char *str);

#endif /* __STRING_H_ */