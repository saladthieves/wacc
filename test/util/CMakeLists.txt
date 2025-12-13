set(TARGET util-tests)

add_library(${TARGET})

target_sources(${TARGET}
    PUBLIC
        cmd_test.cpp
        utils_test.cpp
)

target_link_libraries(${TARGET}
    PUBLIC
        util
        GTest::gtest_main
)