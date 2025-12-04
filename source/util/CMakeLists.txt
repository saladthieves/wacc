set(TARGET util)

add_library(${TARGET})

target_sources(${TARGET}
    PRIVATE
        cmd.cpp
        utils.cpp

    PUBLIC
        FILE_SET HEADERS
)