/**
 * LuxeOS (c) 2023 by Jozef Nagy
 *
 * LuxeOS is licensed under a
 * Creative Commons Attribution-NoDerivatives 4.0 International License.
 *
 * You should have received a copy of the license along with this
 * work. If not, see <http://creativecommons.org/licenses/by-nd/4.0/>.
 */

#include <stdint.h>
#include <string.h>

void *memset(void *dest, int val, uint64_t n)
{
	uint8_t *ptr = dest;
	while (n--) {
		*ptr++ = (uint8_t)val;
	}
	return dest;
}

void memcpy(void *dest, const void *src, size_t n)
{
	char *a = (char *)src;
	char *b = (char *)dest;

	for (size_t i = 0; i < n; i++) {
		b[i] = a[i];
	}
}

int strncmp(const char *s1, const char *s2, size_t n)
{
	while (n && *s1 && (*s1 == *s2)) {
		++s1;
		++s2;
		--n;
	}
	if (n == 0) {
		return 0;
	} else {
		return (*(uint8_t *)s1 - *(uint8_t *)s2);
	}
}

char *strncpy(char *dest, const char *src, size_t n)
{
	size_t i;
	for (i = 0; i < n; i++) {
		dest[i] = src[i];
		if (src[i] == '\0') {
			break;
		}
	}
	return dest + i;
}

int strlen(const char *str)
{
	int n = 0;
	while (*str != '\0') {
		n++;
		str++;
	}

	return n;
}