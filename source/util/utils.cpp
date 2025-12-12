#include "utils.hpp"

#include <filesystem>
#include <format>
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

std::string decorate(unsigned int lineNo, std::string_view line,
                     unsigned int offset, std::string_view value) {
    constexpr char c = '^';

    auto output = std::format("Line [{}]: ", lineNo);
    const auto prefixLength = output.length();

    output += std::format("{}\n", line);
    const auto totalLength = output.length();

    output += std::string(offset + prefixLength, ' ');
    output += std::string(value.length(), c);
    output += '\n';

    return output;
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