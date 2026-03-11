#pragma once

#include <Eigen/Dense>

namespace valle
{

    template <typename T>
    T precise_dot(const Eigen::Vector2<T>& vec1, const Eigen::Vector2<T>& vec2)
    {
        if constexpr (std::is_floating_point_v<T>)
        {
            // Force fma: vec1[0]*vec2[0] + (vec1[1]*vec2[1])
            return std::fma(vec1[0], vec2[0], vec1[1] * vec2[1]);
        }
        else
        {
            return vec1.dot(vec2);
        }
    }

}  // namespace valle