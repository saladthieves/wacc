set(TARGET test-utils)

add_library(${TARGET} INTERFACE)

target_sources(${TARGET} 
    INTERFACE
        FILE_SET HEADERS
)