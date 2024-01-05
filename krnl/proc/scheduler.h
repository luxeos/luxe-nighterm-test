/**
 * LuxeOS (c) 2023 by Jozef Nagy
 *
 * LuxeOS is licensed under a
 * Creative Commons Attribution-NoDerivatives 4.0 International License.
 *
 * You should have received a copy of the license along with this
 * work. If not, see <http://creativecommons.org/licenses/by-nd/4.0/>.
 */

#ifndef __SCHEDULER_H_
#define __SCHEDULER_H_

#include <proc/tasking.h>

#include <luxe.h>

extern void sched_ctx_switch_enter(void *v);
extern void sched_ctx_switch_exit(task_t *next, uint64_t cr3);
extern void sched_ctx_switch_force(void);

void sched_init(const char *name, uint16_t cpu_id);

task_t *sched_new(const char *name, void (*entry)(uint64_t), bool usermode);
void sched_add(task_t *t);

void sched_ctx_switch(void *stack, int64_t force_switch);

task_event_t sched_wait_for_event(task_event_t event);
bool sched_resume_event(task_event_t event);

uint64_t _sched_get_ticks();

void task_idle(uint64_t id);

void sched_sleep(uint64_t millis);

uint16_t sched_get_cpu_count();

#endif /* __SCHEDULER_H_ */