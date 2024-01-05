/**
 * LuxeOS (c) 2023 by Jozef Nagy
 *
 * LuxeOS is licensed under a
 * Creative Commons Attribution-NoDerivatives 4.0 International License.
 *
 * You should have received a copy of the license along with this
 * work. If not, see <http://creativecommons.org/licenses/by-nd/4.0/>.
 */

#ifndef __BOOT_H_
#define __BOOT_H_

#include <boot/limine.h>
#include <luxe.h>

extern volatile struct limine_memmap_request memmap_request;
extern volatile struct limine_hhdm_request hhdm_request;
extern volatile struct limine_kernel_address_request kernel_addr_request;
extern volatile struct limine_rsdp_request rsdp_request;
extern volatile struct limine_framebuffer_request framebuffer_request;
extern volatile struct limine_smp_request smp_request;
extern volatile struct limine_kernel_file_request kern_file_request;

bool boot_parse_cmdline(char *look_for);

#endif /* __BOOT_H_ */