#include "args.hpp"
#include <algorithm>
#include <stdexcept>
#include <vector>

namespace wacc::driver {
DriverArgs parseDriverArgs(std::vector<std::string>& args) {
    if (args.empty()) {
        throw std::runtime_error("No driver arguments provided.");
    }

    const auto assign = [&args](const std::string& flag) -> bool {
        auto result = std::find(args.begin(), args.end(), flag);
        if (result != args.end()) {
            args.erase(result);
            return true;
        } else {
            return false;
        }
    };

    const bool lex = assign(FLAG_LEX);
    const bool parse = assign(FLAG_PARSE);
    const bool codegen = assign(FLAG_CODEGEN);

    if (args.empty()) {
        throw std::runtime_error("No source path provided.");
    }

    if (args.size() != 1) {
        throw std::runtime_error("More than one source path provided.");
    }

    return {lex, parse, codegen, *args.begin()};
}
} // namespace wacc::driver