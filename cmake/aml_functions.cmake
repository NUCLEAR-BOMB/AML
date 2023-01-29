
macro(aml_set_option varname default type docstr)
	if (NOT DEFINED ${varname})
		set(${varname} ${default} CACHE ${type} ${docstr})
	else()
		set(${varname} ${${varname}} CACHE ${type} ${docstr})
	endif()
endmacro()

function(aml_add_native_visualizer proj_name)
	if (MSVC_IDE)
		target_sources(${proj_name} INTERFACE $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/AML.natvis>)
	endif()
endfunction()

macro(aml_find_gtest)
	find_package(GTest QUIET)
	if (NOT GTest_FOUND)
		include(FetchContent)
		FetchContent_Declare(
		  googletest
		  GIT_REPOSITORY https://github.com/google/googletest.git
		  GIT_TAG 		 58d77fa8070e8cec2dc1ed015d66b454c8d78850 # release-1.12.1
		)
		
		set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
		FetchContent_MakeAvailable(googletest)
		
		include(GoogleTest)
	endif()
endmacro()

macro(aml_find_googlebenchmark)
	set(BENCHMARK_ENABLE_TESTING NO)

	find_package(benchmark QUIET)
	if (NOT benchmark_FOUND)
		include(FetchContent)
		
		FetchContent_Declare(
			googlebenchmark
			GIT_REPOSITORY https://github.com/google/benchmark.git
			GIT_TAG        d572f4777349d43653b21d6c2fc63020ab326db2 # v1.7.1
		)

		FetchContent_MakeAvailable(googlebenchmark)
	endif()
endmacro()

function(aml_inherit_compile_options target scope targetfrom)
	get_target_property(options ${targetfrom} INTERFACE_COMPILE_OPTIONS)
	if (options STREQUAL "options-NOTFOUND")
		get_target_property(options ${targetfrom} COMPILE_OPTIONS)
	endif()
	
	target_compile_options(${target} ${scope} ${options})
endfunction()

