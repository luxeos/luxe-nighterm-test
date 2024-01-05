/**
 * LuxeOS (c) 2023 by Jozef Nagy
 *
 * LuxeOS is licensed under a
 * Creative Commons Attribution-NoDerivatives 4.0 International License.
 *
 * You should have received a copy of the license along with this
 * work. If not, see <http://creativecommons.org/licenses/by-nd/4.0/>.
 */

#ifndef __MMIO_H_
#define __MMIO_H_

#include <luxe.h>

void mmio_write(void *reg, uint32_t val);
uint32_t mmio_read(void *reg);

#endif /* __MMIO_H_ */