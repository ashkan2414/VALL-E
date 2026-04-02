#pragma once

namespace valle
{
    struct CtNullopt
    {
        explicit constexpr CtNullopt() = default;
    };

    inline constexpr CtNullopt ct_nullopt{};

    // A C++20 Structural-Type friendly optional
    template <typename T>
    struct CtOptional
    {
        T    m_value{};
        bool m_has_value = false;

        // Default constructor (Empty)
        constexpr CtOptional() = default;

        // Construct from ct_nullopt (Empty)
        constexpr CtOptional(CtNullopt) : m_value{}, m_has_value(false)
        {
        }

        // Value constructor (Implicit to make assignment clean)
        constexpr CtOptional(T v) : m_value(v), m_has_value(true)
        {
        }

        constexpr void reset()
        {
            m_has_value = false;
        }

        // Convenience accessors
        constexpr bool has_value() const
        {
            return m_has_value;
        }

        constexpr const T& value() const
        {
            return m_value;
        }

        constexpr const T* operator->() const
        {
            return &m_value;
        }

        constexpr explicit operator bool() const
        {
            return m_has_value;
        }
    };

}  // namespace valle