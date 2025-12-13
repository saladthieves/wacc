#include "args.hpp"
#include <algorithm>
#include <stdexcept>
#include <vector>

namespace wacc::driver {
DriverArgs parseDriverArgs(std::vector<std::string>& args) {
    if (args.empty()) {
        throw std::runtime_error("No driver arguments provided.");
    }

    const auto findFlag = [&args](std::string_view flag) -> bool {
        auto result = std::find(args.begin(), args.end(), flag);
        if (result != args.end()) {
            args.erase(result);
            return true;
        } else {
            return false;
        }
    };

    bool lex{false};
    bool parse{false};
    bool codegen{false};
    bool special{false};

    if (findFlag(FLAG_LEX)) {
        lex = true;
        parse = codegen = special = false;
    } else if (findFlag(FLAG_PARSE)) {
        parse = true;
        lex = codegen = special = false;
    } else if (findFlag(FLAG_CODEGEN)) {
        codegen = true;
        lex = parse = special = false;
    } else if (findFlag(FLAG_SPECIAL)) {
        special = true;
        lex = parse = codegen = false;
    }

    const bool skipCleanup = findFlag(FLAG_SKIP_CLEANUP);

    for (const auto& flag : args) {
        if (flag.starts_with("-")) {
            throw std::runtime_error("Multiple flags provided instead of one.");
        }
    }

    if (args.empty()) {
        throw std::runtime_error("No source path provided.");
    }

    if (args.size() != 1) {
        throw std::runtime_error("More than one source path or flag provided.");
    }

    return {lex, parse, codegen, special, !skipCleanup, *args.begin()};
}
} // namespace wacc::driver