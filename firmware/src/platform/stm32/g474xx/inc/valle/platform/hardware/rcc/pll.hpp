#pragma once

#include <cstdint>
#include <optional>
#include <string_view>

#include "stm32g4xx_ll_rcc.h"
#include "valle/base/panic.hpp"
#include "valle/platform/hardware/power.hpp"

namespace valle::platform
{
    // ============================================================================
    // ENUMERATIONS
    // ============================================================================

    enum class PllSource
    {
        kNone = LL_RCC_PLLSOURCE_NONE,
        kHSI  = LL_RCC_PLLSOURCE_HSI,
        kHSE  = LL_RCC_PLLSOURCE_HSE
    };

    enum class PllMDivider : uint32_t
    {
        kDiv1  = LL_RCC_PLLM_DIV_1,
        kDiv2  = LL_RCC_PLLM_DIV_2,
        kDiv3  = LL_RCC_PLLM_DIV_3,
        kDiv4  = LL_RCC_PLLM_DIV_4,
        kDiv5  = LL_RCC_PLLM_DIV_5,
        kDiv6  = LL_RCC_PLLM_DIV_6,
        kDiv7  = LL_RCC_PLLM_DIV_7,
        kDiv8  = LL_RCC_PLLM_DIV_8,
        kDiv9  = LL_RCC_PLLM_DIV_9,
        kDiv10 = LL_RCC_PLLM_DIV_10,
        kDiv11 = LL_RCC_PLLM_DIV_11,
        kDiv12 = LL_RCC_PLLM_DIV_12,
        kDiv13 = LL_RCC_PLLM_DIV_13,
        kDiv14 = LL_RCC_PLLM_DIV_14,
        kDiv15 = LL_RCC_PLLM_DIV_15,
        kDiv16 = LL_RCC_PLLM_DIV_16
    };

    enum class PllPDivider : uint32_t
    {
        kDiv2  = LL_RCC_PLLP_DIV_2,
        kDiv3  = LL_RCC_PLLP_DIV_3,
        kDiv4  = LL_RCC_PLLP_DIV_4,
        kDiv5  = LL_RCC_PLLP_DIV_5,
        kDiv6  = LL_RCC_PLLP_DIV_6,
        kDiv7  = LL_RCC_PLLP_DIV_7,
        kDiv8  = LL_RCC_PLLP_DIV_8,
        kDiv9  = LL_RCC_PLLP_DIV_9,
        kDiv10 = LL_RCC_PLLP_DIV_10,
        kDiv11 = LL_RCC_PLLP_DIV_11,
        kDiv12 = LL_RCC_PLLP_DIV_12,
        kDiv13 = LL_RCC_PLLP_DIV_13,
        kDiv14 = LL_RCC_PLLP_DIV_14,
        kDiv15 = LL_RCC_PLLP_DIV_15,
        kDiv16 = LL_RCC_PLLP_DIV_16,
        kDiv17 = LL_RCC_PLLP_DIV_17,
        kDiv18 = LL_RCC_PLLP_DIV_18,
        kDiv19 = LL_RCC_PLLP_DIV_19,
        kDiv20 = LL_RCC_PLLP_DIV_20,
        kDiv21 = LL_RCC_PLLP_DIV_21,
        kDiv22 = LL_RCC_PLLP_DIV_22,
        kDiv23 = LL_RCC_PLLP_DIV_23,
        kDiv24 = LL_RCC_PLLP_DIV_24,
        kDiv25 = LL_RCC_PLLP_DIV_25,
        kDiv26 = LL_RCC_PLLP_DIV_26,
        kDiv27 = LL_RCC_PLLP_DIV_27,
        kDiv28 = LL_RCC_PLLP_DIV_28,
        kDiv29 = LL_RCC_PLLP_DIV_29,
        kDiv30 = LL_RCC_PLLP_DIV_30,
        kDiv31 = LL_RCC_PLLP_DIV_31,
    };

    enum class PllQDivider : uint32_t
    {
        kDiv2 = LL_RCC_PLLQ_DIV_2,
        kDiv4 = LL_RCC_PLLQ_DIV_4,
        kDiv6 = LL_RCC_PLLQ_DIV_6,
        kDiv8 = LL_RCC_PLLQ_DIV_8
    };

