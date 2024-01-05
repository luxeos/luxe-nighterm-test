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

extern __panic

global panic
panic:
	pushaq
	cld
	call __panic
	ret