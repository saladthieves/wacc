set(TARGET driver-tests)

add_library(${TARGET})

target_sources(${TARGET}
    PUBLIC
        args_test.cpp
        compiler_test.cpp
        preprocessor_test.cpp
)

target_link_libraries(${TARGET}
    PUBLIC
        driver
        util
        test-utils
        GTest::gtest_main
)