#pragma once

#include "valle/platform/hardware/rcc/lsi.hpp"

namespace valle::platform
{
    template <typename T = void>
    struct LsiOscillatorHdi : public LsiOscillatorInterface
    {
        struct Descriptor : public UniqueDeviceDescriptor
        {
        };
    };

    // =========================================================================
    // LSI OSCILLATOR INFO HDI
    // =========================================================================
    template <typename T = void>
    class LsiOscillatorInfoHdi
    {
    public:
        struct Descriptor : public SharedDeviceDescriptor
        {
            constexpr static bool skNeedsInit = false;
        };

    private:
        LsiOscillatorInterface m_interface{};

    public:
        [[nodiscard]] bool is_ready() const
        {
            return m_interface.is_ready();
        }

        [[nodiscard]] constexpr uint32_t get_frequency_hz() const
        {
            return LsiOscillatorInterface::skFrequencyHz;
        }
    };

}  // namespace valle::platform