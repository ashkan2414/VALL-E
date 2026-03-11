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
    struct RCCInterface
    {
        // =====================================================================
        // OSCILLATOR FREQUENCIES
        // =====================================================================

        [[nodiscard]] static constexpr uint32_t get_hsi_freq_hz()
        {
            return HSIOscillatorInterface::skFrequencyHz;
        }
        [[nodiscard]] static constexpr uint32_t get_hse_freq_hz()
        {
            return HSEOscillatorInterface::skFrequencyHz;
        }
        [[nodiscard]] static constexpr uint32_t get_hsi48_freq_hz()
        {
            return HSI48OscillatorInterface::skFrequencyHz;
        }
        [[nodiscard]] static constexpr uint32_t get_lsi_freq_hz()
        {
            return LSIOscillatorInterface::skFrequencyHz;
        }
        [[nodiscard]] static constexpr uint32_t get_lse_freq_hz()
        {
            return LSEOscillatorInterface::skFrequencyHz;
        }

        // =====================================================================
        // PLL FREQUENCIES
        // =====================================================================

        [[nodiscard]] static uint32_t get_pll_source_freq_hz()
        {
            switch (PLLInterface::get_source())
            {
                case PLLSource::kHSI:
                    return get_hsi_freq_hz();
                case PLLSource::kHSE:
                    return get_hse_freq_hz();
                default:
                    return 0U;
            }
        }

        [[nodiscard]] static uint32_t get_pll_vco_freq_hz()
        {
            return PLLInterface::calculate_vco_freq_hz(
                get_pll_source_freq_hz(), PLLInterface::get_m_divider(), PLLInterface::get_n_multiplier());
        }

        [[nodiscard]] static uint32_t get_pll_r_freq_hz()
        {
            if (!PLLInterface::is_r_output_enabled()) return 0U;
            return PLLInterface::calculate_r_output_freq_hz(get_pll_vco_freq_hz(), PLLInterface::get_r_divider());
        }

        [[nodiscard]] static uint32_t get_pll_p_freq_hz()
        {
            if (!PLLInterface::is_p_output_enabled()) return 0U;
            return PLLInterface::calculate_p_output_freq_hz(get_pll_vco_freq_hz(), PLLInterface::get_p_divider());
        }

        [[nodiscard]] static uint32_t get_pll_q_freq_hz()
        {
            if (!PLLInterface::is_q_output_enabled()) return 0U;
            return PLLInterface::calculate_q_output_freq_hz(get_pll_vco_freq_hz(), PLLInterface::get_q_divider());
        }

        // =====================================================================
        // SYSTEM TREE FREQUENCIES (SCT)
        // =====================================================================

        [[nodiscard]] static uint32_t get_sysclk_freq_hz()
        {
            switch (SCTInterface::get_source_status())
            {
                case SCTSourceStatus::kHSI:
                    return get_hsi_freq_hz();
                case SCTSourceStatus::kHSE:
                    return get_hse_freq_hz();
                case SCTSourceStatus::kPLL:
                    return get_pll_r_freq_hz();
                default:
                    return 0U;
            }
        }

        [[nodiscard]] static uint32_t get_hclk_freq_hz()
        {
            return SCTInterface::calculate_hclk_freq_hz(get_sysclk_freq_hz(), SCTInterface::get_ahb_prescaler());
        }

        [[nodiscard]] static uint32_t get_pclk1_freq_hz()
        {
            return SCTInterface::calculate_pclk1_freq_hz(get_hclk_freq_hz(), SCTInterface::get_apb1_prescaler());
        }

        [[nodiscard]] static uint32_t get_pclk2_freq_hz()
        {
            return SCTInterface::calculate_pclk2_freq_hz(get_hclk_freq_hz(), SCTInterface::get_apb2_prescaler());
        }
    };
}  // namespace valle::platform