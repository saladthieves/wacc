#include "preprocessor.hpp"
#include "cmd.hpp"
#include "utils.hpp"

#include <filesystem>
#include <format>
#include <stdexcept>

namespace wacc::driver {
std::string runPreprocessor(const DriverArgs& args) {
    const auto& path = args.path;

    if (!std::filesystem::exists(path)) {
        auto message =
            std::format("The provided source file does not exist: [{}]", path);
        throw std::runtime_error(message);
    }

    if (!path.ends_with(".c")) {
        throw std::runtime_error("The provided source file must end in .c");
    }

    auto info = utils::getFileInfo(path);

    auto output = std::filesystem::path{info.parent};
    output /= std::format("{}.{}", info.stem, "i");

    auto result =
        utils::runCommand(args.compiler, {"-E", "-P", path, "-o", output});

    if (!result.success) {
        auto message = std::format("Running [{}] failed: [exitCode: {}]",
                                   result.command, result.exitCode);
        throw std::runtime_error(message);
    }

    return output;
}
} // namespace wacc::driver