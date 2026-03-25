#pragma once

#include "stm32g4xx_ll_rcc.h"
#include "valle/platform/core.hpp"
#include "valle/platform/hardware/rcc/hsi.hpp"
#include "valle/platform/hardware/rcc/pll.hpp"
#include "valle/platform/hardware/rcc/sct.hpp"

namespace valle::platform
{
    // =========================================================================
    // HSI OSCILLATOR INFO DEVICE
    // =========================================================================
    template <typename T = void>
    class HsiOscillatorInfoDevice
    {
    public:
        struct Descriptor : public SharedDeviceDescriptor
        {
            constexpr static bool skNeedsInit = false;
        };

        using InterfaceT = HsiOscillatorInterface;

        using InjectDevices = TypeList<>;

        [[nodiscard]] bool is_ready() const
        {
            return InterfaceT::is_ready();
        }

        [[nodiscard]] uint32_t get_calibration() const
        {
            return InterfaceT::get_calibration();
        }

        [[nodiscard]] constexpr uint32_t get_frequency_hz() const
        {
            return InterfaceT::skFrequencyHz;
        }
    };

    // =============================================================================
    // HSI OSCILLATOR DEVICE
    // =============================================================================

    // -----------------------------------------------------------------------------
    // CONFIGURATIONS
    // -----------------------------------------------------------------------------
    static constexpr auto kDefaultHsiCalibrationTrim = HsiOscillatorInterface::skDefaultCalibrationTrim;

    struct HsiOscillatorConfig
    {
        bool     enabled  = true;
        uint32_t trimming = kDefaultHsiCalibrationTrim;

        [[nodiscard]] constexpr uint32_t get_frequency_hz() const
        {
            return enabled ? HsiOscillatorInterface::skFrequencyHz : 0U;
        }
    };

    // -----------------------------------------------------------------------------
    // DEVICE CLASS
    // -----------------------------------------------------------------------------
    template <typename T = void>
    class HsiOscillatorDevice
    {
    public:
        struct Descriptor : public UniqueDeviceDescriptor
        {
        };

        using InterfaceT = HsiOscillatorInterface;

        using InjectDevices = TypeList<>;

        // --- Initialization ---
        /**
         * @brief Initialize HSI with given configuration.
         * @param config HSI configuration.
         */
        [[nodiscard]] inline bool init(const HsiOscillatorConfig& config)
        {
            if (config.enabled)
            {
                InterfaceT::enable();

                expect(wait_for_ready(InterfaceT::skDefaultEnableTimeoutCount),
                       "HSI failed to become ready within timeout");

                InterfaceT::set_calibration_trimming(config.trimming);

                return true;
            }

            const auto sysclk_status = SctInterface::get_source_status();
            const auto pll_source    = PllInterface::get_source();

            const bool hsi_used_as_sysclk = (sysclk_status == SctSourceStatus::kHSI);
            const bool hsi_used_by_pll_sysclk =
                (sysclk_status == SctSourceStatus::kPLL) && (pll_source == PllSource::kHSI);

            expect(!(hsi_used_as_sysclk || hsi_used_by_pll_sysclk),
                   "Cannot disable HSI while it is used by SYSCLK or active PLL system clock path");

            InterfaceT::disable();

            expect(wait_for_not_ready(InterfaceT::skDefaultDisableTimeoutCount),
                   "HSI failed to disable within timeout");

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