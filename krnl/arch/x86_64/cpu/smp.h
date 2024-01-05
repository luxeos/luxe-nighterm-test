/**
 * LuxeOS (c) 2023 by Jozef Nagy
 *
 * LuxeOS is licensed under a
 * Creative Commons Attribution-NoDerivatives 4.0 International License.
 *
 * You should have received a copy of the license along with this
 * work. If not, see <http://creativecommons.org/licenses/by-nd/4.0/>.
 */

#ifndef __SMP_H_
#define __SMP_H_

#include <cpu/tss.h>
#include <mem/phys.h>

#include <luxe.h>

#define STACK_SIZE (BLOCK_SIZE * 32)

typedef struct {
	int64_t errno;
	tss_t tss;
	uint16_t cpu_id;
	uint16_t lapic_id;
	bool cpu_is_bsp;
	uint8_t reserved[3];
} __attribute__((packed)) cpu_t;

typedef struct {
	cpu_t cpu[CONFIG_CPU_MAX];
	uint16_t cpu_count;
} smp_t;

extern uint8_t smp_tramp_start;
extern uint8_t smp_tramp_end;

void smp_init(void);

void smp_ap_entry_point(cpu_t *cpu_info);

cpu_t *smp_cur_cpu(bool force);
const smp_t *_smp_get_info(void);

#endif /* __SMP_H_ */