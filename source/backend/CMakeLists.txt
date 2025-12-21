set(TARGET backend)

add_library(${TARGET})

target_sources(${TARGET}
    PRIVATE
        asm_ast.cpp
        asm_emit.cpp
        asm_gen.cpp
        asm_instr_fix_pass.cpp
        asm_pseudo_pass.cpp
        asm_writer.cpp
    
    PUBLIC
        FILE_SET HEADERS
)

target_link_libraries(${TARGET}
    PRIVATE
        util
        tacky
)