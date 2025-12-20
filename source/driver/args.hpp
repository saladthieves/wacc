#pragma once

#include <string>
#include <vector>

namespace wacc {
namespace driver {
namespace {
using namespace std::string_view_literals;

constexpr auto FLAG_LEX = "--lex"sv;
constexpr auto FLAG_PARSE = "--parse"sv;
constexpr auto FLAG_TACKY = "--tacky"sv;
constexpr auto FLAG_CODEGEN = "--codegen"sv;
constexpr auto FLAG_SPECIAL = "-S"sv;
constexpr auto FLAG_SKIP_CLEANUP = "--skip-cleanup"sv;
constexpr auto COMPILER_CLANG = "clang"sv;
constexpr auto COMPILER_GCC = "gcc"sv;
} // namespace

class DriverArgs {
public:
    bool lex{false};
    bool parse{false};
    bool tacky{false};
    bool codegen{false};
    bool special{false};
    bool cleanUp{true};
    std::string path{""};
    std::string compiler{COMPILER_CLANG};
};

DriverArgs parseDriverArgs(std::vector<std::string>& args);
} // namespace driver
} // namespace wacc