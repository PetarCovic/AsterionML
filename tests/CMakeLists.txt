include(FetchContent)

set(BUILD_GMOCK OFF CACHE BOOL "Build GoogleMock" FORCE)
set(INSTALL_GTEST OFF CACHE BOOL "Install GoogleTest" FORCE)

if(MSVC)
    set(gtest_force_shared_crt ON CACHE BOOL "Use the shared MSVC runtime" FORCE)
endif()

FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.17.0.tar.gz
    URL_HASH SHA256=65fab701d9829d38cb77c14acdc431d2108bfdbf8979e40eb8ae567edf10b27c
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)

FetchContent_MakeAvailable(googletest)

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
    COMMAND asterionml_build_smoke
)

add_executable(asterionml_data_type_tests
    data/DataTypeTests.cpp
)

target_link_libraries(asterionml_data_type_tests
    PRIVATE
        ml::core
        GTest::gtest_main
)

asterionml_enable_warnings(asterionml_data_type_tests)

include(GoogleTest)

gtest_discover_tests(asterionml_data_type_tests
    TEST_PREFIX "data."
    DISCOVERY_TIMEOUT 30
    PROPERTIES TIMEOUT 30
)