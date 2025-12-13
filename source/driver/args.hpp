#pragma once

#include <string>
#include <vector>

namespace wacc {
namespace driver {
namespace {
using namespace std::string_literals;
/*
TODO: Implement flags functionality
TODO: Write tests for flags functionality
    --lex       - Run lexer only.
    --parse     - Run lexer + parser only.
    --codegen   - Run lexer + parser + generation only.

    None of the above flags should produce output files.

    -S  - Run emission + writer only (don't assemble or link).
*/
constexpr auto FLAG_LEX = "--lex"s;
constexpr auto FLAG_PARSE = "--parse"s;
constexpr auto FLAG_CODEGEN = "--codegen"s;
constexpr auto FLAG_CLEANUP = "--cleanup"s;
constexpr auto COMPILER_CLANG = "clang"s;
constexpr auto COMPILER_GCC = "gcc"s;
} // namespace

class DriverArgs {
public:
    bool lex{false};
    bool parse{false};
    bool codegen{false};
    bool cleanUp{true};
    std::string path{""};
    std::string compiler{COMPILER_CLANG};
};

// TODO: Update tests with --cleanup flag
DriverArgs parseDriverArgs(std::vector<std::string>& args);
} // namespace driver
} // namespace wacc