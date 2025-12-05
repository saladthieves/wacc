#include "utils.hpp"

#include <filesystem>
#include <fstream>
#include <sstream>

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

std::string readFile(const std::string& path) {
    std::ifstream file{path};
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}
} // namespace wacc::utils