set(TARGET tacky)

add_library(${TARGET})

target_sources(${TARGET}
    PRIVATE
        tacky_ast.cpp
        tacky_gen.cpp

    PUBLIC
        FILE_SET HEADERS
)

target_link_libraries(${TARGET}
    PRIVATE
        util
        frontend
)   