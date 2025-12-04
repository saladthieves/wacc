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
} // namespace utils
} // namespace wacc