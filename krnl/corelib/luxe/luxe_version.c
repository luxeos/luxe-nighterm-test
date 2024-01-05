/**
 * LuxeOS (c) 2023 by Jozef Nagy
 *
 * LuxeOS is licensed under a
 * Creative Commons Attribution-NoDerivatives 4.0 International License.
 *
 * You should have received a copy of the license along with this
 * work. If not, see <http://creativecommons.org/licenses/by-nd/4.0/>.
 */

#include <luxe/luxe_version.h>
#include <config.h>

// GCC would shit its pants if I
// statically defined these right in the header

// VERSION STRINGS
char *g_luxeos_rel_str = "0.0.1";
char *g_kernel_ver_str = __GIT_REV__;

#ifdef CONFIG_DEBUG
char *g_kernel_configuration_str = "Debug";
#elif defined CONFIG_RELEASE
char *g_kernel_configuration_str = "Release";
#else
char *g_kernel_configuration_str = "Unknown";
#endif // CONFIG_DEBUG

#ifdef CONFIG_X86_64
char *g_kernel_arch_str = "x86_64";
#else
char *g_kernel_arch_str = "Unknown";
#endif // CONFIG_X86_64