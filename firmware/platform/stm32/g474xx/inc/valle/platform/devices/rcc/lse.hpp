#pragma once

#include "valle/core/device/device.hpp"
#include "valle/platform/core.hpp"
#include "valle/platform/devices/power.hpp"
#include "valle/platform/hardware/rcc/lse.hpp"

namespace valle
{
    // =============================================================================
    // FORWARD DECLARATIONS
    // =============================================================================
    class LSEOscillatorRootDevice;
    template <typename T>
    class LSEOscillatorDevice;

    // =============================================================================
    // ROOT LSE OSCILLATOR DEVICE
    // =============================================================================
    class LSEOscillatorRootDevice
    {
    public:
        struct Descriptor : public InterfaceDeviceDescriptor
        {
            using Children = DeviceTreeList<LSEOscillatorDevice<void>>;
        };
    };

    // =========================================================================
    // LSE Oscillator INFO DEVICE
    // =========================================================================
    template <typename T = void>
    class LSEOscillatorInfoDevice
    {
    public:
        struct Descriptor : public SharedDeviceDescriptor
        {
            constexpr static bool skNeedsInit = false;
        };

        using InterfaceT = LSEOscillatorInterface;

        using DependDevices = TypeList<LSEOscillatorRootDevice>;
        using InjectDevices = TypeList<>;

        [[nodiscard]] bool is_ready() const
        {
            return InterfaceT::is_ready();
        }

        [[nodiscard]] LSEOscillatorDriveCapability get_drive_capability() const
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
    struct LSEOscillatorConfig
    {
        bool                         enabled = true;
        LSEOscillatorMode            mode    = LSEOscillatorMode::kCrystal;
        LSEOscillatorDriveCapability drive   = LSEOscillatorDriveCapability::kLow;

        [[nodiscard]] constexpr uint32_t get_frequency_hz() const
        {
            return enabled ? LSEOscillatorInterface::skFrequencyHz : 0U;
        }
    };

    // --------------------------------------------------------------------------------
    // DEVICE
    // --------------------------------------------------------------------------------
    template <typename T = void>
    class LSEOscillatorDevice
    {
    public:
        struct Descriptor : public UniqueDeviceDescriptor
        {
        };

        using InterfaceT = LSEOscillatorInterface;

        using DependDevices = TypeList<LSEOscillatorRootDevice, PowerDevice<>>;
        using InjectDevices = TypeList<>;

        [[nodiscard]] bool init(const LSEOscillatorConfig& config)
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
                InterfaceT::set_drive_capability(LSEOscillatorDriveCapability::kHigh);
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
            return PlatformTimingUtils::wait_for_with_timeout_countdown([]() { return InterfaceT::is_ready(); },
                                                                        timeout_count);
        }

        [[nodiscard]] static bool wait_for_not_ready(const uint32_t timeout_count)
        {
            return PlatformTimingUtils::wait_for_with_timeout_countdown(
                []() -> bool { return !InterfaceT::is_ready(); }, timeout_count);
        }
    };

}  // namespace valle