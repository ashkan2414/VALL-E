#pragma once
#include <concepts>
#include <numbers>

namespace valle
{
    template <std::floating_point T>
    [[nodiscard]] constexpr T deg2rad(const T degrees) noexcept
    {
        return degrees * std::numbers::pi_v<T> / static_cast<T>(180);
    }

    template <std::floating_point T>
    [[nodiscard]] constexpr T rad2deg(const T radians) noexcept
    {
        return radians * static_cast<T>(180) / std::numbers::pi_v<T>;
    }
}  // namespace valle