    enum class PllRDivider : uint32_t
    {
        kDiv2 = LL_RCC_PLLR_DIV_2,
        kDiv4 = LL_RCC_PLLR_DIV_4,
        kDiv6 = LL_RCC_PLLR_DIV_6,
        kDiv8 = LL_RCC_PLLR_DIV_8
    };

    // ============================================================================
    // INTERFACE
    // ============================================================================

    struct PllFrequencyLimits
    {
        uint32_t min_input_hz  = 0U;
        uint32_t max_input_hz  = 0U;
        uint32_t min_vco_hz    = 0U;
        uint32_t max_vco_hz    = 0U;
        uint32_t min_output_hz = 0U;
        uint32_t max_output_hz = 0U;
    };

    struct PllInterface
    {
        static constexpr uint32_t skDefaultEnableTimeoutCount  = 1'000'000U;
        static constexpr uint32_t skDefaultDisableTimeoutCount = 1'000'000U;

        static constexpr uint32_t skMinNMultiplier = 8;
        static constexpr uint32_t skMaxNMultiplier = 127;

        static constexpr PllFrequencyLimits skRange1BoostFrequencyLimits = {.min_input_hz  = 2'660'000,
                                                                            .max_input_hz  = 8'000'000,
                                                                            .min_vco_hz    = 96'000'000,
                                                                            .max_vco_hz    = 344'000'000,
                                                                            .min_output_hz = 8'000'000,
                                                                            .max_output_hz = 170'000'000};

        static constexpr PllFrequencyLimits skRange1NormalFrequencyLimits = {.min_input_hz  = 2'660'000,
                                                                             .max_input_hz  = 8'000'000,
                                                                             .min_vco_hz    = 96'000'000,
                                                                             .max_vco_hz    = 344'000'000,
                                                                             .min_output_hz = 8'000'000,
                                                                             .max_output_hz = 150'000'000};

        static constexpr PllFrequencyLimits skPllRange2FrequencyLimits = {.min_input_hz  = 2'660'000,
                                                                          .max_input_hz  = 8'000'000,
                                                                          .min_vco_hz    = 96'000'000,
                                                                          .max_vco_hz    = 128'000'000,
                                                                          .min_output_hz = 8'000'000,
                                                                          .max_output_hz = 26'000'000};

        // ---------------------------------------------------------------------
        // Control
        // ---------------------------------------------------------------------
        static void enable()
        {
            LL_RCC_PLL_Enable();
        }

        static void disable()
        {
            LL_RCC_PLL_Disable();
        }

        [[nodiscard]] static bool is_ready()
        {
            return LL_RCC_PLL_IsReady() == 1;
        }

        static void set_source(const PllSource source)
        {
            LL_RCC_PLL_SetMainSource(static_cast<uint32_t>(source));
        }

        [[nodiscard]] static PllSource get_source()
        {
            return static_cast<PllSource>(LL_RCC_PLL_GetMainSource());
        }

        static void config_domains(const PllSource                  source,
                                   const PllMDivider                m_divider,
                                   const uint32_t                   n_multiplier,
                                   const std::optional<PllRDivider> r_divider,
                                   const std::optional<PllPDivider> p_divider,
                                   const std::optional<PllQDivider> q_divider)
        {
            expect(n_multiplier >= skMinNMultiplier && n_multiplier <= skMaxNMultiplier,
                   "PLL N multiplier must be between 8 and 127");
            expect(r_divider.has_value() || p_divider.has_value() || q_divider.has_value(),
                   "At least one PLL output (P, Q, or R) must be enabled");
            expect(!is_ready(), "PLL must be disabled before reconfiguration");

            if (r_divider.has_value())
            {
                LL_RCC_PLL_ConfigDomain_SYS(static_cast<uint32_t>(source),
                                            static_cast<uint32_t>(m_divider),
                                            n_multiplier,
                                            static_cast<uint32_t>(r_divider.value()));
            }

            if (p_divider.has_value())
            {
                LL_RCC_PLL_ConfigDomain_ADC(static_cast<uint32_t>(source),
                                            static_cast<uint32_t>(m_divider),
                                            n_multiplier,
                                            static_cast<uint32_t>(p_divider.value()));
            }

            if (q_divider.has_value())
            {
                LL_RCC_PLL_ConfigDomain_48M(static_cast<uint32_t>(source),
                                            static_cast<uint32_t>(m_divider),
                                            n_multiplier,
                                            static_cast<uint32_t>(q_divider.value()));
            }
        }

