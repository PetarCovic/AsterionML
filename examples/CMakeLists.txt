add_executable(asterionml_build_check
	build_check.cpp
)

target_link_libraries(asterionml_build_check PRIVATE ml::ml)

asterionml_enable_warnings(asterionml_build_check)