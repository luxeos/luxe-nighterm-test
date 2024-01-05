/**
 * LuxeOS (c) 2023 by Jozef Nagy
 *
 * LuxeOS is licensed under a
 * Creative Commons Attribution-NoDerivatives 4.0 International License.
 *
 * You should have received a copy of the license along with this
 * work. If not, see <http://creativecommons.org/licenses/by-nd/4.0/>.
 */

#include <dd/apic/pic.h>
#include <int/idt.h>

#include <luxe.h>

idt_desc_t idt[IDT_MAX_ENTRIES];
interrupt_handler g_int_handlers[16];

extern uint64_t isr_tbl[];

void idt_init()
{
	for (size_t i = 0; i < IDT_MAX_ENTRIES; i++) {
		idt_set_entry(i, isr_tbl[i], IDT_INT_GATE);
	}
	idt_set_entry(255, isr_tbl[255], IDT_TRAP_GATE);

	for (size_t i = 0; i < 16; i++) {
		g_int_handlers[i] = NULL;
	}

	pic_init();
	pic_disable();

	idtr_t idtr;
	idtr.size = sizeof(idt) - 1;
	idtr.base = (uint64_t)&idt;
	_idt_load(&idtr);

	klog("done");
}

void idt_set_entry(uint8_t vector, uint64_t handler, uint8_t flags)
{
	idt_desc_t *entry = &idt[vector];
	entry->base_lo = handler & 0xffff;
	entry->cs = 0x28;
	entry->ist = 0;
	entry->flags = flags;
	entry->base_mid = (handler >> 16) & 0xffff;
	entry->base_hi = handler >> 32;
	entry->reserved = 0;
}