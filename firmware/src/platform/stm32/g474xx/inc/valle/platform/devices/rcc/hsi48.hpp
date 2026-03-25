#pragma once

#include "valle/platform/core.hpp"
#include "valle/platform/hardware/rcc/hsi48.hpp"

namespace valle::platform
{
    // =========================================================================
    // Hsi48 Oscillator INFO DEVICE
    // =========================================================================
    template <typename T = void>
    class Hsi48OscillatorInfoDevice
    {
    public:
        struct Descriptor : public SharedDeviceDescriptor
        {
            constexpr static bool skNeedsInit = false;
        };

        using InterfaceT = Hsi48OscillatorInterface;

        using InjectDevices = TypeList<>;

        [[nodiscard]] bool is_ready() const
        {
            return InterfaceT::is_ready();
        }

        [[nodiscard]] constexpr uint32_t get_frequency_hz() const
        {
            return InterfaceT::skFrequencyHz;
        }
    };

    // =========================================================================
    // Hsi48 Oscillator DEVICE
    // =========================================================================

    // -----------------------------------------------------------------------------
    // CONFIGURATION
    // -----------------------------------------------------------------------------
    struct Hsi48OscillatorConfig
    {
        bool enabled = true;

        [[nodiscard]] constexpr uint32_t get_frequency_hz() const
        {
            return enabled ? Hsi48OscillatorInterface::skFrequencyHz : 0U;
        }
    };

    // -----------------------------------------------------------------------------
    // DEVICE
    // -----------------------------------------------------------------------------
    template <typename T = void>
    class Hsi48OscillatorDevice
    {
    public:
        struct Descriptor : public UniqueDeviceDescriptor
        {
        };

        using InterfaceT = Hsi48OscillatorInterface;

        using InjectDevices = TypeList<>;

        [[nodiscard]] bool init(const Hsi48OscillatorConfig& config)
        {
            if (config.enabled)
            {
                InterfaceT::enable();

                expect(wait_for_ready(InterfaceT::skDefaultEnableTimeoutCount),
                       "Hsi48 failed to become ready within timeout");

                return true;
            }

            InterfaceT::disable();

            expect(wait_for_not_ready(InterfaceT::skDefaultDisableTimeoutCount),
                   "Hsi48 failed to disable within timeout");

            return true;
        }

    private:
        [[nodiscard]] static bool wait_for_ready(const uint32_t timeout_count)
        {
            return TimingContext::wait_for_with_timeout_countdown([]() { return InterfaceT::is_ready(); },
                                                                  timeout_count);
        }

        [[nodiscard]] static bool wait_for_not_ready(const uint32_t timeout_count)
        {
            return TimingContext::wait_for_with_timeout_countdown([]() -> bool { return !InterfaceT::is_ready(); },
                                                                  timeout_count);
        }
    };

}  // namespace valle::platform