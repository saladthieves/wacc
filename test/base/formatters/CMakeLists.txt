set(TARGET test-formatters)

add_library(${TARGET})

target_sources(${TARGET}
    PRIVATE
        asm_formatters.cpp
        ast_formatters.cpp
        tacky_formatters.cpp

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