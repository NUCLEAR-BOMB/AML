cmake_minimum_required(VERSION 3.24 FATAL_ERROR)

project("AML-unittests" CXX)

if (MSVC)

else()
	add_compile_options(-Wno-implicit-int-float-conversion)
endif()

add_subdirectory(googletest)

file(GLOB_RECURSE ALL_FILES CONFIGURE_DEPENDS 
	"${PROJECT_SOURCE_DIR}/*.cpp"
	"${PROJECT_SOURCE_DIR}/*.hpp"
)

add_executable("${PROJECT_NAME}" Vector.cpp ${ALL_FILES})

target_link_libraries("${PROJECT_NAME}" gtest_main gmock_main 
	# AML
)

target_include_directories("${PROJECT_NAME}" PUBLIC 
	"${PROJECT_SOURCE_DIR}/googletest/googletest/include"
	"${PROJECT_SOURCE_DIR}/googletest/googlemock/include"
	"${PROJECT_SOURCE_DIR}/../include"
)