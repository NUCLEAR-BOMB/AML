cmake_minimum_required(VERSION 3.24 FATAL_ERROR)

project("AML" CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED true)

file(GLOB_RECURSE SOURCE_FILES CONFIGURE_DEPENDS "${PROJECT_SOURCE_DIR}/source/*.cpp")
file(GLOB_RECURSE HEADER_FILES CONFIGURE_DEPENDS "${PROJECT_SOURCE_DIR}/include/AML/*.hpp")

if (MSVC)
	add_compile_options(/W4 /WX)
else()
	add_compile_options(-Wall -Wextra -pedantic -Werror)
endif()

add_library("${PROJECT_NAME}" STATIC ${SOURCE_FILES} ${HEADER_FILES})

target_include_directories("${PROJECT_NAME}" PUBLIC "${PROJECT_SOURCE_DIR}/include")

set_target_properties("${PROJECT_NAME}" PROPERTIES LINKER_LANGUAGE CXX)

add_subdirectory(unittests)
