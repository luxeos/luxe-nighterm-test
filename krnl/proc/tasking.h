/**
 * LuxeOS (c) 2023 by Jozef Nagy
 *
 * LuxeOS is licensed under a
 * Creative Commons Attribution-NoDerivatives 4.0 International License.
 *
 * You should have received a copy of the license along with this
 * work. If not, see <http://creativecommons.org/licenses/by-nd/4.0/>.
 */

#ifndef __TASKING_H_
#define __TASKING_H_

#include <mem/virt.h>

#include <luxe.h>

#define TASKID_MAX UINT64_MAX

typedef enum {
	KERNEL_TASK,
	USER_TASK,
} task_mode_t;

typedef enum {
	TASK_READY,
	TASK_RUNNING,
	TASK_SLEEPING,
	TASK_DEAD,
	TASK_UNKNOWN
} task_status_t;

typedef enum {
	EVENT_UNDEFINED = 1,
	EVENT_KEYPRESS,
} event_type_t;

typedef struct {
	uint64_t r15;
	uint64_t r14;
	uint64_t r13;
	uint64_t r12;
	uint64_t r11;
	uint64_t r10;
	uint64_t r9;
	uint64_t r8;
	uint64_t rbp;
	uint64_t rdi;
	uint64_t rsi;
	uint64_t rdx;
	uint64_t rcx;
	uint64_t rbx;
	uint64_t rax;
	uint64_t rip;
	uint64_t cs;
	uint64_t rflags;
	uint64_t rsp;
	uint64_t ss;
} __attribute__((packed)) task_regs_t;

typedef struct {
	uint64_t pub_id;
	uint64_t sub_id;
	uint64_t type;
	uint64_t para;
	uint64_t timestamp;
} task_event_t;

typedef struct {
	uint64_t entry;
	uint64_t phdrs;
	uint64_t phaddr;
	uint16_t phentsize;
	uint16_t phnum;
	uint64_t shdr;
	uint16_t shnum;
} __attribute__((packed)) auxval_t;

typedef struct task {
	void *stack_top;
	void *stack_limit;

	void *kern_stack_top;
	void *kern_stack_limit;

	void *user_stack_top;
	void *user_stack_limit;

	uint64_t id;
	uint64_t pid;
	uint8_t priority;
	uint64_t last_tick;
	uint64_t wake_time;
	task_event_t wake_event;
	task_status_t status;
	task_mode_t mode;

	auxval_t aux;
	vector_struct(uint64_t) child_list;

	int64_t errno;

	addr_space_t *ads;
	vector_struct(mem_map_t) memmap_list;
	uint64_t fs_base;

	char cwd[256];
	char name[64];
} task_t;

task_t *task_create(const char *name, void (*entry)(uint64_t), uint8_t priority,
					task_mode_t mode, addr_space_t *pas);

#endif /* __TASKING_H_ */