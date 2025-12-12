#pragma once

#include <string>

namespace wacc {
namespace utils {
class FileInfo {
public:
    std::string fileName{};
    std::string stem{};
    std::string extension{};
    std::string parent{};
    std::string path{};
};

FileInfo getFileInfo(const std::string& path);

std::string readFile(const std::string& path);

std::string decorate(unsigned int lineNo, std::string_view line,
                     unsigned int offset, std::string_view value);

namespace {
using ConstIter = std::string_view::const_iterator;
}

unsigned int getLineStop(ConstIter next, ConstIter begin, ConstIter end);

enum class PlatformType {
    MACOS = 1,
    LINUX,
    UNKNOWN,
};

class Platform {
public:
    Platform();
    
    Platform(PlatformType type);

    bool isMacOS() const { return type == PlatformType::MACOS; }

    bool isLinux() const { return type == PlatformType::LINUX; }

    bool isUnknown() const { return type == PlatformType::UNKNOWN; }

private:
    PlatformType type;
};
} // namespace utils
} // namespace wacc