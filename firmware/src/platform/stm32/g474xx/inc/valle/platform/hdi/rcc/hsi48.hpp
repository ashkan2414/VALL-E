#pragma once

#include "valle/platform/hardware/rcc/hsi48.hpp"

namespace valle::platform
{
    template <typename T = void>
    struct Hsi48OscillatorHdi : public Hsi48OscillatorInterface
    {
        struct Descriptor : public UniqueDeviceDescriptor
        {
        };
    };

    // =========================================================================
    // HSI48 OSCILLATOR INFO HDI
    // =========================================================================
    template <typename T = void>
    class Hsi48OscillatorInfoHdi
    {
    public:
        struct Descriptor : public SharedDeviceDescriptor
        {
            constexpr static bool skNeedsInit = false;
        };

    private:
        HSI48OscillatorInterface m_interface{};

    public:
        [[nodiscard]] bool is_ready() const
        {
            return m_interface.is_ready();
        }

        [[nodiscard]] constexpr uint32_t get_frequency_hz() const
        {
            return HSI48OscillatorInterface::skFrequencyHz;
        }
    };

}  // namespace valle::platform