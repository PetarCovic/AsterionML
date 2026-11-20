add_executable(asterionml_build_smoke
	build_smoke.cpp
)

target_link_libraries(asterionml_build_smoke PRIVATE ml::core)

target_compile_definitions(asterionml_build_smoke
	PRIVATE
		ASTERIONML_EXPECTED_VERSION="${PROJECT_VERSION}"
)

asterionml_enable_warnings(asterionml_build_smoke)

add_test(
	NAME build.smoke
	COMMAND asterionml_build_smoke)