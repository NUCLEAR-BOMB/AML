
function(aml_add_native_visualizer)
	if (MSVC_IDE)
		target_sources(${PROJECT_NAME} INTERFACE $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/AML.natvis>)
	endif()
endfunction()
