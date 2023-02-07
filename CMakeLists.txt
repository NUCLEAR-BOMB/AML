cmake_minimum_required(VERSION 3.24 FATAL_ERROR)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
include(aml_functions)

project("AML" CXX)

aml_set_option(AML_DOC 			NO 		BOOL 	"Build AML doxygen doc")
aml_set_option(AML_TESTS		YES		BOOL 	"Build AML unittests")
aml_set_option(AML_DOC_LATEX	NO		BOOL 	"Generate doxygen LaTeX")
aml_set_option(AML_DOC_HTML		YES		BOOL 	"Generate doxygen HTML")
aml_set_option(AML_BENCHMARK	NO		BOOL	"Build AML benchmark")

aml_set_option(AML_STD			"17"	STRING	"AML lib C++ standard")

aml_set_option(AML_INCLUDE_PATH
	"${CMAKE_SOURCE_DIR}/include" PATH "AML lib include path")

set(CMAKE_INSTALL_MESSAGE LAZY)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

#file(GLOB_RECURSE SOURCE_FILES CONFIGURE_DEPENDS "${AML_INCLUDE_PATH}/source/*.cpp")
file(GLOB_RECURSE HEADER_FILES CONFIGURE_DEPENDS "${AML_INCLUDE_PATH}/AML/*.hpp")

add_library				  (${PROJECT_NAME} INTERFACE ${HEADER_FILES})

target_compile_features	  (${PROJECT_NAME} INTERFACE "cxx_std_${AML_STD}")
target_include_directories(${PROJECT_NAME} INTERFACE ${AML_INCLUDE_PATH})

target_compile_options	  (${PROJECT_NAME} INTERFACE
	$<IF:$<CXX_COMPILER_ID:MSVC>,
		/W4 /WX
	, # ^^^ MSVC | not MSVC vvv
		-Wall -Wextra -pedantic -Wconversion -Werror
	>
)

#set_target_properties("${PROJECT_NAME}" PROPERTIES LINKER_LANGUAGE CXX)

aml_add_native_visualizer(${PROJECT_NAME})

source_group(TREE ${AML_INCLUDE_PATH} FILES ${HEADER_FILES})


if (AML_DOC)
	add_subdirectory(doc)
endif()

if (AML_TESTS)
	enable_testing()
	#add_subdirectory(unittests)
	add_subdirectory(test)
endif()

if (AML_BENCHMARK)
	add_subdirectory(benchmark)
endif()
