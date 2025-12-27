set(TARGET test-base)

add_library(${TARGET})

target_sources(${TARGET}
    PRIVATE
        base_test.cpp
        asm_formatters.cpp
        ast_formatters.cpp
        tacky_formatters.cpp
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