#pragma once

#include <string>
#include <vector>

namespace wacc {
namespace driver {
namespace {
using namespace std::string_literals;
// TODO: Implement flags functionality
constexpr auto FLAG_LEX = "--lex"s;
constexpr auto FLAG_PARSE = "--parse"s;
constexpr auto FLAG_CODEGEN = "--codegen"s;
constexpr auto FLAG_CLEANUP = "--cleanup";
constexpr auto COMPILER_CLANG = "clang";
constexpr auto COMPILER_GCC = "gcc";
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