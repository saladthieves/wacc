#include "assembler.hpp"
#include "cmd.hpp"
#include "utils.hpp"

#include <filesystem>
#include <format>
#include <stdexcept>

namespace wacc::driver {
void runAssembler(const std::string& assembly, const DriverArgs& args) {
    if (!std::filesystem::exists(assembly)) {
        auto message = std::format(
            "The provided assembly file does not exist: [{}]", assembly);
        throw std::runtime_error(message);
    }

    if (!assembly.ends_with(".s")) {
        throw std::runtime_error("The assembly file must end in .s");
    }

    auto info = utils::getFileInfo(assembly);

    auto output = std::filesystem::path{info.parent} / info.stem;

    auto result = utils::runCommand(args.compiler, {assembly, "-o", output});
    if (!result.success) {
        auto message = std::format("Running [{}] failed: [exitCode: {}]",
                                   result.command, result.exitCode);
        throw std::runtime_error(message);
    }

    std::filesystem::remove(assembly);
}
} // namespace wacc::driver