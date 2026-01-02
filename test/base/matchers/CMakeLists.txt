set(TARGET test-matchers)

add_library(${TARGET})

target_sources(${TARGET}
    PRIVATE
        asm_matchers.cpp
        ast_matchers.cpp
        tacky_matchers.cpp

    PUBLIC
        FILE_SET HEADERS
)

target_link_libraries(${TARGET}
    PUBLIC
        frontend
        tacky
        backend
        test-utils
        GTest::gtest_main
)