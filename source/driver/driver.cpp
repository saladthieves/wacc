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

    try {
        const auto preprocessed = runPreprocessor(args);
        const auto [proceed, assembly] = runCompiler(preprocessed, args);

        if (proceed && !args.special) {
            runAssembler(assembly, args);
        }

        cleanUp(args, info, false);
    } catch (const std::runtime_error& ex) {
        cleanUp(args, info, true);
        throw ex;
    }
}

void cleanUp(const DriverArgs& args, const utils::FileInfo& info, bool failed) {
    if (!args.cleanUp) return;
    if (!std::filesystem::exists(info.path)) return;

    auto source = std::filesystem::path(info.path);
    auto directory = std::filesystem::directory_iterator{info.parent};

    for (auto& file : directory) {
        const auto& path = file.path();
        if (args.special && path.string().ends_with(".s")) {
            continue;
        }
        
        if (failed) {
            if (path != source) {
                std::filesystem::remove(path);
            }
        } else {
            if (path.has_extension() && path != source) {
                std::filesystem::remove(path);
            }
        }
    }
}
} // namespace wacc::driver