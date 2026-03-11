#pragma once

#include "valle/platform/core.hpp"
#include "valle/platform/hardware/rcc/hsi48.hpp"


namespace valle::platform
{
    // =============================================================================
    // FORWARD DECLARATIONS
    // =============================================================================
    class HSI48OscillatorRootDevice;
    template <typename T>
    class HSI48OscillatorDevice;

    // =============================================================================
    // ROOT HSI48 OSCILLATOR DEVICE
    // =============================================================================
    class HSI48OscillatorRootDevice
    {
    public:
        struct Descriptor : public InterfaceDeviceDescriptor
        {
            using Children = DeviceTreeList<HSI48OscillatorDevice<void>>;
        };
    };

    // =========================================================================
    // HSI48 Oscillator INFO DEVICE
    // =========================================================================
    template <typename T = void>
    class HSI48OscillatorInfoDevice
    {
    public:
        struct Descriptor : public SharedDeviceDescriptor
        {
            constexpr static bool skNeedsInit = false;
        };

        using InterfaceT = HSI48OscillatorInterface;

        using DependDevices = TypeList<HSI48OscillatorRootDevice>;
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
    // HSI48 Oscillator DEVICE
    // =========================================================================

    // -----------------------------------------------------------------------------
    // CONFIGURATION
    // -----------------------------------------------------------------------------
    struct HSI48OscillatorConfig
    {
        bool enabled = true;

        [[nodiscard]] constexpr uint32_t get_frequency_hz() const
        {
            return enabled ? HSI48OscillatorInterface::skFrequencyHz : 0U;
        }
    };

    // -----------------------------------------------------------------------------
    // DEVICE
    // -----------------------------------------------------------------------------
    template <typename T = void>
    class HSI48OscillatorDevice
    {
    public:
        struct Descriptor : public UniqueDeviceDescriptor
        {
        };

        using InterfaceT = HSI48OscillatorInterface;

        using DependDevices = TypeList<HSI48OscillatorRootDevice>;
        using InjectDevices = TypeList<>;

        [[nodiscard]] bool init(const HSI48OscillatorConfig& config)
        {
            if (config.enabled)
            {
                InterfaceT::enable();

                expect(wait_for_ready(InterfaceT::skDefaultEnableTimeoutCount),
                       "HSI48 failed to become ready within timeout");

                return true;
            }

            InterfaceT::disable();

            expect(wait_for_not_ready(InterfaceT::skDefaultDisableTimeoutCount),
                   "HSI48 failed to disable within timeout");

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