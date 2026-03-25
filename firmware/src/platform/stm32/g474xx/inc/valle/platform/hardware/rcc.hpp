#pragma once

#include "valle/platform/hardware/rcc/hse.hpp"
#include "valle/platform/hardware/rcc/hsi.hpp"
#include "valle/platform/hardware/rcc/hsi48.hpp"
#include "valle/platform/hardware/rcc/lse.hpp"
#include "valle/platform/hardware/rcc/lsi.hpp"
#include "valle/platform/hardware/rcc/pll.hpp"
#include "valle/platform/hardware/rcc/sct.hpp"

namespace valle::platform
{
    struct RccInterface
    {
        // =====================================================================
        // OSCILLATOR FREQUENCIES
        // =====================================================================

        [[nodiscard]] static constexpr uint32_t get_hsi_freq_hz()
        {
            return HsiOscillatorInterface::skFrequencyHz;
        }
        [[nodiscard]] static constexpr uint32_t get_hse_freq_hz()
        {
            return HseOscillatorInterface::skFrequencyHz;
        }
        [[nodiscard]] static constexpr uint32_t get_hsi48_freq_hz()
        {
            return Hsi48OscillatorInterface::skFrequencyHz;
        }
        [[nodiscard]] static constexpr uint32_t get_lsi_freq_hz()
        {
            return LsiOscillatorInterface::skFrequencyHz;
        }
        [[nodiscard]] static constexpr uint32_t get_lse_freq_hz()
        {
            return LseOscillatorInterface::skFrequencyHz;
        }

        // =====================================================================
        // PLL FREQUENCIES
        // =====================================================================

        [[nodiscard]] static uint32_t get_pll_source_freq_hz()
        {
            switch (PllInterface::get_source())
            {
                case PllSource::kHSI:
                    return get_hsi_freq_hz();
                case PllSource::kHSE:
                    return get_hse_freq_hz();
                default:
                    return 0U;
            }
        }

        [[nodiscard]] static uint32_t get_pll_vco_freq_hz()
        {
            return PllInterface::calculate_vco_freq_hz(
                get_pll_source_freq_hz(), PllInterface::get_m_divider(), PllInterface::get_n_multiplier());
        }

        [[nodiscard]] static uint32_t get_pll_r_freq_hz()
        {
            if (!PllInterface::is_r_output_enabled()) return 0U;
            return PllInterface::calculate_r_output_freq_hz(get_pll_vco_freq_hz(), PllInterface::get_r_divider());
        }

        [[nodiscard]] static uint32_t get_pll_p_freq_hz()
        {
            if (!PllInterface::is_p_output_enabled()) return 0U;
            return PllInterface::calculate_p_output_freq_hz(get_pll_vco_freq_hz(), PllInterface::get_p_divider());
        }

        [[nodiscard]] static uint32_t get_pll_q_freq_hz()
        {
            if (!PllInterface::is_q_output_enabled()) return 0U;
            return PllInterface::calculate_q_output_freq_hz(get_pll_vco_freq_hz(), PllInterface::get_q_divider());
        }

        // =====================================================================
        // SYSTEM TREE FREQUENCIES (SCT)
        // =====================================================================

        [[nodiscard]] static uint32_t get_sysclk_freq_hz()
        {
            switch (SctInterface::get_source_status())
            {
                case SctSourceStatus::kHSI:
                    return get_hsi_freq_hz();
                case SctSourceStatus::kHSE:
                    return get_hse_freq_hz();
                case SctSourceStatus::kPLL:
                    return get_pll_r_freq_hz();
                default:
                    return 0U;
            }
        }

        [[nodiscard]] static uint32_t get_hclk_freq_hz()
        {
            return SctInterface::calculate_hclk_freq_hz(get_sysclk_freq_hz(), SctInterface::get_ahb_prescaler());
        }

        [[nodiscard]] static uint32_t get_pclk1_freq_hz()
        {
            return SctInterface::calculate_pclk1_freq_hz(get_hclk_freq_hz(), SctInterface::get_apb1_prescaler());
        }

        [[nodiscard]] static uint32_t get_pclk2_freq_hz()
        {
            return SctInterface::calculate_pclk2_freq_hz(get_hclk_freq_hz(), SctInterface::get_apb2_prescaler());
        }
    };
}  // namespace valle::platform