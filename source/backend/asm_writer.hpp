#pragma once

#include <filesystem>
#include <format>
#include <memory>
#include <string>
#include <vector>

namespace wacc {
namespace back {
namespace write {
class AsmWriter {
    using Line = std::string;
    using Lines = std::vector<Line>;
    using LinesPtr = std::unique_ptr<Lines>;

public:
    AsmWriter(LinesPtr ptr, std::filesystem::path assembly);

    void write();

    template <typename... T>
    [[noreturn]] void fail(std::format_string<T...> str, T&&... args) const {
        const auto message = std::format(str, std::forward<T>(args)...);
        throw std::runtime_error(std::format("AsmWriterError: {}", message));
    }

private:
    LinesPtr lines{nullptr};
    std::filesystem::path assembly{};
};
} // namespace write
} // namespace back
} // namespace wacc