        [[nodiscard]] static PllMDivider get_m_divider()
        {
            return static_cast<PllMDivider>(LL_RCC_PLL_GetDivider());
        }

        [[nodiscard]] static uint32_t get_n_multiplier()
        {
            return LL_RCC_PLL_GetN();
        }

        [[nodiscard]] static PllPDivider get_p_divider()
        {
            return static_cast<PllPDivider>(LL_RCC_PLL_GetP());
        }

        [[nodiscard]] static PllQDivider get_q_divider()
        {
            return static_cast<PllQDivider>(LL_RCC_PLL_GetQ());
        }

        [[nodiscard]] static PllRDivider get_r_divider()
        {
            return static_cast<PllRDivider>(LL_RCC_PLL_GetR());
        }

        static void enable_p_output()
        {
            LL_RCC_PLL_EnableDomain_ADC();
        }

        static void disable_p_output()
        {
            LL_RCC_PLL_DisableDomain_ADC();
        }

        [[nodiscard]] static bool is_p_output_enabled()
        {
            return LL_RCC_PLL_IsEnabledDomain_ADC() == 1;
        }

        static void enable_q_output()
        {
            LL_RCC_PLL_EnableDomain_48M();
        }

        static void disable_q_output()
        {
            LL_RCC_PLL_DisableDomain_48M();
        }

        [[nodiscard]] static bool is_q_output_enabled()
        {
            return LL_RCC_PLL_IsEnabledDomain_48M() == 1;
        }

        static void enable_r_output()
        {
            LL_RCC_PLL_EnableDomain_SYS();
        }

        static void disable_r_output()
        {
            LL_RCC_PLL_DisableDomain_SYS();
        }

        [[nodiscard]] static bool is_r_output_enabled()
        {
            return LL_RCC_PLL_IsEnabledDomain_SYS() == 1;
        }

        // ---------------------------------------------------------------------
        // Enum to Value Helpers
        // ---------------------------------------------------------------------
        [[nodiscard]] static constexpr uint32_t get_m_divider_factor(const PllMDivider divider)
        {
            // NOLINTBEGIN(readability-magic-numbers)
            switch (divider)
            {
                case PllMDivider::kDiv1:
                    return 1;
                case PllMDivider::kDiv2:
                    return 2;
                case PllMDivider::kDiv3:
                    return 3;
                case PllMDivider::kDiv4:
                    return 4;
                case PllMDivider::kDiv5:
                    return 5;
                case PllMDivider::kDiv6:
                    return 6;
                case PllMDivider::kDiv7:
                    return 7;
                case PllMDivider::kDiv8:
                    return 8;
                case PllMDivider::kDiv9:
                    return 9;
                case PllMDivider::kDiv10:
                    return 10;
                case PllMDivider::kDiv11:
                    return 11;
                case PllMDivider::kDiv12:
                    return 12;
                case PllMDivider::kDiv13:
                    return 13;
                case PllMDivider::kDiv14:
                    return 14;
                case PllMDivider::kDiv15:
                    return 15;
                case PllMDivider::kDiv16:
                    return 16;
                default:
                    return 0;
            }
            // NOLINTEND(readability-magic-numbers)
        }

        [[nodiscard]] static constexpr uint32_t get_r_divider_factor(const PllRDivider divider)
        {
            // NOLINTBEGIN(readability-magic-numbers)
            switch (divider)
            {
                case PllRDivider::kDiv2:
                    return 2;
                case PllRDivider::kDiv4:
                    return 4;
                case PllRDivider::kDiv6:
                    return 6;
                case PllRDivider::kDiv8:
                    return 8;
                default:
                    return 0;
            }

            // NOLINTEND(readability-magic-numbers)
        }

