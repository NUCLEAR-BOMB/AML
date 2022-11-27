cmake_minimum_required(VERSION 3.24 FATAL_ERROR)

project("AML-unittest" CXX)

add_subdirectory(googletest)

add_executable("${PROJECT_NAME}" Vector.cpp)

target_link_libraries("${PROJECT_NAME}" gtest_main gmock_main 
	# AML
)

target_include_directories("${PROJECT_NAME}" PUBLIC 
	"${PROJECT_SOURCE_DIR}/googletest/googletest/include"
	"${PROJECT_SOURCE_DIR}/googletest/googlemock/include"
	"${PROJECT_SOURCE_DIR}/../include"
)