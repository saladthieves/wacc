#pragma once

// TODO: Remove this
namespace wacc {
namespace utils {
template <typename T>
inline constexpr T* as(auto& pointer) {
    return pointer ? dynamic_cast<T*>(pointer.get()) : nullptr;
}
} // namespace utils
} // namespace wacc