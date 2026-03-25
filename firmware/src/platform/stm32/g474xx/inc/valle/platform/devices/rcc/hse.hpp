#pragma once

#include "valle/platform/core.hpp"
#include "valle/platform/hardware/rcc/hse.hpp"
#include "valle/platform/hardware/rcc/pll.hpp"
#include "valle/platform/hardware/rcc/sct.hpp"

namespace valle::platform
{
    // =============================================================================
    // HSE OSCILLATOR INFO DEVICE
    // =============================================================================
    template <typename T = void>
    class HseOscillatorInfoDevice
    {
    public:
        struct Descriptor : public SharedDeviceDescriptor
        {
            constexpr static bool skNeedsInit = false;
        };

        using InterfaceT = HseOscillatorInterface;

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
    // HSE OSCILLATOR DEVICE
    // =========================================================================

    // -----------------------------------------------------------------------------
    // CONFIGURATION
    // -----------------------------------------------------------------------------
    struct HseOscillatorConfig
    {
        bool              enabled = true;
        HseOscillatorMode mode    = HseOscillatorMode::kCrystal;

        [[nodiscard]] constexpr uint32_t get_frequency_hz() const
        {
            return enabled ? HseOscillatorInterface::skFrequencyHz : 0U;
        }
    };

    // -----------------------------------------------------------------------------
    // DEVICE
    // -----------------------------------------------------------------------------
    template <typename T = void>
    class HseOscillatorDevice
    {
    public:
        struct Descriptor : public UniqueDeviceDescriptor
        {
        };

        using InterfaceT = HseOscillatorInterface;

        using InjectDevices = TypeList<>;

        [[nodiscard]] inline bool init(const HseOscillatorConfig& config)
        {
            if (config.enabled)
            {
                InterfaceT::enable(config.mode);

                expect(wait_for_ready(InterfaceT::skDefaultEnableTimeoutCount),
                       "HSE failed to become ready within timeout");

                return true;
            }

            const auto sysclk_status = SctInterface::get_source_status();
            const auto pll_source    = PllInterface::get_source();

            const bool hse_used_as_sysclk = (sysclk_status == SctSourceStatus::kHSE);
            const bool hse_used_by_pll_sysclk =
                (sysclk_status == SctSourceStatus::kPLL) && (pll_source == PllSource::kHSE);

            expect(!(hse_used_as_sysclk || hse_used_by_pll_sysclk),
                   "Cannot disable HSE while it is used by SYSCLK or active PLL system clock path");

            InterfaceT::disable();

            expect(wait_for_not_ready(InterfaceT::skDefaultDisableTimeoutCount),
                   "HSE failed to disable within timeout");

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