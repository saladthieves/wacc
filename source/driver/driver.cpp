#include "driver.hpp"
#include "args.hpp"
#include "assembler.hpp"
#include "compiler.hpp"
#include "preprocessor.hpp"
#include "utils.hpp"

#include <filesystem>
#include <stdexcept>

namespace wacc::driver {
void runDriver(std::vector<std::string>& arguments) {
    const auto args = parseDriverArgs(arguments);
    const auto info = utils::getFileInfo(args.path);

    auto source = std::filesystem::path{args.path};
    auto directory = std::filesystem::directory_iterator{info.parent};

    try {
        const auto preprocessed = runPreprocessor(args);
        const auto [proceed, assembly] = runCompiler(preprocessed, args);
        
        if (proceed) {
            runAssembler(assembly, args);
        }

        cleanUp(args, false);
    } catch (const std::runtime_error& ex) {
        cleanUp(args, true);
        throw ex;
    }
}

void cleanUp(const DriverArgs& args, bool failed) {
    if (!args.cleanUp) return;

    const auto info = utils::getFileInfo(args.path);
    auto source = std::filesystem::path{args.path};
    auto directory = std::filesystem::directory_iterator{info.parent};

    for (auto& file : directory) {
        const auto& path = file.path();
        if (path == source) continue;

        if (path.has_extension() || failed) {
            std::filesystem::remove(path);
        }
    }
}
} // namespace wacc::driver