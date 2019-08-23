/*
    Copyright (C) 2019 Florian Cabot <florian.cabot@epfl.ch>

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

/*! \defgroup pycall Python Callable Instance
 * This group contains every class that has a callable instance in python.
 *
 * See each class description to know the identifier of their corresponding
 * instance.
 *
 * Due to PythonQt limitations, not everything can be accessed in every class :
 * only properties and slots can be.
 *
 * All properties/slots identifiers are common between C++ and Python, so this
 * documentation is relevant for both languages.
 *
 * Properties can be accessed as their accessors permit it (some of them only
 * have a getter and no setter, so they are read-only). See each property's
 * documentation to know.
 *
 * Slots may require parameters or return types from classes that are not
 * wrapped yet. In those cases, they are unusable in python yet.
 *
 * Qt classes are wrapped thanks to PythonQt, so you should always be able to
 * provide Qt-related parameters or get Qt-related return values. Make sure you
 * import the relevant classes to be able to use them.
 */

/*! \defgroup pywrap Python Wrapped Classes
 * This group contains every wrapped class from the integrated python
 * interpreter.
 *
 * A class is considered wrapped if it can be instanciated and called in python
 * almost like it would be in C++. Most constructors and methods should be
 * callable (check each member documentation for details).
 *
 * Operator overloading (both internal and external) is also wrapped in most
 * cases (bear in mind operator<< will often be wrapped as __str__ though).
 *
 * Most related external functions should be wrapped as well.
 *
 * Members or functions may require parameters or return types from classes that
 * are not wrapped yet. In those cases, they are unusable in python yet.
 *
 * Qt classes are wrapped thanks to PythonQt, so you should always be able to
 * provide Qt-related parameters or get Qt-related return values. Make sure you
 * import the relevant classes to be able to use them.
 */
