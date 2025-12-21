set(TARGET backend-tests)

add_library(${TARGET})

target_sources(${TARGET}
    PUBLIC
        asm_emit_test.cpp
        asm_gen_test.cpp
        asm_instr_fix_pass_test.cpp
        asm_pseudo_pass_test.cpp
        asm_writer_test.cpp
)

target_link_libraries(${TARGET}
    PUBLIC
        frontend
        backend
        tacky
        test-utils
        util
        GTest::gtest_main
)