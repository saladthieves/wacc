set(TARGET compiler)

add_executable(${TARGET})

target_sources(${TARGET}
    PRIVATE
        main.cpp
)