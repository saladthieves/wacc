#include "utils.hpp"

#include <filesystem>

namespace wacc::utils {
FileInfo getFileInfo(const std::string& path) {
    auto file = std::filesystem::path{path};
    // clang-format off
    return {
        file.filename(),
        file.stem(),
        file.extension(),
        file.parent_path(),
        path,
    };
    // clang-format on
}
} // namespace wacc::utils