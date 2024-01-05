/**
 * LuxeOS (c) 2023 by Jozef Nagy
 *
 * LuxeOS is licensed under a
 * Creative Commons Attribution-NoDerivatives 4.0 International License.
 *
 * You should have received a copy of the license along with this
 * work. If not, see <http://creativecommons.org/licenses/by-nd/4.0/>.
 */

#include <boot/boot.h>

#include <dd/fb/fb.h>
#include <debug/klog.h>
#include <dd/uart/uart.h>

#include "printf.h"

#include <luxe.h>

lock_t klog_lock = lock_create();

bool _g_can_use_fb = false;

void klog_init(void)
{
#ifndef CONFIG_DEBUG
	if (boot_parse_cmdline("-v")) {
		_g_can_use_fb = true;
	}
#endif
}

void _klog(char *fmt, ...)
{
	va_list ptr;
	char klog_buffer[4096];

	va_start(ptr, fmt);
	vsnprintf((char *)&klog_buffer, -1, fmt, ptr);

	lock_acquire(&klog_lock);
	uart_write(klog_buffer);
// #ifndef CONFIG_DEBUG
	// if (g_fb_init && _g_can_use_fb) {
	// if (g_fb_init) {
		fb_write(klog_buffer);
	// }
	// }
// #endif
	lock_release(&klog_lock);
	va_end(ptr);
}

void _klog_lock(void)
{
	lock_acquire(&klog_lock);
}

void _klog_unlock(void)
{
	lock_release(&klog_lock);
}