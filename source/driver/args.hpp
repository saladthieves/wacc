#pragma once

#include <string>
#include <vector>

namespace wacc {
namespace driver {
namespace {
using namespace std::string_literals;
/*
    TODO: Implement the -S flag
    -S  - Run emission + writer only (don't assemble or link).
*/
constexpr auto FLAG_LEX = "--lex"s;
constexpr auto FLAG_PARSE = "--parse"s;
constexpr auto FLAG_CODEGEN = "--codegen"s;
constexpr auto FLAG_SKIP_CLEANUP = "--skip-cleanup"s;
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

DriverArgs parseDriverArgs(std::vector<std::string>& args);
} // namespace driver
} // namespace wacc