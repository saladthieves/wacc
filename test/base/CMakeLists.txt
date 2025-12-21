set(TARGET test-base)

add_library(${TARGET})

target_sources(${TARGET}
    PRIVATE
        base_test.cpp
        matchers.cpp

    PUBLIC
        FILE_SET HEADERS
)

target_link_libraries(${TARGET}
    PUBLIC
        frontend
        tacky
        backend
        util
        test-utils
        GTest::gtest_main
)