set(TARGET testing)

enable_testing()

add_executable(${TARGET})

target_sources(${TARGET}
    PRIVATE
        core/lexer_test.cpp
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
        GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(${TARGET})