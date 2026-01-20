#pragma once

#include <Eigen/Dense>

template <typename T>
T precise_dot(const Eigen::Vector2<T>& a, const Eigen::Vector2<T>& b)
{
    if constexpr (std::is_floating_point_v<T>)
    {
        // Force fma: a[0]*b[0] + (a[1]*b[1])
        return std::fma(a[0], b[0], a[1] * b[1]);
    }
    else
    {
        return a.dot(b);
    }
}
