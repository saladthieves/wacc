#include "driver.hpp"
#include "args.hpp"
#include "assembler.hpp"
#include "compiler.hpp"
#include "preprocessor.hpp"
#include "utils.hpp"

#include <filesystem>
#include <format>
#include <stdexcept>
#include <vector>

namespace wacc::driver {
void runDriver(std::vector<std::string>& arguments) {
    const auto args = parseDriverArgs(arguments);
    const auto info = utils::getFileInfo(args.path);

    try {
        const auto preprocessed = runPreprocessor(args);
        const auto [proceed, assembly] = runCompiler(preprocessed, args);

        if (proceed && !args.special) {
            runAssembler(assembly, args);
        }

        cleanUp(args, info, false);
    } catch (const std::runtime_error& ex) {
        cleanUp(args, info, true);
        throw std::runtime_error(ex);
    }
}

void cleanUp(const DriverArgs& args, const utils::FileInfo& info, bool failed) {
    if (!args.cleanUp) return;
    if (!std::filesystem::exists(info.path)) return;

    auto source = std::filesystem::path(info.path);
    auto directory = std::filesystem::directory_iterator{info.parent};

    auto root = std::filesystem::path{info.parent};
    auto prepFile = root / std::format("{}.{}", info.stem, "i");
    auto asmFile = root / std::format("{}.{}", info.stem, "s");
    auto binFile = root / info.stem;

    std::vector<std::filesystem::path> paths{prepFile};

    if (!args.special) {
        paths.push_back(asmFile);
    }

    if (failed) {
        paths.push_back(binFile);
    }

    for (const auto& path : paths) {
        std::filesystem::remove(path);
    }
}
} // namespace wacc::driver