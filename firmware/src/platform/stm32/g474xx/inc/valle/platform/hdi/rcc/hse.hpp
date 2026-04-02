#pragma once

#include "valle/platform/hardware/rcc/hse.hpp"

namespace valle::platform
{
    template <typename T = void>
    struct HseOscillatorHdi : public HseOscillatorInterface
    {
        struct Descriptor : public UniqueDeviceDescriptor
        {
        };
    };

    // =============================================================================
    // HSE OSCILLATOR INFO HDI
    // =============================================================================
    template <typename T = void>
    class HseOscillatorInfoHdi
    {
    public:
        struct Descriptor : public SharedDeviceDescriptor
        {
            constexpr static bool skNeedsInit = false;
        };

    private:
        HseOscillatorInterface m_interface{};

    public:
        [[nodiscard]] bool is_ready() const
        {
            return m_interface.is_ready();
        }

        [[nodiscard]] constexpr uint32_t get_frequency_hz() const
        {
            return HseOscillatorInterface::skFrequencyHz;
        }
    };

}  // namespace valle::platform