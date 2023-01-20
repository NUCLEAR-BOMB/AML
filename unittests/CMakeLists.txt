cmake_minimum_required(VERSION 3.24 FATAL_ERROR)

project("unittests" CXX)

aml_find_gtest()

file(GLOB_RECURSE ALL_FILES CONFIGURE_DEPENDS 
	"${PROJECT_SOURCE_DIR}/*.cpp"
	"${PROJECT_SOURCE_DIR}/*.hpp"
)

add_executable(${PROJECT_NAME} ${ALL_FILES})

aml_inherit_compile_options(${PROJECT_NAME} PRIVATE "AML")

target_compile_options(${PROJECT_NAME} PRIVATE
	$<$<NOT:$<CXX_COMPILER_ID:MSVC>>:
		-Wno-unused-variable -Wno-implicit-int-float-conversion
	>
)

target_link_libraries(${PROJECT_NAME} PRIVATE GTest::gtest AML)

set_target_properties(gtest gtest_main gmock gmock_main PROPERTIES FOLDER "GoogleTest")

gtest_discover_tests(${PROJECT_NAME})

set_property(DIRECTORY "${CMAKE_SOURCE_DIR}" PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})
