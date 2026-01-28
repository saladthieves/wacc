#pragma once

#include <random>
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

namespace {
using ConstIter = std::string_view::const_iterator;
}

unsigned int getLineStop(ConstIter next, ConstIter begin, ConstIter end);

template <typename T = unsigned int>
T generateRandom(T first, T last) {
    std::random_device device{};
    auto engine = std::mt19937{device()};
    auto distribution = std::uniform_int_distribution<T>{first, last};
    return distribution(engine);
}

enum class PlatformType : std::uint8_t {
    MACOS = 1,
    LINUX,
    UNKNOWN,
};

class Platform {
public:
    Platform();

    explicit Platform(PlatformType type);

    bool isMacOS() const { return type == PlatformType::MACOS; }

    bool isLinux() const { return type == PlatformType::LINUX; }

    bool isUnknown() const { return type == PlatformType::UNKNOWN; }

private:
    PlatformType type;
};
} // namespace utils
} // namespace wacc