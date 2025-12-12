set(TARGET testing)

enable_testing()

add_executable(${TARGET})

target_sources(${TARGET}
    PRIVATE
        backend/asm_gen_test.cpp
        frontend/lexer_test.cpp
        frontend/parser_test.cpp
        driver/args_test.cpp
        driver/compiler_test.cpp
        driver/preprocessor_test.cpp
        util/cmd_test.cpp
        util/utils_test.cpp
)

target_link_libraries(${TARGET}
    PRIVATE
        driver
        util
        test-utils
        frontend
        backend
        GTest::gtest_main
)

add_subdirectory(test_utils)

include(GoogleTest)
gtest_discover_tests(${TARGET})