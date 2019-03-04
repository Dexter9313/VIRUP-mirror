#!/bin/bash
#
#    Copyright (C) 2018 Florian Cabot <florian.cabot@hotmail.fr>
#
#    This program is free software; you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation; either version 3 of the License, or
#    (at your option) any later version.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License along
#    with this program; if not, write to the Free Software Foundation, Inc.,
#    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

UNBUFFER=""
if [[ $(command -v unbuffer) ]]; then UNBUFFER="unbuffer" ; fi
result=$($UNBUFFER $1 ${@:2} 2> /dev/null | tee /dev/tty)
result=$(echo "$result" | grep -v "warnings generated\.$" | grep -v "^Suppressed " | grep -v "Use -header-filter=.* to display errors from all non-system headers." | grep -v "^$" | wc -l)
if [ "$result" != "0" ]; then
	exit 1
fi

exit 0
