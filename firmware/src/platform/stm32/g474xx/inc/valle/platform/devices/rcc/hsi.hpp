#pragma once

#include "stm32g4xx_ll_rcc.h"
#include "valle/platform/core.hpp"
#include "valle/platform/hardware/rcc/hsi.hpp"
#include "valle/platform/hardware/rcc/pll.hpp"
#include "valle/platform/hardware/rcc/sct.hpp"

namespace valle::platform
{

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
    class HsiOscillator
    {
    public:
        struct Descriptor : public UniqueDeviceDescriptor
        {
        };

        using HdiT          = HsiOscillatorHdi<T>;
        using SctInfoHdiT   = SctInfoHdi<T>;
        using PllInfoHdiT   = PllInfoHdi<T>;
        using InjectDevices = TypeList<HdiT, SctInfoHdiT, PllInfoHdiT>;

    private:
        [[no_unique_address]] DeviceRef<HdiT>        m_hw;
        [[no_unique_address]] DeviceRef<SctInfoHdiT> m_sct_info_hw;
        [[no_unique_address]] DeviceRef<PllInfoHdiT> m_pll_info_hw;

    public:
        explicit HsiOscillator(DeviceRef<HdiT>&&        hardware_key,
                               DeviceRef<SctInfoHdiT>&& sct_info_hw,
                               DeviceRef<PllInfoHdiT>&& pll_info_hw)
            : m_hw(std::move(hardware_key))
            , m_sct_info_hw(std::move(sct_info_hw))
            , m_pll_info_hw(std::move(pll_info_hw))
        {
        }

        // --- Initialization ---
        /**
         * @brief Initialize HSI with given configuration.
         * @param config HSI configuration.
         */
        [[nodiscard]] inline bool init(const HsiOscillatorConfig& config)
        {
            if (config.enabled)
            {
                m_hw->enable();

                expect(wait_for_ready(HdiT::skDefaultEnableTimeoutCount), "HSI failed to become ready within timeout");

                m_hw->set_calibration_trimming(config.trimming);

                return true;
            }

            const auto sysclk_status = m_sct_info_hw->get_source_status();
            const auto pll_source    = m_pll_info_hw->get_source();

            const bool hsi_used_as_sysclk = (sysclk_status == SctSourceStatus::kHSI);
            const bool hsi_used_by_pll_sysclk =
                (sysclk_status == SctSourceStatus::kPLL) && (pll_source == PllSource::kHSI);

            expect(!(hsi_used_as_sysclk || hsi_used_by_pll_sysclk),
                   "Cannot disable HSI while it is used by SYSCLK or active PLL system clock path");

            m_hw->disable();

            expect(wait_for_not_ready(HdiT::skDefaultDisableTimeoutCount), "HSI failed to disable within timeout");

            return true;
        }

    private:
        [[nodiscard]] static bool wait_for_ready(const uint32_t timeout_count)
        {
            return TimingContext::wait_for_with_timeout_countdown([]() { return m_hw->is_ready(); }, timeout_count);
        }

        [[nodiscard]] static bool wait_for_not_ready(const uint32_t timeout_count)
        {
            return TimingContext::wait_for_with_timeout_countdown([]() -> bool { return !m_hw->is_ready(); },
                                                                  timeout_count);
        }
    };

}  // namespace valle::platform