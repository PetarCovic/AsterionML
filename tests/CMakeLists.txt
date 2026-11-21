add_executable(asterionml_build_smoke
	build_smoke.cpp
)

target_link_libraries(asterionml_build_smoke
	PRIVATE
		ml::core
)

target_compile_definitions(asterionml_build_smoke
	PRIVATE
		ASTERIONML_EXPECTED_VERSION="${PROJECT_VERSION}"
)

asterionml_enable_warnings(asterionml_build_smoke)

add_test(
	NAME build.smoke
	COMMAND asterionml_build_smoke
)


add_executable(asterionml_data_type_tests
	data/DataTypeTests.cpp
)

target_link_libraries(asterionml_data_type_tests
	PRIVATE
		ml::core
)

asterionml_enable_warnings(asterionml_data_type_tests)

add_test(
	NAME data.type.construction
	COMMAND asterionml_data_type_tests
)