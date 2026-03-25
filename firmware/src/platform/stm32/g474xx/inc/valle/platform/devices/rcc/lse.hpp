#pragma once

#include "valle/platform/core.hpp"
#include "valle/platform/devices/power.hpp"
#include "valle/platform/hardware/rcc/lse.hpp"

namespace valle::platform
{
    // =========================================================================
    // LSE Oscillator INFO DEVICE
    // =========================================================================
    template <typename T = void>
    class LseOscillatorInfoDevice
    {
    public:
        struct Descriptor : public SharedDeviceDescriptor
        {
            constexpr static bool skNeedsInit = false;
        };

        using InterfaceT = LseOscillatorInterface;

        using InjectDevices = TypeList<>;

        [[nodiscard]] bool is_ready() const
        {
            return InterfaceT::is_ready();
        }

        [[nodiscard]] LseOscillatorDriveCapability get_drive_capability() const
        {
            return InterfaceT::get_drive_capability();
        }

        [[nodiscard]] constexpr uint32_t get_frequency_hz() const
        {
            return InterfaceT::skFrequencyHz;
        }
    };

    // =============================================================================
    // LSE OSCILLATOR DEVICE
    // =============================================================================

    // --------------------------------------------------------------------------------
    // CONFIGURATION
    // --------------------------------------------------------------------------------
    struct LseOscillatorConfig
    {
        bool                         enabled = true;
        LseOscillatorMode            mode    = LseOscillatorMode::kCrystal;
        LseOscillatorDriveCapability drive   = LseOscillatorDriveCapability::kLow;

        [[nodiscard]] constexpr uint32_t get_frequency_hz() const
        {
            return enabled ? LseOscillatorInterface::skFrequencyHz : 0U;
        }
    };

    // --------------------------------------------------------------------------------
    // DEVICE
    // --------------------------------------------------------------------------------
    template <typename T = void>
    class LseOscillatorDevice
    {
    public:
        struct Descriptor : public UniqueDeviceDescriptor
        {
        };

        using InterfaceT = LseOscillatorInterface;

        using DependDevices = TypeList<PowerDevice<>>;
        using InjectDevices = TypeList<>;

        [[nodiscard]] bool init(const LseOscillatorConfig& config)
        {
            // LL_PWR_EnableBkUpAccess() must be called before enabling LSE, otherwise LSE won't start.
            // This is a quirk of the STM32G4 hardware.
            if (config.enabled)
            {
                if (InterfaceT::is_ready())
                {
                    // If LSE is already enabled, just update the drive capability
                    InterfaceT::set_drive_capability(config.drive);
                    return true;
                }

                // Set initial drive capability to high to ensure reliable startup,
                // then reduce it to the desired level after the oscillator is stable.
                InterfaceT::set_drive_capability(LseOscillatorDriveCapability::kHigh);
                InterfaceT::enable(config.mode);

                expect(wait_for_ready(InterfaceT::skDefaultEnableTimeoutCount),
                       "LSE failed to become ready within timeout");

                InterfaceT::set_drive_capability(config.drive);

                return true;
            }

            InterfaceT::disable();
            expect(wait_for_not_ready(InterfaceT::skDefaultDisableTimeoutCount),
                   "LSE failed to disable within timeout");

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