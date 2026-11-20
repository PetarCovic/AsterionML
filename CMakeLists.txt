cmake_minimum_required (VERSION 3.24)

project(AsterionML VERSION 0.1.0 LANGUAGES CXX)

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
	message(FATAL_ERROR "In-source builds are not allowed. Please create a separate build directory.")
endif()

set(CMAKE_CXX_EXTENSIONS OFF)

option(ASTERIONML_BUILD_EXAMPLES "Build AsterionML examples" ON)

include(CTest)

function(asterionml_enable_warnings target)
	if(MSVC)
		target_compile_options(${target} PRIVATE
			/W4
			/permissive-
			/Zc:__cplusplus
		)
	elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
		target_compile_options(${target} PRIVATE
			-Wall
			-Wextra
			-Wpedantic
		)
	endif()
endfunction()

add_subdirectory(src)

if(ASTERIONML_BUILD_EXAMPLES)
	add_subdirectory(examples)
endif()

if(BUILD_TESTING)
	add_subdirectory(tests)
endif()