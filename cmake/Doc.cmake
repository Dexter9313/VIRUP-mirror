find_package(Doxygen)

if(DOXYGEN_FOUND)
	message("doxygen executable: ${DOXYGEN_EXECUTABLE}")
	message("doxygen version: ${DOXYGEN_VERSION}")

	if(PROJECT_DOC)
		set(DOC_INPUT "${DOC_INPUT} ${PROJECT_SOURCE_DIR}/${PROJECT_DIRECTORY}/include/ ${PROJECT_SOURCE_DIR}/${PROJECT_DIRECTORY}/src/")
	endif()
	if(PROJECT_DOC_ENGINE)
		set(DOC_INPUT "${DOC_INPUT} ${PROJECT_SOURCE_DIR}/include/ ${PROJECT_SOURCE_DIR}/src/")
	endif()
	if(PROJECT_DOC_THIRDPARTY)
		set(DOC_INPUT "${DOC_INPUT} ${LIBRARIES_DIRS}")
	endif()

	message("Generate documentation for: ${DOC_INPUT}")

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
