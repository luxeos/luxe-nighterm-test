/**
 * LuxeOS (c) 2023 by Jozef Nagy
 *
 * LuxeOS is licensed under a
 * Creative Commons Attribution-NoDerivatives 4.0 International License.
 *
 * You should have received a copy of the license along with this
 * work. If not, see <http://creativecommons.org/licenses/by-nd/4.0/>.
 */

#ifndef __FB_H_
#define __FB_H_

#include <luxe.h>

typedef struct {
	uint32_t *addr;
	uint16_t width;
	uint16_t height;
	uint16_t pitch;
	uint16_t bpp;
} fb_info_t;

extern fb_info_t g_fb_info;

extern bool g_fb_init;

void fb_init();
void putpixel(int x, int y, uint32_t color);

void fb_write(char *msg);

#endif /* __FB_H_ */