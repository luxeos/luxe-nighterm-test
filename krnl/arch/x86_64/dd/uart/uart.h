/**
 * LuxeOS (c) 2023 by Jozef Nagy
 *
 * LuxeOS is licensed under a
 * Creative Commons Attribution-NoDerivatives 4.0 International License.
 *
 * You should have received a copy of the license along with this
 * work. If not, see <http://creativecommons.org/licenses/by-nd/4.0/>.
 */

#ifndef __UART_H_
#define __UART_H_

#include <luxe.h>

#define COM1 0x3F8

void uart_init();
void uart_write(char *str);

#endif /* __UART_H_ */