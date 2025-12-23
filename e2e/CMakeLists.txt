set(TARGET e2e-testing)

enable_testing()

add_executable(${TARGET})

target_sources(${TARGET}
    PRIVATE
        ch01.cpp
)

target_link_libraries(${TARGET}
    PRIVATE
        driver
        util
        test-utils
        GTest::gtest_main
)