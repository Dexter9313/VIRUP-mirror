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

exitcode=0

for i in "${@:2}"; do
	result=$($1 $i -output-replacements-xml | grep -c "replacement ")
	if [ "$result" != "0" ]; then
		echo "File does not comply to clang-format :" $i
		$1 $i > foo
		echo "$(diff $i foo)"
		rm foo
		exitcode=1
	fi
done

exit $exitcode
