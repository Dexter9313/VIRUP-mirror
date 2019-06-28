/*
    Copyright (C) 2018 Florian Cabot <florian.cabot@hotmail.fr>

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

#ifndef EXCEPTIONS_HPP
#define EXCEPTIONS_HPP

#include <boost/predef.h>
#include <sstream>
#include <string>

// Any hpp file included there must define these functions :
// void print_stacktrace(bool calledFromSigHand);
// void init_exceptions(char* programName);
#ifdef __unix
#include "Exceptions_unix.hpp"
#else
#include "Exceptions_default.hpp"
#endif

/*! \ingroup exceptions
 * Prints stack trace and throws a critical exception.
 *
 * This exception will be automatically catched between the #BEGIN_EXCEPTIONS
 * and
 * #END_EXCEPTIONS macros. These macros will ensure the displaying of a helpful
 * debug message and will exit the program with the error code EXIT_FAILURE.
 * The debug message contains three automatically-filled informations and a
 * custom string. These informations are : the function name of the function
 * being called, the file name which contains the function and the line number
 * within the file where #CRITICAL has been called.
 * \param str The custom string to be displayed for debugging.
 */
#define CRITICAL(str)                                                     \
	{                                                                     \
		print_stacktrace(false);                                          \
		throw(CriticalException(                                          \
		    str, __func__, /*NOLINT complaining about __func__ argument*/ \
		    __FILE__, __LINE__));                                         \
	}

/*! \ingroup exceptions
 * Initializes the critical exceptions handling.
 *
 * This exceptions system prints stack traces automatically whenever a critical
 * signal is sent, whether it is from the system (SIG_SEGV for example) or by
 * the program itself through the #CRITICAL macro.
 *
 * Because of the way it handles system signals, it is also currently not
 * possible to use the signal() function to redefine some system signal handlers
 * (details about which one are platform specific, please read the
 * implementation). If
 * you wish to do specific things on system signal capture, please edit the
 * signal handler defined in the Exceptions.hpp file corresponding to the
 * platform. It is
 * expected that some additional functions or macros will be written to allow
 * system signals handling anywhere else in the code.
 *
 * This macro has to be put within the main function together with the
 * #END_EXCEPTIONS macro and has to be called only once in the whole program. It
 * will assume the argv parameter is passed to main and use it automatically.
 * To be more precise, it will start a try{} block which has to be closed with
 * the #END_EXCEPTIONS macro, which holds the corresponding closing bracket and
 * the catch{} block.
 */
#define BEGIN_EXCEPTIONS                                           \
	init_exceptions(argv[0]); /*NOLINT complaining about argv[0]*/ \
	try /*NOLINT clang-tidy thinks this is never executed */       \
	{
/*! \ingroup exceptions
 * Closes the critical exceptions handling.
 *
 * This exceptions system prints automatically stack traces whenever a critical
 * signal is sent, whether it is from the system (SIG_SEGV for example) or by
 * the program itself through the #CRITICAL macro.
 *
 * Because of the way it handles system signals, it is also currently not
 * possible to use the signal() function to redefine some system signal handlers
 * (details about which one are platform specific, please read the
 * implementation). If
 * you wish to do specific things on system signal capture, please edit the
 * signal handler defined in the Exceptions.hpp file corresponding to the
 * platform to do it there. It is
 * expected that some additional functions or macros will be written to allow
 * system signals handling anywhere else in the code.
 *
 * This macro has to be put within the main function together with the
 * #BEGIN_EXCEPTIONS macro and has to be called only once in the whole program.
 * To
 * be more precise, it will close the try{} block opened by the
 * #BEGIN_EXCEPTIONS
 * macro.
 */
#define END_EXCEPTIONS                       \
	}                                        \
	catch(CriticalException & exception)     \
	{                                        \
		std::cerr << exception << std::endl; \
		exit(EXIT_FAILURE);                  \
	}

/*! Exception to be thrown by the #CRITICAL macro
 *
 * It is not intended to be thrown by the user, even if he could in theory. The
 * prefered way to use it
 * is through the #CRITICAL macro.
 * It contains an error message, the name of the function that threw it, the
 * name of the file that contains
 * the throw statement and the line at which this throw statement can be found.
 */
class CriticalException
{
  private:
	std::string message;
	std::string funcName;
	std::string file;
	int line;

	static std::string itos(int i)
	{
		std::ostringstream oss;
		oss << i;
		return oss.str();
	}

  public:
	/*! Constructor
	 *
	 * \param message Custom message that should be displayed.
	 * \param funcName Name of the function it was called inside
	 * \param file Name of the file it was called inside
	 * \param line Line in the file which it was called at
	 */
	CriticalException(std::string message, std::string funcName,
	                  std::string file, int line)
	    : message(message)
	    , funcName(funcName)
	    , file(file)
	    , line(line)
	{
	}

	/*! Transforms the Exception into a nice character string
	 */
	std::string toStr() const

	{
		return message + " (in " + funcName + " at " + file + ":" + itos(line)
		       + ")";
	}
};

/*! Puts the exception's string form in the stream
 *
 * \param stream stream to use
 * \param exception exception to print in stream
 */
inline std::ostream& operator<<(std::ostream& stream,
                                CriticalException const& exception)
{
	return stream << exception.toStr();
}

#endif // EXCEPTIONS_HPP
