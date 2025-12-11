set(TARGET backend)

add_library(${TARGET})

target_sources(${TARGET}
    PRIVATE
        asm_ast.cpp
        asm_gen.cpp
    
    PUBLIC
        FILE_SET HEADERS
)

target_link_libraries(${TARGET}
    PRIVATE
        util
        frontend
)