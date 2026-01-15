#include "cmd.hpp"

#include <cstdlib>
#include <format>
#include <numeric>

namespace wacc::utils {
CommandResult runCommand(std::string_view tool,
                         std::initializer_list<std::string> args) {
    auto command = std::accumulate(args.begin(), args.end(), std::string{tool},
                                   [](const auto& left, const auto& right) {
                                       return std::format("{} {}", left, right);
                                   });

    int exitCode = std::system(command.c_str());

    return {
        command,
        exitCode,
        exitCode == 0,
    };
}
} // namespace wacc::utils