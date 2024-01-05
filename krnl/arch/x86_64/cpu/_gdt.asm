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

global _gdt_load
_gdt_load:
	lgdt [rdi]
	push 0x28
	lea rax, [rel .relcs]
	push rax
	retfq
.relcs:
	mov ax, 0x30
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax
	ret