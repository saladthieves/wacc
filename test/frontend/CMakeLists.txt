set(TARGET frontend-tests)

add_library(${TARGET})

target_sources(${TARGET}
    PUBLIC
        lexer_test.cpp
        parser_test.cpp
        source_test.cpp
)

target_link_libraries(${TARGET}
    PUBLIC
        frontend
        test-utils
        GTest::gtest_main
)