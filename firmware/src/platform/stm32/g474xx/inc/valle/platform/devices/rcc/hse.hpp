#pragma once

#include "valle/platform/core.hpp"
#include "valle/platform/hardware/rcc/hse.hpp"
#include "valle/platform/hardware/rcc/pll.hpp"
#include "valle/platform/hardware/rcc/sct.hpp"

namespace valle::platform
{

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
    class HseOscillator
    {
    public:
        struct Descriptor : public UniqueDeviceDescriptor
        {
        };

        using HdiT          = HseOscillatorHdi<T>;
        using SctInfoHdiT   = SctInfoHdi<T>;
        using PllInfoHdiT   = PllInfoHdi<T>;
        using InjectDevices = TypeList<HdiT, SctInfoHdiT, PllInfoHdiT>;

    private:
        [[no_unique_address]] DeviceRef<HdiT>        m_hw;
        [[no_unique_address]] DeviceRef<SctInfoHdiT> m_sct_info_hw;
        [[no_unique_address]] DeviceRef<PllInfoHdiT> m_pll_info_hw;

    public:
        explicit HseOscillator(DeviceRef<HdiT>&&        hardware_key,
                               DeviceRef<SctInfoHdiT>&& sct_info_hw,
                               DeviceRef<PllInfoHdiT>&& pll_info_hw)
            : m_hw(std::move(hardware_key))
            , m_sct_info_hw(std::move(sct_info_hw))
            , m_pll_info_hw(std::move(pll_info_hw))
        {
        }

        [[nodiscard]] inline bool init(const HseOscillatorConfig& config)
        {
            if (config.enabled)
            {
                m_hw->enable(config.mode);

                expect(wait_for_ready(HdiT::DefaultEnableTimeoutCount), "HSE failed to become ready within timeout");

                return true;
            }

            const auto sysclk_status = m_sct_info_hw->get_source_status();
            const auto pll_source    = m_pll_info_hw->get_source();

            const bool hse_used_as_sysclk = (sysclk_status == SctSourceStatus::kHSE);
            const bool hse_used_by_pll_sysclk =
                (sysclk_status == SctSourceStatus::kPLL) && (pll_source == PllSource::kHSE);

            expect(!(hse_used_as_sysclk || hse_used_by_pll_sysclk),
                   "Cannot disable HSE while it is used by SYSCLK or active PLL system clock path");

            m_hw->disable();

            expect(wait_for_not_ready(HdiT::DefaultDisableTimeoutCount), "HSE failed to disable within timeout");

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