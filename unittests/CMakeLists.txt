cmake_minimum_required(VERSION 3.24 FATAL_ERROR)

project("AML-unittests" CXX)

if (MSVC)

else()
	add_compile_options(-Wno-implicit-int-float-conversion)
endif()

include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG 		 58d77fa8070e8cec2dc1ed015d66b454c8d78850 # release-1.12.1
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

file(GLOB_RECURSE ALL_FILES CONFIGURE_DEPENDS 
	"${PROJECT_SOURCE_DIR}/*.cpp"
	"${PROJECT_SOURCE_DIR}/*.hpp"
)

add_executable("${PROJECT_NAME}" ${ALL_FILES})

target_link_libraries("${PROJECT_NAME}" GTest::gtest_main AML)

set_target_properties(gtest gtest_main gmock gmock_main PROPERTIES FOLDER "GoogleTest")

include(GoogleTest)
gtest_discover_tests("${PROJECT_NAME}")

set_property(DIRECTORY "${CMAKE_SOURCE_DIR}" PROPERTY VS_STARTUP_PROJECT "${PROJECT_NAME}")
