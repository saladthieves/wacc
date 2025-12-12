set(TARGET test-utils)

add_library(${TARGET})

target_sources(${TARGET}
    PRIVATE
        test_utils.cpp

    PUBLIC
        FILE_SET HEADERS
)