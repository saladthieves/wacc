#pragma once

#include <string>
#include <string_view>

namespace wacc {
namespace utils {
class CommandResult {
public:
    std::string command{};
    int exitCode{0};
    bool success{true};
};

CommandResult runCommand(std::string_view tool,
                         std::initializer_list<std::string> args);
} // namespace utils
} // namespace wacc