        [[nodiscard]] static constexpr uint32_t get_p_divider_factor(const PllPDivider divider)
        {
            // NOLINTBEGIN(readability-magic-numbers)
            switch (divider)
            {
                case PllPDivider::kDiv2:
                    return 2;
                case PllPDivider::kDiv3:
                    return 3;
                case PllPDivider::kDiv4:
                    return 4;
                case PllPDivider::kDiv5:
                    return 5;
                case PllPDivider::kDiv6:
                    return 6;
                case PllPDivider::kDiv7:
                    return 7;
                case PllPDivider::kDiv8:
                    return 8;
                case PllPDivider::kDiv9:
                    return 9;
                case PllPDivider::kDiv10:
                    return 10;
                case PllPDivider::kDiv11:
                    return 11;
                case PllPDivider::kDiv12:
                    return 12;
                case PllPDivider::kDiv13:
                    return 13;
                case PllPDivider::kDiv14:
                    return 14;
                case PllPDivider::kDiv15:
                    return 15;
                case PllPDivider::kDiv16:
                    return 16;
                case PllPDivider::kDiv17:
                    return 17;
                case PllPDivider::kDiv18:
                    return 18;
                case PllPDivider::kDiv19:
                    return 19;
                case PllPDivider::kDiv20:
                    return 20;
                case PllPDivider::kDiv21:
                    return 21;
                case PllPDivider::kDiv22:
                    return 22;
                case PllPDivider::kDiv23:
                    return 23;
                case PllPDivider::kDiv24:
                    return 24;
                case PllPDivider::kDiv25:
                    return 25;
                case PllPDivider::kDiv26:
                    return 26;
                case PllPDivider::kDiv27:
                    return 27;
                case PllPDivider::kDiv28:
                    return 28;
                case PllPDivider::kDiv29:
                    return 29;
                case PllPDivider::kDiv30:
                    return 30;
                case PllPDivider::kDiv31:
                    return 31;
                default:
                    return 0;
            }
            // NOLINTEND(readability-magic-numbers)
        }

        [[nodiscard]] static constexpr uint32_t get_q_divider_factor(const PllQDivider divider)
        {
            // NOLINTBEGIN(readability-magic-numbers)
            switch (divider)
            {
                case PllQDivider::kDiv2:
                    return 2;
                case PllQDivider::kDiv4:
                    return 4;
                case PllQDivider::kDiv6:
                    return 6;
                case PllQDivider::kDiv8:
                    return 8;
                default:
                    return 0;
            }
            // NOLINTEND(readability-magic-numbers)
        }

        [[nodiscard]] static constexpr auto get_frequency_limits(const PowerVoltageRangeMode voltage_range)
        {
            switch (voltage_range)
            {
                case PowerVoltageRangeMode::kRange1Boost:
                    return skRange1BoostFrequencyLimits;
                case PowerVoltageRangeMode::kRange1Normal:
                    return skRange1NormalFrequencyLimits;
                case PowerVoltageRangeMode::kRange2:
                    return skPllRange2FrequencyLimits;
                default:
                    return PllFrequencyLimits{};
            }
        }

        // ---------------------------------------------------------------------
        // Calculation Helpers
        // ---------------------------------------------------------------------

        [[nodiscard]] static constexpr uint32_t calculate_input_freq_hz(const uint32_t    source_hz,
                                                                        const PllMDivider m_divider)
        {
            const uint32_t m_factor = get_m_divider_factor(m_divider);
            return (m_factor == 0) ? 0 : (source_hz / m_factor);
        }

        [[nodiscard]] static constexpr uint32_t calculate_vco_freq_hz(const uint32_t    source_hz,
                                                                      const PllMDivider m_divider,
                                                                      const uint32_t    n_multiplier)
        {
            const uint32_t m_factor = get_m_divider_factor(m_divider);
            return (m_factor == 0) ? 0 : ((source_hz * n_multiplier) / m_factor);
        }

        [[nodiscard]] static constexpr uint32_t calculate_r_output_freq_hz(const uint32_t    vco_hz,
                                                                           const PllRDivider r_divider)
        {
            return vco_hz / get_r_divider_factor(r_divider);
        }

