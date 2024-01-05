/**
 * LuxeOS (c) 2023 by Jozef Nagy
 *
 * LuxeOS is licensed under a
 * Creative Commons Attribution-NoDerivatives 4.0 International License.
 *
 * You should have received a copy of the license along with this
 * work. If not, see <http://creativecommons.org/licenses/by-nd/4.0/>.
 */

#ifndef __GDT_H_
#define __GDT_H_

#include <cpu/smp.h>

#include <luxe.h>
#define AC_AC 0x1 /* access */
#define AC_RW 0x2 /* readable for code & writeable for data selector */
#define AC_DC 0x4 /* direction */
#define AC_EX 0x8 /* executable, code segment */
#define AC_ST \
	0x10 /* Descriptor type bit. If clear (0) the descriptor
                             * defines a system segment (eg. a Task State Segment).
                             * If set (1) it defines a code or data segment
                             */
#define AC_PR \
	0x80 /* persent in memory, Must be set (1) for any valid
                             * segment.
                             */
#define AC_DPL_KERN 0x0 /* RING 0 kernel level */
#define AC_DPL_USER 0x60 /* RING 3 user level - 01100000b */

#define GDT_GR \
	0x8 /* Granularity flag, indicates the size the Limit
                             * value is scaled by. If clear (0), the Limit is
                             * in 1 Byte blocks (byte granularity). If set (1),
                             * the Limit is in 4 KiB blocks (page granularity).
                             */
#define GDT_SZ 0x4 /* size bt, 32 bit protect mode */
#define GDT_LM \
	0x2 /* Long-mode code flag. If set (1), the descriptor
                             * defines a 64-bit code segment. When set, DB
                             * should always be clear. For any other type of
                             * segment (other code types or any data segment),
                             * it should be clear (0).
                             */
/* gdt selector */
#define SEL_KCODE 0x1
#define SEL_KDATA 0x2
#define SEL_UCODE 0x3
#define SEL_UDATA 0x4
#define SEL_TSS 0x5

/* request privilege level */
#define RPL_KERN 0x0
#define RPL_USER 0x3

/* current privilege level */
#define CPL_KERN 0x0
#define CPL_USER 0x3

typedef struct {
	uint16_t limit_low;
	uint16_t base_low;
	uint8_t base_mid;
	uint8_t access;
	uint8_t flags;
	uint8_t base_hi;
} __attribute__((packed)) gdt_t;

typedef struct {
	gdt_t null;
	gdt_t kc16;
	gdt_t kd16;
	gdt_t kc32;
	gdt_t kd32;
	gdt_t kc64;
	gdt_t kd64;
	gdt_t ud64;
	gdt_t uc64;
	tss_desc_t tss;
} __attribute__((packed)) gdt_tbl_t;

typedef struct {
	uint16_t size;
	uint64_t offset;
} __attribute__((packed)) gdtr_t;

void tss_init(cpu_t *cpu_info);

void gdt_init(cpu_t *cpu_info);
void gdt_set_entry(gdt_t *entry, uint64_t base, uint64_t limit, uint8_t access);

extern void _gdt_load(gdtr_t *gdtr);

#endif /* __GDT_H_ */