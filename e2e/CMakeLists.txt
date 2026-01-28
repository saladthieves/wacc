set(TARGET e2e-testing)

enable_testing()

add_executable(${TARGET})

target_sources(${TARGET}
    PRIVATE
        ch01.cpp
        ch02.cpp
        ch03.cpp
        ch04.cpp
        e2e_base.cpp
)

target_link_libraries(${TARGET}
    PRIVATE
        driver
        util
        test-utils
        GTest::gtest_main
)