#pragma once

#include "valle/platform/hardware/rcc/hsi.hpp"

namespace valle::platform
{
    template <typename T = void>
    struct HsiOscillatorHdi : public HsiOscillatorInterface
    {
        struct Descriptor : public UniqueDeviceDescriptor
        {
        };
    };

    // =========================================================================
    // HSI OSCILLATOR INFO HDI
    // =========================================================================
    template <typename T = void>
    class HsiOscillatorInfoHdi
    {
    public:
        struct Descriptor : public SharedDeviceDescriptor
        {
            constexpr static bool skNeedsInit = false;
        };

    private:
        HsiOscillatorInterface m_interface{};

    public:
        [[nodiscard]] bool is_ready() const
        {
            return m_interface.is_ready();
        }

        [[nodiscard]] uint32_t get_calibration() const
        {
            return m_interface.get_calibration();
        }

        [[nodiscard]] constexpr uint32_t get_frequency_hz() const
        {
            return HsiOscillatorInterface::skFrequencyHz;
        }
    };

}  // namespace valle::platform