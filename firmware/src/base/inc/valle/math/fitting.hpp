#pragma once

#include <concepts>

namespace valle
{

    template <std::floating_point T>
    T linear_interpolate(const T point0_x, const T point0_y, const T point1_x, const T point1_y, const T target_x)
    {
        // Linear interpolation formula:
        // y = y0 + ( (y1 - y0) / (x1 - x0) ) * (x - x0)
        const T slope = (point1_y - point0_y) / (point1_x - point0_x);
        return point0_y + (slope * (target_x - point0_x));
    }

}  // namespace valle