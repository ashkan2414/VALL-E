#pragma once

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <span>
#include <variant>

namespace valle
{

    // ========================================================================
    // IDENTITY (Pass-through)
    // ========================================================================
    template <typename T>
    struct IdentityConverter
    {
        using ConfigT    = std::monostate;  // No config needed
        using InputT     = T;
        using ConvertedT = T;

        void init(const ConfigT& config)
        {
            (void)config;
        }

        [[nodiscard]] ConvertedT convert(const T raw) const
        {
            return raw;
        }
    };

    // ========================================================================
    // LINEAR (Scale + Offset)
    // ========================================================================
    struct LinearConverterConfig
    {
        float scale  = 1.0F;
        float offset = 0.0F;
    };

    // Formula: y = (x * scale) + offset
    template <typename T, std::floating_point TConverted = float>
    struct LinearConverter
    {
        using InputT     = T;
        using ConvertedT = TConverted;
        using ConfigT    = LinearConverterConfig;

        ConfigT m_cfg;

        LinearConverter() = default;

        LinearConverter(ConvertedT scale, ConvertedT offset) : m_cfg{scale, offset}
        {
        }

        explicit LinearConverter(const ConfigT& cfg) : m_cfg(cfg)
        {
        }

        void init(const ConfigT& cfg)
        {
            m_cfg = cfg;
        }

        [[nodiscard]] ConvertedT convert(const T raw) const
        {
            return (static_cast<ConvertedT>(raw) * m_cfg.scale) + m_cfg.offset;
        }
    };

    // ========================================================================
    // POLYNOMIAL (Correction Curve)
    // ========================================================================
    template <size_t tkOrder>
    struct PolynomialConverterConfig
    {
        std::array<float, tkOrder + 1> coeffs;  // [c0, c1, c2...]
    };

    // Formula: y = c0 + c1*x + c2*x^2 ...
    template <typename T, size_t tkOrder>
    struct PolynomialConverter
    {
        using InputT     = T;
        using ConvertedT = float;
        using ConfigT    = PolynomialConverterConfig<tkOrder>;

        ConfigT m_cfg;

        void init(const ConfigT& cfg)
        {
            m_cfg = cfg;
        }

        [[nodiscard]] ConvertedT convert(const T input) const
        {
            // coeffs = [c0, c1, c2 ... cOrder]
            // Start with the highest power coefficient
            float result = m_cfg.coeffs.back();

            // Iterate backwards from second-to-last to the beginning (c0)
            // Use integer index to avoid unsigned/size_t underflow issues in reverse loops
            for (int i = static_cast<int>(tkOrder) - 1; i >= 0; --i)
            {
                result = (result * input) + m_cfg.coeffs[i];
            }

            return result;
        }
    };

    // ========================================================================
    // INTERPOLATED LOOKUP TABLE (Static/Flash)
    // ========================================================================
    struct LookupTablePoint
    {
        float x;
        float y;
    };

    template <size_t tkSize>
    struct LookupTableConverterConfig
    {
        std::array<LookupTablePoint, tkSize> table;
    };

    // Best for NTC Thermistors where you have a CSV of Volt -> Temp
    template <typename T, size_t tkSize>
    struct LookupTableConverter
    {
        using InputT     = T;
        using ConvertedT = float;
        using ConfigT    = LookupTableConverterConfig<tkSize>;

        ConfigT m_cfg;

        void init(const ConfigT& cfg)
        {
            m_cfg = cfg;
            // Ensure sorted by X for binary search
            std::sort(m_cfg.table.begin(),
                      m_cfg.table.end(),
                      [](const LookupTablePoint& point_a, const LookupTablePoint& point_b)
                      { return point_a.x < point_b.x; });
        }

        [[nodiscard]] ConvertedT convert(const T raw_x) const
        {
            // Clamp to range
            if (raw_x <= m_cfg.table.front().x)
            {
                return m_cfg.table.front().y;
            }
            if (raw_x >= m_cfg.table.back().x)
            {
                return m_cfg.table.back().y;
            }

            // Binary Search (std::upper_bound)
            auto bs_it = std::upper_bound(m_cfg.table.begin(),
                                          m_cfg.table.end(),
                                          raw_x,
                                          [](float val, const LookupTablePoint& point) { return val < point.x; });

            const LookupTablePoint& left_point  = *(bs_it - 1);  // Point before p2
            const LookupTablePoint& right_point = *bs_it;        // First point with x > raw_x
            return linear_interpolate(left_point.x, left_point.y, right_point.x, right_point.y, raw_x);
        }
    };

}  // namespace valle
