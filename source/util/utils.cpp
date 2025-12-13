#include "utils.hpp"

#include <filesystem>
#include <fstream>
#include <sstream>

namespace wacc::utils {
FileInfo getFileInfo(const std::string& path) {
    auto file = std::filesystem::absolute(path);
    // clang-format off
    return {
        file.filename(),
        file.stem(),
        file.extension(),
        file.parent_path(),
        file.string(),
    };
    // clang-format on
}

std::string readFile(const std::string& path) {
    std::ifstream file{path};
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

unsigned int getLineStop(ConstIter next, ConstIter begin, ConstIter end) {
    if (next == end) return next - begin;
    auto iter = next;
    while ((iter != end) && (*iter != '\n')) ++iter;
    return iter - begin;
}

// Platform
Platform::Platform() {
#ifdef WACC_HOST_MACOS
    type = PlatformType::MACOS;
#elif defined(WACC_HOST_LINUX)
    type = PlatformType::LINUX;
#else
    type = PlatformType::UNKNOWN;
#endif
}

Platform::Platform(PlatformType type) : type{type} {
}
} // namespace wacc::utils