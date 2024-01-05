/**
 * LuxeOS (c) 2023 by Jozef Nagy
 *
 * LuxeOS is licensed under a
 * Creative Commons Attribution-NoDerivatives 4.0 International License.
 *
 * You should have received a copy of the license along with this
 * work. If not, see <http://creativecommons.org/licenses/by-nd/4.0/>.
 */

#ifndef __IDT_H_
#define __IDT_H_

#include <luxe.h>

#define IDT_MAX_ENTRIES 256

#define IDT_INT_GATE 0x8e
#define IDT_TRAP_GATE 0x8f

typedef struct {
	uint16_t base_lo;
	uint16_t cs;
	uint8_t ist;
	uint8_t flags;
	uint16_t base_mid;
	uint32_t base_hi;
	uint32_t reserved;
} __attribute__((packed)) idt_desc_t;

typedef struct {
	uint16_t size;
	uint64_t base;
} __attribute__((packed)) idtr_t;

typedef void (*interrupt_handler)();

extern void _idt_load(idtr_t *idtr);
extern interrupt_handler g_int_handlers[16];

void idt_init();

void idt_set_entry(uint8_t vector, uint64_t handler, uint8_t flags);

#endif /* __IDT_H_ */