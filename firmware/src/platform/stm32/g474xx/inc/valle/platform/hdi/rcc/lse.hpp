#pragma once

#include "valle/platform/hardware/rcc/lse.hpp"

namespace valle::platform
{
    template <typename T = void>
    struct LseOscillatorHdi : public LseOscillatorInterface
    {
        struct Descriptor : public UniqueDeviceDescriptor
        {
        };
    };

    // =========================================================================
    // LSE OSCILLATOR INFO HDI
    // =========================================================================
    template <typename T = void>
    class LseOscillatorInfoHdi
    {
    public:
        struct Descriptor : public SharedDeviceDescriptor
        {
            constexpr static bool skNeedsInit = false;
        };

    private:
        LseOscillatorInterface m_interface{};

    public:
        [[nodiscard]] bool is_ready() const
        {
            return m_interface.is_ready();
        }

        [[nodiscard]] LseOscillatorDriveCapability get_drive_capability() const
        {
            return m_interface.get_drive_capability();
        }

        [[nodiscard]] constexpr uint32_t get_frequency_hz() const
        {
            return LseOscillatorInterface::skFrequencyHz;
        }
    };

}  // namespace valle::platform