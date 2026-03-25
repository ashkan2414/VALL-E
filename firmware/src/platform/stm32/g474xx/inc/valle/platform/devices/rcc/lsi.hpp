#pragma once

#include "valle/platform/core.hpp"
#include "valle/platform/hardware/rcc/lsi.hpp"

namespace valle::platform
{
    // =============================================================================
    // CONFIGURATION
    // =============================================================================
    struct LSIOscillatorConfig
    {
        bool enabled = true;

        [[nodiscard]] constexpr uint32_t get_frequency_hz() const
        {
            return enabled ? LSIOscillatorInterface::skFrequencyHz : 0U;
        }
    };

    // =========================================================================
    // LSI Oscillator INFO DEVICE
    // =========================================================================
    template <typename T = void>
    class LSIOscillatorInfoDevice
    {
    public:
        struct Descriptor : public SharedDeviceDescriptor
        {
            constexpr static bool skNeedsInit = false;
        };

        using InterfaceT = LSIOscillatorInterface;

        using InjectDevices = TypeList<>;

        [[nodiscard]] static bool is_ready()
        {
            return InterfaceT::is_ready();
        }

        [[nodiscard]] constexpr uint32_t get_frequency_hz() const
        {
            return InterfaceT::skFrequencyHz;
        }
    };

    // =============================================================================
    // DEVICE
    // =============================================================================
    template <typename T = void>
    class LSIOscillatorDevice
    {
    public:
        struct Descriptor : public UniqueDeviceDescriptor
        {
        };

        using InterfaceT = LSIOscillatorInterface;

        using InjectDevices = TypeList<>;

        [[nodiscard]] inline bool init(const LSIOscillatorConfig& config)
        {
            if (config.enabled)
            {
                InterfaceT::enable();

                expect(wait_for_ready(InterfaceT::skDefaultEnableTimeoutCount),
                       "LSI failed to become ready within timeout");

                return true;
            }

            InterfaceT::disable();

            expect(wait_for_not_ready(InterfaceT::skDefaultDisableTimeoutCount),
                   "LSI failed to disable within timeout");

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