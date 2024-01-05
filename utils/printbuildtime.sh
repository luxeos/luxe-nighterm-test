#!/bin/bash
#
# LuxeOS (c) 2023 by Jozef Nagy
#
# LuxeOS is licensed under a
# Creative Commons Attribution-NoDerivatives 4.0 International License.
#
# You should have received a copy of the license along with this
# work. If not, see <http://creativecommons.org/licenses/by-nd/4.0/>.
#

if [ -z $1 ]; then
	printf "Usage: $0 [start time]\n"
	exit 1
fi

start_build_time=$1
end_build_time=$(date +%s)

build_time=$(($end_build_time-$start_build_time))

if [ $build_time -eq 1 ]; then
	printf "Build time: $build_time second.\n"
else
	printf "Build time: $build_time seconds.\n"
fi
