/**
 * LuxeOS (c) 2023 by Jozef Nagy
 *
 * LuxeOS is licensed under a
 * Creative Commons Attribution-NoDerivatives 4.0 International License.
 *
 * You should have received a copy of the license along with this
 * work. If not, see <http://creativecommons.org/licenses/by-nd/4.0/>.
 */

#include <cpu/smp.h>
#include <cpu/gdt.h>

#include <luxe.h>

// 5 entries for GDT
// 1 entry for TSS
#define GDT_ENTRIES 6

gdt_tbl_t g_gdt_list[CONFIG_CPU_MAX] = { 0 };
size_t g_gdt_num = 0;
tss_t tss;

void gdt_init(cpu_t *cpu_info)
{
	gdt_tbl_t *gdt = (gdt_tbl_t *)&g_gdt_list[g_gdt_num++];
	memset(gdt, 0, sizeof(gdt_tbl_t));

	gdt_set_entry(&(gdt->null), 0, 0, 0);

	gdt_set_entry(&(gdt->kc16), 0, 0xFFFF, AC_RW | AC_EX | AC_PR | AC_ST);
	gdt->kc16.flags = 0;

	gdt_set_entry(&(gdt->kd16), 0, 0xFFFF, AC_RW | AC_PR | AC_ST);
	gdt->kd16.flags = 0;

	gdt_set_entry(&(gdt->kc32), 0, 0xFFFFFFFF, AC_RW | AC_EX | AC_PR | AC_ST);
	gdt->kc32.flags &= 0x0F;
	gdt->kc32.flags |= (GDT_GR | GDT_SZ) << 4;

	gdt_set_entry(&(gdt->kd32), 0, 0xFFFFFFFF, AC_RW | AC_PR | AC_ST);
	gdt->kd32.flags &= 0x0F;
	gdt->kd32.flags |= (GDT_GR | GDT_SZ) << 4;

	gdt_set_entry(&(gdt->kc64), 0, 0xFFFFFFFF,
				  AC_RW | AC_EX | AC_DPL_KERN | AC_PR | AC_ST);

	gdt_set_entry(&(gdt->kd64), 0, 0xFFFFFFFF,
				  AC_RW | AC_DPL_KERN | AC_PR | AC_ST);

	gdt_set_entry(&(gdt->uc64), 0, 0xFFFFFFFF,
				  AC_RW | AC_EX | AC_DPL_USER | AC_PR | AC_ST);

	gdt_set_entry(&(gdt->ud64), 0, 0xFFFFFFFF,
				  AC_RW | AC_DPL_USER | AC_PR | AC_ST);

	gdtr_t gdtr;
	gdtr.size = sizeof(gdt_tbl_t) - 1;
	gdtr.offset = (uint64_t)gdt;
	_gdt_load(&gdtr);

	if (cpu_info != NULL) {
		klog("loaded gdt for cpu %d", cpu_info->cpu_id);
	} else {
		klog("loaded gdt 0x%llx", (uint64_t)gdt);
	}
}

void tss_init(cpu_t *cpu_info)
{
	gdtr_t gdtr;
	__asm__ volatile("sgdt %0" ::"m"(gdtr) : "memory");

	gdt_tbl_t *gdt_table = (gdt_tbl_t *)(gdtr.offset);
	uint64_t addr = (uint64_t)(&cpu_info->tss);

	gdt_table->tss.base_low = (addr >> 8) & 0xff;
	gdt_table->tss.base_mid = (addr >> 16) & 0xff;
	gdt_table->tss.base_hi = (addr >> 24) & 0xff;
	gdt_table->tss.base_highest = (addr >> 32) & 0xffffffff;
	gdt_table->tss.limit_low = 0x67;
	gdt_table->tss.present = 1;
	gdt_table->tss.type = 0x9;

	__asm__ volatile("ltr %%ax" ::"a"(0x48));

	if (cpu_info != NULL) {
		klog("loaded tss for cpu %d with base addr 0x%llx", cpu_info->cpu_id,
			 addr);
	} else {
		klog("loaded tss with base addr 0x%llx", addr);
	}
}

void gdt_set_entry(gdt_t *entry, uint64_t base, uint64_t limit, uint8_t access)
{
	if (limit > 0x10000) {
		limit = limit >> 12;
		entry->flags = (GDT_GR | GDT_LM) << 4;
	} else {
		entry->flags = GDT_GR << 4;
	}

	entry->limit_low = limit & 0xFFFF;
	entry->flags |= (limit >> 16) & 0xF;
	entry->base_low = base & 0xFFFF;
	entry->base_mid = (base >> 16) & 0xFF;
	entry->base_hi = (base >> 24) & 0xFF;
	entry->access = access;
}