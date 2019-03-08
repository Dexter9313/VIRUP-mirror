find_package(Doxygen)

if(DOXYGEN_FOUND)
	message("doxygen executable: ${DOXYGEN_EXECUTABLE}")
	message("doxygen version: ${DOXYGEN_VERSION}")


	# Get all project files
	file(GLOB_RECURSE ALL_SOURCE_FILES ${DOCUMENTED_FILES})

	list(REMOVE_DUPLICATES ALL_SOURCE_FILES)

	set(doxyfile_in ${CMAKE_CURRENT_SOURCE_DIR}/doc/Doxyfile.in)
	set(doxyfile ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

	configure_file(${doxyfile_in} ${doxyfile} @ONLY)

	add_custom_target(doc
		COMMAND ${PROJECT_SOURCE_DIR}/ci/doc-report.sh ${DOXYGEN_EXECUTABLE} ${doxyfile}
		WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
		COMMENT "Generating API documentation with Doxygen"
		VERBATIM)
else()
	message("doxygen executable not found")
endif()
