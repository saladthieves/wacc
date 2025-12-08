set(TARGET core)

add_library(${TARGET})

target_sources(${TARGET}
    PRIVATE
        assembly.cpp
        ast.cpp
        generator.cpp
        lexer.cpp
        parser.cpp
        token.cpp
    
    PUBLIC
        FILE_SET HEADERS
)

target_link_libraries(${TARGET}
    PRIVATE
        util
)