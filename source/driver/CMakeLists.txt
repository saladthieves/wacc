set(TARGET driver)

add_library(${TARGET})

target_sources(${TARGET}
    PRIVATE
        args.cpp
        assembler.cpp
        compiler.cpp
        driver.cpp
        preprocessor.cpp
    
    PUBLIC
        FILE_SET HEADERS
)

target_link_libraries(${TARGET}
    PRIVATE
        frontend
        backend
        
    PUBLIC
        util
)