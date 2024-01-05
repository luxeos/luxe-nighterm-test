/**
 * LuxeOS (c) 2023 by Jozef Nagy
 *
 * LuxeOS is licensed under a
 * Creative Commons Attribution-NoDerivatives 4.0 International License.
 *
 * You should have received a copy of the license along with this
 * work. If not, see <http://creativecommons.org/licenses/by-nd/4.0/>.
 */

.global smp_tramp_start
.global smp_tramp_end

smp_tramp_start:
    .incbin "build/krnl/arch/x86_64/cpu/_trampoline.bin"
smp_tramp_end:
