
find_package(Doxygen REQUIRED)

if (DOXYGEN_VERSION VERSION_LESS "1.9.5")
	message(FATAL_ERROR "Requires a doxygen version greater than or equal to 1.9.5")
endif()

set(DOXYGEN_INPUT_DIR 	${CMAKE_SOURCE_DIR}/doc)
set(DOXYGEN_OUTPUT_DIR 	${CMAKE_BINARY_DIR}/doc)

set(DOXYGEN_CONFIGURED_INPUT ${DOXYGEN_OUTPUT_DIR}/Doxyfile)

configure_file(${DOXYGEN_INPUT_DIR}/Doxyfile.in ${DOXYGEN_CONFIGURED_INPUT} @ONLY)

add_custom_target(doc ALL
	COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_CONFIGURED_INPUT}
	WORKING_DIRECTORY ${DOXYGEN_INPUT_DIR}
)
