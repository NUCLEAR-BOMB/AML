cmake_minimum_required(VERSION 3.24 FATAL_ERROR)

project("AML" CXX)

macro(aml_set_option varname default type docstr)
	if (NOT DEFINED ${varname})
		set(${varname} ${default} CACHE ${type} ${docstr})
	else()
		set(${varname} ${${varname}} CACHE ${type} ${docstr})
	endif()
endmacro()


aml_set_option(AML_BUILD_DOC 		NO 	BOOL "Build AML doxygen doc")
aml_set_option(AML_BUILD_TESTS		YES	BOOL "Build AML unittests")
aml_set_option(AML_DOC_LATEX		NO	BOOL "Generate doxygen LaTeX")
aml_set_option(AML_DOC_HTML			YES	BOOL "Generate doxygen HTML")

set(AML_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/include")
set(CMAKE_INSTALL_MESSAGE LAZY)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

#file(GLOB_RECURSE SOURCE_FILES CONFIGURE_DEPENDS "${PROJECT_SOURCE_DIR}/source/*.cpp")
file(GLOB_RECURSE HEADER_FILES CONFIGURE_DEPENDS "${AML_INCLUDE_DIR}/AML/*.hpp")



if (MSVC)
	add_compile_options(/W4 /WX)
else()
	add_compile_options(-Wall -Wextra -pedantic -Werror)
endif()

add_library("${PROJECT_NAME}" INTERFACE ${HEADER_FILES})

target_compile_features("${PROJECT_NAME}" INTERFACE "cxx_std_17")

target_include_directories("${PROJECT_NAME}" INTERFACE ${AML_INCLUDE_DIR})

#set_target_properties("${PROJECT_NAME}" PROPERTIES LINKER_LANGUAGE CXX)


if (AML_BUILD_DOC)
	add_subdirectory(doc)
endif()

if (AML_BUILD_TESTS)
	enable_testing()
	add_subdirectory(unittests)
endif()
