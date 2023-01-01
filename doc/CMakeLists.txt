
find_package(Doxygen REQUIRED)

set(DOXYGEN_INPUT_DIR 	${CMAKE_CURRENT_SOURCE_DIR})
set(DOXYGEN_OUTPUT_DIR 	${CMAKE_BINARY_DIR}/doc)

set(DOXYGEN_CONFIGURED_INPUT ${DOXYGEN_OUTPUT_DIR}/Doxyfile)

configure_file(${DOXYGEN_INPUT_DIR}/Doxyfile.in ${DOXYGEN_CONFIGURED_INPUT} @ONLY)

add_custom_target(doc ALL
	COMMAND ${CMAKE_COMMAND} -E echo_append "   |   Building AML doc...   |   " 
	COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_CONFIGURED_INPUT}
	COMMAND ${CMAKE_COMMAND} -E echo_append "   |   Ending building AML doc...   |   "
	WORKING_DIRECTORY ${DOXYGEN_INPUT_DIR}
	VERBATIM)
		
