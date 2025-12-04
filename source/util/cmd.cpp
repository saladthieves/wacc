#include "cmd.hpp"

#include <cstdlib>
#include <format>

namespace wacc::utils {
CommandResult runCommand(std::string_view tool,
                         std::initializer_list<std::string> args) {

    auto command = std::format("{}", tool);
    for (const auto& arg : args) {
        command += std::format(" {}", arg);
    }

    int exitCode = std::system(command.c_str());
    
    return {
        command,
        exitCode,
        exitCode == 0,
    };
}
} // namespace wacc::utils