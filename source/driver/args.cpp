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
    bool tacky{false};
    bool codegen{false};
    bool special{false};

    if (findFlag(FLAG_LEX)) {
        lex = true;
    } else if (findFlag(FLAG_PARSE)) {
        parse = true;
    } else if (findFlag(FLAG_TACKY)) {
        tacky = true;
    } else if (findFlag(FLAG_CODEGEN)) {
        codegen = true;
    } else if (findFlag(FLAG_SPECIAL)) {
        special = true;
    }

    const bool skipCleanup = findFlag(FLAG_SKIP_CLEANUP);

    const auto hasMultiple =
        std::any_of(args.begin(), args.end(), [](const auto& flag) {
            return flag.starts_with("-"); //
        });

    if (hasMultiple) {
        throw std::runtime_error("Multiple flags provided instead of one.");
    }

    if (args.empty()) {
        throw std::runtime_error("No source path provided.");
    }

    if (args.size() != 1) {
        throw std::runtime_error("More than one source path or flag provided.");
    }

    return {lex, parse, tacky, codegen, special, !skipCleanup, *args.begin()};
}
} // namespace wacc::driver