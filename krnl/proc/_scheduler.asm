;
; LuxeOS (c) 2023 by Jozef Nagy
;
; LuxeOS is licensed under a
; Creative Commons Attribution-NoDerivatives 4.0 International License.
;
; You should have received a copy of the license along with this
; work. If not, see <http://creativecommons.org/licenses/by-nd/4.0/>.
;
;

%include "krnl/arch/x86_64/_utils.asm"

global sched_ctx_switch_enter
global sched_ctx_switch_exit
global sched_ctx_switch_force

extern sched_ctx_switch
extern lock_release

sched_ctx_switch_enter:
	pushaq
	mov rdi, rsp
	mov rsi, 0
	call sched_ctx_switch
	add rsp, 120
	iretq

sched_ctx_switch_exit:
	test rsi, rsi
	jz ._dont_load_cr3
	mov cr3, rsi
._dont_load_cr3:
	mov rsp, rdi
	popaq
	iretq

sched_ctx_switch_force:
	cli
	mov rax, rsp
	push qword 0x30
	push rax
	push qword 0x202
	push qword 0x28
	mov rax, ._exit
	push rax
	pushaq
	mov rdi, rsp
	mov rsi, 1
	call sched_ctx_switch
	add rsp, 120
	iretq
._exit:
	ret