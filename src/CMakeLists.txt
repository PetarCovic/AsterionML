add_library(asterionml_core STATIC
	core/version.cpp
)

add_library(ml::core ALIAS asterionml_core)

target_compile_features(asterionml_core PUBLIC cxx_std_20)

target_include_directories(asterionml_core
	PUBLIC
		"$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>"
)

target_compile_definitions(asterionml_core
	PRIVATE
		ASTERIONML_VERSION="${PROJECT_VERSION}"
)

set_target_properties(asterionml_core PROPERTIES
	POSITION_INDEPENDENT_CODE ON
)

asterionml_enable_warnings(asterionml_core)

add_library(asterionml INTERFACE)

add_library(ml::ml ALIAS asterionml)

target_link_libraries(asterionml INTERFACE ml::core)