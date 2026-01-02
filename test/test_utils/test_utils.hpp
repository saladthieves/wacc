#pragma once

#include <expected>
#include <format>
#include <functional>
#include <stdexcept>

namespace wacc {
namespace test {
namespace utils {

namespace samples {
namespace {
using namespace std::string_literals;

static const auto root = "test";
static const auto name = "test_file";
} // namespace

static const auto rootFolder = std::format("{}/test_sample", root);
static const auto sampleSource = std::format("{}/{}.{}", rootFolder, name, "c");
static const auto samplePrep = std::format("{}/{}.{}", rootFolder, name, "i");
static const auto sampleAsm = std::format("{}/{}.{}", rootFolder, name, "s");
static const auto sampleBin = std::format("{}/{}", rootFolder, name);
static constexpr auto sampleCode = "int main(void) { return ~(-(~25)); }";  // TODO: Remove this

void cleanUpSamples(std::function<bool(const std::string&)> condition);

} // namespace samples

template <typename T>
inline constexpr T* as(auto& pointer) {
    if (pointer == nullptr) {
        throw std::logic_error("Pointer is null before dynamic_cast");
    }

    auto result = dynamic_cast<T*>(pointer.get());

    if (result == nullptr) {
        throw std::logic_error("Pointer is null after dynamic_cast");
    }

    return result;
}
} // namespace utils
} // namespace test
} // namespace wacc