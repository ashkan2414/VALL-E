#pragma once

#include <concepts>

template <std::floating_point T>
T linear_interpolate(const T x0, const T y0, const T x1, const T y1, const T x)
{
    // Linear interpolation formula:
    // y = y0 + ( (y1 - y0) / (x1 - x0) ) * (x - x0)
    const T slope = (y1 - y0) / (x1 - x0);
    return y0 + slope * (x - x0);
}