#include "compiler.hpp"
#include "cmd.hpp"
#include "utils.hpp"

#include <filesystem>
#include <format>
#include <stdexcept>

namespace wacc::driver {
std::string runCompiler(const std::string& preprocessed, const DriverArgs& args) {
    if (!std::filesystem::exists(preprocessed)) {
        auto message =
            std::format("The provided preprocessed file does not exist: [{}]",
                        preprocessed);
        throw std::runtime_error(message);
    }

    if (!preprocessed.ends_with(".i")) {
        throw std::runtime_error("The preprocessed file must end in .i");
    }

    auto info = utils::getFileInfo(preprocessed);

    auto output = std::filesystem::path{info.parent};
    output /= std::format("{}.{}", info.stem, "s");

    auto result = utils::runCommand(
        args.compiler, {"-S", "-O", "-fno-asynchronous-unwind-tables",
                        "-fcf-protection=none", preprocessed, "-o", output});

    if (!result.success) {
        auto message = std::format("Running [{}] failed: [exitCode: {}]",
                                   result.command, result.exitCode);
        throw std::runtime_error(message);
    }

    std::filesystem::remove(preprocessed);

    return output;
}
} // namespace wacc::driver