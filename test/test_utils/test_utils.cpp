#include "test_utils.hpp"

#include <filesystem>

namespace wacc::test::utils::samples {
void cleanUpSamples(std::function<bool(const std::string&)> condition) {
    auto directory = std::filesystem::directory_iterator{
        std::filesystem::absolute(rootFolder)};
    for (auto& file : directory) {
        std::string path = file.path();
        if (condition(path)) {
            std::filesystem::remove(path);
        }
    }
}
} // namespace wacc::test::utils::samples