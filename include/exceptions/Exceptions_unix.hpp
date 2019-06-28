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

// DISCLAIMER : most of the code in this file has been adapted from this
// article, so Prograde doesn't own some bits of it :
// *Catching Exceptions and Printing Stack Traces for C on Windows, Linux, &
// Mac* written by Job Vranish
// https://spin.atomicobject.com/2013/01/13/exceptions-stack-traces-c/

#ifndef EXCEPTIONS_UNIX_HPP
#define EXCEPTIONS_UNIX_HPP

#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <execinfo.h>
#include <iostream>
#include <stdint.h>

#define MAX_BACKTRACE_LINES 64

/*! Encapsulation of the _programName global variable. If you read this in
 * the documentation, you can ignore this implementation class.
 */
class Exceptions
{
  public:
	/*! \{ */
	static char*& getProgramName()
	{
		static char* _programName;
		return _programName;
	}
	/*! \} */
};

void print_stacktrace(bool calledFromSigHand);
void posix_signal_handler(int sig);
void set_signal_handler(sig_t handler);
void init_exceptions(char* programName);
int addr2line(char const* const program_name, void const* const addr,
              int lineNb);

// prints formated stack trace with the most information as possible
// parameter indicates if the function is called by the signal handler or not
//(to hide the call to the signal handler)
inline void print_stacktrace(bool calledFromSigHand)
{
	void* buffer[MAX_BACKTRACE_LINES];
	char** strings;

	int nptrs = backtrace(buffer, MAX_BACKTRACE_LINES);
	strings   = backtrace_symbols(buffer, nptrs);

	if(strings == NULL)
	{
		perror("backtrace_symbols");
		exit(EXIT_FAILURE);
	}

	int i = 1;

	// we don't want to print the signal handler call in the stack trace
	if(calledFromSigHand)
		++i;

	for(; i < nptrs - 2; ++i)
	{
		// if addr2line failed, print what we can anyway
		if(addr2line(Exceptions::getProgramName(), buffer[i], nptrs - 2 - i - 1)
		   != 0)
			std::cerr << "[" << nptrs - 2 - i - 1 << "] " << strings[i]
			          << std::endl;
	}

	free(strings);
}

inline void posix_signal_handler(int sig)
{
	print_stacktrace(true);

	switch(sig)
	{
		case SIGABRT:
			std::cerr
			    << "Caught SIGABRT: usually caused by an abort() or assert()"
			    << std::endl;
			break;

		case SIGFPE:
			std::cerr
			    << "Caught SIGFPE: arithmetic exception, such as divide by zero"
			    << std::endl;
			;
			break;

		case SIGILL:
			std::cerr << "Caught SIGILL: illegal instruction" << std::endl;
			;
			break;

		case SIGINT:
			std::cerr << "Caught SIGINT: interactive attention signal, "
			             "probably a ctrl+c"
			          << std::endl;
			;
			break;

		case SIGSEGV:
			std::cerr << "Caught SIGSEGV: segfault" << std::endl;
			;
			break;

		case SIGTERM:
		default:
			std::cerr << "Caught SIGTERM: a termination request was sent to "
			             "the program"
			          << std::endl;
			;
			break;
	}

	_Exit(EXIT_FAILURE);
}

inline void set_signal_handler(sig_t handler)
{
	// SIGINT and SIGTERM are considered normal behavior of the program,
	// not exceptions
	signal(SIGABRT, handler);
	signal(SIGFPE, handler);
	signal(SIGILL, handler);
	// signal(SIGINT, handler);
	signal(SIGSEGV, handler);
	// signal(SIGTERM, handler);
}

// lib activation, first thing to do in main
// programName should be set to argv[0]
inline void init_exceptions(char* programName)
{
	set_signal_handler(posix_signal_handler);
	Exceptions::getProgramName() = programName;
}

/* Resolve symbol name and source location given the path to the executable
   and an address */
// returns 0 if address has been resolved and a message has been printed; else
// returns 1
inline int addr2line(char const* const program_name, void const* const addr,
                     int lineNb)
{
	char addr2line_cmd[512] = {0};

/* have addr2line map the address to the relent line in the code */
#ifdef __APPLE__
	/* apple does things differently... */
	sprintf(addr2line_cmd, "atos -o %.256s %p", program_name, addr);
#else
	sprintf(addr2line_cmd, "addr2line -C -f -e %.256s %p", program_name, addr);
#endif

	/* This will print a nicely formatted string specifying the
	function and source line of the address */

	FILE* fp;
	char outLine1[1035];
	char outLine2[1035];

	/* Open the command for reading. */
	fp = popen(addr2line_cmd, "r");

	if(fp == NULL)
		return 1;

	while(fgets(outLine1, sizeof(outLine1) - 1, fp) != NULL)
	{
		// if we have a pair of lines
		if(fgets(outLine2, sizeof(outLine2) - 1, fp) != NULL)
		{
			// if symbols are readable
			if(outLine2[0] != '?')
			{
				// only let func name in outLine1
				int i;

				for(i = 0; i < 1035; ++i)
				{
					if(outLine1[i] == '\r' || outLine1[i] == '\n')
					{
						outLine1[i] = '\0';
						break;
					}
				}

				// don't display the whole path
				int lastSlashPos = 0;

				for(i = 0; i < 1035 && outLine2[i] != '\0'; ++i)
				{
					if(outLine2[i] == '/')
						lastSlashPos = i + 1;
				}

				std::cerr << "[" << lineNb << "] " << addr << " in " << outLine1
				          << " at " << outLine2 + lastSlashPos << std::flush;
			}
			else
			{
				pclose(fp);
				return 1;
			}
		}
		else
		{
			pclose(fp);
			return 1;
		}
	}

	/* close */
	pclose(fp);

	return 0;
}

#endif // EXCEPTIONS_UNIX_HPP
