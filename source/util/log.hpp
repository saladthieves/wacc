#pragma once

#include <iostream>

namespace wacc {
namespace log {
namespace {
template <typename... T>
constexpr inline void out(bool isError, std::format_string<T...> string,
                          T&&... args) {
    auto& stream = isError ? std::cerr : std::cout;
    std::println(stream, string, std::forward<T>(args)...);
}
} // namespace

template <typename... T>
constexpr inline void d(std::format_string<T...> string = "", T&&... args) {
    out(false, string, std::forward<T>(args)...);
}

template <typename... T>
constexpr inline void e(std::format_string<T...> string = "", T&&... args) {
    out(true, string, std::forward<T>(args)...);
}
} // namespace log
} // namespace wacc