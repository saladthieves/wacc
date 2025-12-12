set(TARGET frontend)

add_library(${TARGET})

target_sources(${TARGET}
    PRIVATE
        ast.cpp
        lexer.cpp
        parser.cpp
        source.cpp
        token.cpp
    
    PUBLIC
        FILE_SET HEADERS
)

target_link_libraries(${TARGET}
    PRIVATE
        util
)