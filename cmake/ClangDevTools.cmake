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

find_package(ClangFormat)

if(CLANG_FORMAT_FOUND)
	if(PROJECT_CHECK_FORMAT)
		file(GLOB_RECURSE ALL_FILES ${PROJECT_HPP_FILES} ${TEST_HPP_FILES} ${PROJECT_SRC_FILES} ${MAIN_FILE} ${TEST_SRC_FILES})
	else()
		file(GLOB_RECURSE ALL_FILES ${ENGINE_HPP_FILES} ${ENGINE_TEST_HPP_FILES} ${ENGINE_SRC_FILES} ${MAIN_FILE} ${ENGINE_TEST_SRC_FILES})
	endif()
	list(REMOVE_DUPLICATES ALL_FILES)
	message(STATUS "Format : ${ALL_FILES}")

	#Only get first executable
	string(REGEX REPLACE ";.*$" "" CLANG_FORMAT "${CLANG_FORMAT_EXECUTABLE}")
	message("clang-format executable: ${CLANG_FORMAT}")
	message("clang-format version: ${CLANG_FORMAT_VERSION}")
	add_custom_target(
		clang-format
		COMMAND ${PROJECT_SOURCE_DIR}/ci/clang-format-report.sh
		${CLANG_FORMAT}
		${ALL_FILES}
	)
else()
	message("clang-format executable not found")
endif()

find_package(ClangTidy)

if(CLANG_TIDY_FOUND)
	if(PROJECT_CHECK_TIDY)
		file(GLOB_RECURSE ALL_SOURCE_FILES ${PROJECT_SRC_FILES} ${MAIN_FILE} ${TEST_SRC_FILES})
	else()
		file(GLOB_RECURSE ALL_SOURCE_FILES ${ENGINE_SRC_FILES} ${ENGINE_TEST_SRC_FILES})
	endif()
	list(REMOVE_DUPLICATES ALL_SOURCE_FILES)
	message(STATUS "Tidy : ${ALL_SOURCE_FILES}")

	#Only get first executable
	string(REGEX REPLACE ";.*$" "" CLANG_TIDY "${CLANG_TIDY_EXECUTABLE}")
	message("clang-tidy executable: ${CLANG_TIDY}")
	message("clang-tidy version: ${CLANG_TIDY_VERSION}")
	add_custom_target(
		clang-tidy
		COMMAND ${PROJECT_SOURCE_DIR}/ci/clang-tidy-report.sh
		${CLANG_TIDY}
		-p .
		${ALL_SOURCE_FILES}
		-system-headers=0
		-extra-arg=-Wno-unknown-warning-option
	)
else()
	message("clang-tidy executable not found")
endif()