        [[nodiscard]] static constexpr uint32_t calculate_p_output_freq_hz(const uint32_t    vco_hz,
                                                                           const PllPDivider p_divider)
        {
            return vco_hz / get_p_divider_factor(p_divider);
        }

        [[nodiscard]] static constexpr uint32_t calculate_q_output_freq_hz(const uint32_t    vco_hz,
                                                                           const PllQDivider q_divider)
        {
            return vco_hz / get_q_divider_factor(q_divider);
        }

        // ---------------------------------------------------------------------
        // Validation
        // ---------------------------------------------------------------------
        static constexpr std::optional<std::string_view> validate_config(const uint32_t                   source_hz,
                                                                         const PowerVoltageRangeMode      voltage_range,
                                                                         const PllMDivider                m_divider,
                                                                         const uint32_t                   n_multiplier,
                                                                         const std::optional<PllRDivider> r_divider,
                                                                         const std::optional<PllPDivider> p_divider,
                                                                         const std::optional<PllQDivider> q_divider)
        {
            if (source_hz == 0)
            {
                return "PLL source frequency must be greater than zero";
            }

            const uint32_t m_factor = get_m_divider_factor(m_divider);
            if (m_factor == 0)
            {
                return "PLL M divider must not be zero";
            }

            if (n_multiplier < skMinNMultiplier || n_multiplier > skMaxNMultiplier)
            {
                return "PLL N multiplier must be between 8 and 127";
            }

            if (!p_divider.has_value() && !q_divider.has_value() && !r_divider.has_value())
            {
                return "At least one PLL output (P, Q, or R) must be enabled";
            }

            const auto frequency_limits = get_frequency_limits(voltage_range);

            const uint32_t input_hz = calculate_input_freq_hz(source_hz, m_divider);
            if (input_hz < frequency_limits.min_input_hz || input_hz > frequency_limits.max_input_hz)
            {
                return "PLL input frequency is out of range";
            }

            const uint32_t vco_hz = calculate_vco_freq_hz(source_hz, m_divider, n_multiplier);
            if (vco_hz < frequency_limits.min_vco_hz || vco_hz > frequency_limits.max_vco_hz)
            {
                return "PLL VCO frequency is out of range";
            }

            if (r_divider.has_value())
            {
                const uint32_t r_hz = calculate_r_output_freq_hz(vco_hz, r_divider.value());
                if (r_hz < frequency_limits.min_output_hz || r_hz > frequency_limits.max_output_hz)
                {
                    return "PLL R output frequency is out of range";
                }
            }

            if (p_divider.has_value())
            {
                const uint32_t p_hz = calculate_p_output_freq_hz(vco_hz, p_divider.value());
                if (p_hz < frequency_limits.min_output_hz || p_hz > frequency_limits.max_output_hz)
                {
                    return "PLL P output frequency is out of range";
                }
            }

            if (q_divider.has_value())
            {
                const uint32_t q_hz = calculate_q_output_freq_hz(vco_hz, q_divider.value());
                if (q_hz < frequency_limits.min_output_hz || q_hz > frequency_limits.max_output_hz)
                {
                    return "PLL Q output frequency is out of range";
                }
            }

            return std::nullopt;
        }

        // ---------------------------------------------------------------------
        // Frequency Helpers
        // ---------------------------------------------------------------------

        [[nodiscard]] static uint32_t get_r_output_freq_hz(const uint32_t source_hz)
        {
            const uint32_t vco_hz = calculate_vco_freq_hz(source_hz, get_m_divider(), get_n_multiplier());
            return calculate_r_output_freq_hz(vco_hz, get_r_divider());
        }

        [[nodiscard]] static uint32_t get_p_output_freq_hz(const uint32_t source_hz)
        {
            const uint32_t vco_hz = calculate_vco_freq_hz(source_hz, get_m_divider(), get_n_multiplier());
            return calculate_p_output_freq_hz(vco_hz, get_p_divider());
        }

        [[nodiscard]] static uint32_t get_q_output_freq_hz(const uint32_t source_hz)
        {
            const uint32_t vco_hz = calculate_vco_freq_hz(source_hz, get_m_divider(), get_n_multiplier());
            return calculate_q_output_freq_hz(vco_hz, get_q_divider());
        }
    };
}  // namespace valle::platform