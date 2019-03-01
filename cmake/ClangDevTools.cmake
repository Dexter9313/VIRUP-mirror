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

# Additional target to perform clang-format/clang-tidy run
# Requires clang-format and clang-tidy


# Get all project files

if(PROJECT_CHECK_FORMAT)
	file(GLOB_RECURSE ALL_SOURCE_FILES ${HPP_FILES} ${SRC_FILES} ${MAIN_FILE} ${TEST_HPP_FILES} ${TEST_SRC_FILES})
else()
	file(GLOB_RECURSE ALL_SOURCE_FILES ${ENGINE_HPP_FILES} ${ENGINE_SRC_FILES} ${MAIN_FILE} ${ENGINE_TEST_HPP_FILES} ${ENGINE_TEST_SRC_FILES})
endif()
list(REMOVE_DUPLICATES ALL_SOURCE_FILES)

find_package(ClangFormat)

if(CLANG_FORMAT_FOUND)
	#Only get first executable
	string(REGEX REPLACE ";.*$" "" CLANG_FORMAT "${CLANG_FORMAT_EXECUTABLE}")
	message("clang-format executable: ${CLANG_FORMAT}")
	message("clang-format version: ${CLANG_FORMAT_VERSION}")
	add_custom_target(
		clang-format
		COMMAND ${PROJECT_SOURCE_DIR}/ci/clang-format-report.sh
		${CLANG_FORMAT}
		${ALL_SOURCE_FILES}
	)
else()
	message("clang-format executable not found")
endif()
