#pragma once

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <span>
#include <variant>

// ========================================================================
// IDENTITY (Pass-through)
// ========================================================================
template <typename T>
struct IdentityConverter
{
    using Config     = std::monostate;  // No config needed
    using InputT     = T;
    using ConvertedT = T;

    inline void init(const Config&)
    {
    }

    [[nodiscard]] inline ConvertedT convert(const T raw) const
    {
        return raw;
    }
};

// ========================================================================
// LINEAR (Scale + Offset)
// ========================================================================
// Formula: y = (x * scale) + offset
template <typename T>
struct LinearConverter
{
    struct Config
    {
        float scale  = 1.0f;
        float offset = 0.0f;
    };
    using InputT     = T;
    using ConvertedT = float;

    Config m_cfg;

    inline void init(const Config& cfg)
    {
        m_cfg = cfg;
    }

    [[nodiscard]] inline ConvertedT convert(const T raw) const
    {
        return (raw * m_cfg.scale) + m_cfg.offset;
    }
};

// ========================================================================
// POLYNOMIAL (Correction Curve)
// ========================================================================
// Formula: y = c0 + c1*x + c2*x^2 ...
template <typename T, size_t Order>
struct PolynomialConverter
{
    struct Config
    {
        std::array<float, Order + 1> coeffs;  // [c0, c1, c2...]
    };

    using InputT     = T;
    using ConvertedT = float;

    Config m_cfg;

    inline void init(const Config& cfg)
    {
        m_cfg = cfg;
    }

    [[nodiscard]] ConvertedT convert(const T x) const
    {
        // coeffs = [c0, c1, c2 ... cOrder]
        // Start with the highest power coefficient
        float result = m_cfg.coeffs.back();

        // Iterate backwards from second-to-last to the beginning (c0)
        // Use integer index to avoid unsigned/size_t underflow issues in reverse loops
        for (int i = static_cast<int>(Order) - 1; i >= 0; --i)
        {
            result = (result * x) + m_cfg.coeffs[i];
        }

        return result;
    }
};

// ========================================================================
// INTERPOLATED LOOKUP TABLE (Static/Flash)
// ========================================================================
// Best for NTC Thermistors where you have a CSV of Volt -> Temp
template <typename T, size_t tkSize>
struct LookupTableConverter
{
    struct Point
    {
        float x;
        float y;
    };

    struct Config
    {
        std::array<Point, tkSize> table;
    };

    using InputT     = T;
    using ConvertedT = float;

    Config m_cfg;

    inline void init(const Config& cfg)
    {
        m_cfg = cfg;
        // Ensure sorted by X for binary search
        std::sort(m_cfg.table.begin(), m_cfg.table.end(), [](auto& a, auto& b) { return a.x < b.x; });
    }

    [[nodiscard]] ConvertedT convert(const T raw_x) const
    {
        // Clamp to range
        if (raw_x <= m_cfg.table.front().x) return m_cfg.table.front().y;
        if (raw_x >= m_cfg.table.back().x) return m_cfg.table.back().y;

        // Binary Search (std::upper_bound)
        auto it = std::upper_bound(
            m_cfg.table.begin(), m_cfg.table.end(), raw_x, [](float val, const Point& p) { return val < p.x; });

        const Point& p1 = *(it - 1);  // Point before p2
        const Point& p2 = *it;        // First point with x > raw_x
        return linear_interpolate(p1.x, p1.y, p2.x, p2.y, raw_x);
    }
};
