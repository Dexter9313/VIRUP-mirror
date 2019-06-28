/*
    Copyright (C) 2018 Florian Cabot

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#ifndef EXCEPTIONS_DEFAULT_HPP
#define EXCEPTIONS_DEFAULT_HPP

#include <iostream>

void print_stacktrace(bool calledFromSigHand);
void init_exceptions(char* programName);

// default error message
inline void print_stacktrace(bool calledFromSigHand)
{
	(void) calledFromSigHand;
	std::cerr << "A stacktrace cannot be printed on this system." << std::endl;
}

// by default, do nothing
inline void init_exceptions(char* programName)
{
	(void) programName;
}

#endif // EXCEPTIONS_DEFAULT_HPP
