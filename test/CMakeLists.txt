cmake_minimum_required(VERSION 3.24 FATAL_ERROR)

aml_find_gtest()

file(GLOB_RECURSE ALL_FILES CONFIGURE_DEPENDS 
	"${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/*.hpp"
)

add_executable(aml-test ${ALL_FILES})
#add_library(aml-test OBJECT ${ALL_FILES})

aml_inherit_compile_options(aml-test PRIVATE "AML")

target_compile_options(aml-test PRIVATE
	$<$<NOT:$<CXX_COMPILER_ID:MSVC>>:
		-Wno-unused-variable -Wno-implicit-int-float-conversion
	>
)

target_link_libraries(aml-test PRIVATE GTest::gtest AML)

set_target_properties(gtest gtest_main gmock gmock_main PROPERTIES FOLDER "GoogleTest")

#set_property(DIRECTORY "${CMAKE_SOURCE_DIR}" PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})
