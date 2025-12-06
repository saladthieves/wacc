#pragma once

#include <gtest/gtest.h>

namespace wacc {
namespace test {
namespace utils {
template <typename T>
inline constexpr T* as(auto& pointer) {
    assert(pointer != nullptr);
    if (pointer) {
        auto result = dynamic_cast<T*>(pointer.get());
        assert(result != nullptr);
        return result;
    }

    return nullptr;
}
} // namespace utils
} // namespace test
} // namespace wacc