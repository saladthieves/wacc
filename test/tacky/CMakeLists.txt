set(TARGET tacky-tests)

add_library(${TARGET})

target_sources(${TARGET}
    PUBLIC
        tacky_gen_test.cpp
)

target_link_libraries(${TARGET}
    PUBLIC
        tacky
        frontend
        test-utils
        util
        GTest::gtest_main
)