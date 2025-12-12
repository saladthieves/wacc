set(TARGET util)

add_library(${TARGET})

target_sources(${TARGET}
    PRIVATE
        cmd.cpp
        utils.cpp

    PUBLIC
        FILE_SET HEADERS
)

if (${CMAKE_HOST_APPLE})
    target_compile_definitions(${TARGET} PRIVATE WACC_HOST_MACOS)
endif()

if (${CMAKE_HOST_LINUX})
    target_compile_definitions(${TARGET} PRIVATE WACC_HOST_LINUX)
endif()