#pragma once

#include <cstdint>
#include <optional>
#include <string_view>

#include "stm32g4xx_ll_rcc.h"
#include "valle/platform/hardware/power.hpp"

namespace valle::platform
{
    // =============================================================================
    // ENUMERATIONS
    // =============================================================================
    /**
     * @brief System clock source selection.
     *
     * These represent the possible clock sources that can feed SYSCLK.
     */
    enum class SCTSource : uint32_t
    {
        kHSI = LL_RCC_SYS_CLKSOURCE_HSI,
        kHSE = LL_RCC_SYS_CLKSOURCE_HSE,
        kPLL = LL_RCC_SYS_CLKSOURCE_PLL
    };

    /**
     * @brief Current system clock source status.
     *
     * These values represent the hardware-reported active SYSCLK source after
     * clock switching has completed.
     */
    enum class SCTSourceStatus : uint32_t
    {
        kHSI = LL_RCC_SYS_CLKSOURCE_STATUS_HSI,
        kHSE = LL_RCC_SYS_CLKSOURCE_STATUS_HSE,
        kPLL = LL_RCC_SYS_CLKSOURCE_STATUS_PLL
    };

    enum class SCTAHBPrescaler : uint32_t
    {
        kDiv1   = LL_RCC_SYSCLK_DIV_1,
        kDiv2   = LL_RCC_SYSCLK_DIV_2,
        kDiv4   = LL_RCC_SYSCLK_DIV_4,
        kDiv8   = LL_RCC_SYSCLK_DIV_8,
        kDiv16  = LL_RCC_SYSCLK_DIV_16,
        kDiv64  = LL_RCC_SYSCLK_DIV_64,
        kDiv128 = LL_RCC_SYSCLK_DIV_128,
        kDiv256 = LL_RCC_SYSCLK_DIV_256,
        kDiv512 = LL_RCC_SYSCLK_DIV_512
    };

    enum class SCTAPB1Prescaler : uint32_t
    {
        kDiv1  = LL_RCC_APB1_DIV_1,
        kDiv2  = LL_RCC_APB1_DIV_2,
        kDiv4  = LL_RCC_APB1_DIV_4,
        kDiv8  = LL_RCC_APB1_DIV_8,
        kDiv16 = LL_RCC_APB1_DIV_16
    };

    enum class SCTAPB2Prescaler : uint32_t
    {
        kDiv1  = LL_RCC_APB2_DIV_1,
        kDiv2  = LL_RCC_APB2_DIV_2,
        kDiv4  = LL_RCC_APB2_DIV_4,
        kDiv8  = LL_RCC_APB2_DIV_8,
        kDiv16 = LL_RCC_APB2_DIV_16
    };

    // =============================================================================
    // INTERFACE
    // =============================================================================
    struct SCTFrequencyLimits
    {
        uint32_t sysclk_freq_hz_min = 0U;
        uint32_t sysclk_freq_hz_max = 0U;
        uint32_t hclk_freq_hz_min   = 0U;
        uint32_t hclk_freq_hz_max   = 0U;
        uint32_t pclk1_freq_hz_min  = 0U;
        uint32_t pclk1_freq_hz_max  = 0U;
        uint32_t pclk2_freq_hz_min  = 0U;
        uint32_t pclk2_freq_hz_max  = 0U;
        uint32_t flash_freq_hz_min  = 0U;
        uint32_t flash_freq_hz_max  = 0U;
    };

    /**
     * @brief Interface for the system clock source multiplexer.
     */
    struct SCTInterface
    {
        static constexpr uint32_t skDefaultSourceSwitchTimeoutCount = 1'000'000U;

        static constexpr auto skRange1BoostFrequencyLimits = SCTFrequencyLimits{.sysclk_freq_hz_min = 0U,
                                                                                .sysclk_freq_hz_max = 170'000'000U,
                                                                                .hclk_freq_hz_min   = 0U,
                                                                                .hclk_freq_hz_max   = 170'000'000U,
                                                                                .pclk1_freq_hz_min  = 0U,
                                                                                .pclk1_freq_hz_max  = 170'000'000U,
                                                                                .pclk2_freq_hz_min  = 0U,
                                                                                .pclk2_freq_hz_max  = 170'000'000U,
                                                                                .flash_freq_hz_min  = 0U,
                                                                                .flash_freq_hz_max  = 170'000'000U};

        static constexpr auto skRange1NormalFrequencyLimits = SCTFrequencyLimits{.sysclk_freq_hz_min = 0U,
                                                                                 .sysclk_freq_hz_max = 150'000'000U,
                                                                                 .hclk_freq_hz_min   = 0U,
                                                                                 .hclk_freq_hz_max   = 150'000'000U,
                                                                                 .pclk1_freq_hz_min  = 0U,
                                                                                 .pclk1_freq_hz_max  = 150'000'000U,
                                                                                 .pclk2_freq_hz_min  = 0U,
                                                                                 .pclk2_freq_hz_max  = 150'000'000U,
                                                                                 .flash_freq_hz_min  = 0U,
                                                                                 .flash_freq_hz_max  = 150'000'000U};

        static constexpr auto skRange2FrequencyLimits = SCTFrequencyLimits{.sysclk_freq_hz_min = 0U,
                                                                           .sysclk_freq_hz_max = 26'000'000U,
                                                                           .hclk_freq_hz_min   = 0U,
                                                                           .hclk_freq_hz_max   = 26'000'000U,
                                                                           .pclk1_freq_hz_min  = 0U,
                                                                           .pclk1_freq_hz_max  = 26'000'000U,
                                                                           .pclk2_freq_hz_min  = 0U,
                                                                           .pclk2_freq_hz_max  = 26'000'000U,
                                                                           .flash_freq_hz_min  = 0U,
                                                                           .flash_freq_hz_max  = 26'000'000U};
        // ---------------------------------------------------------------------
        // SYSCLK source control
        // ---------------------------------------------------------------------
        /**
         * @brief Set the requested SYSCLK source.
         *
         * @param source Desired SYSCLK source.
         */
        static void set_source(const SCTSource source)
        {
            LL_RCC_SetSysClkSource(static_cast<uint32_t>(source));
        }

        /**
         * @brief Get the currently active SYSCLK source status.
         *
         * This reflects the hardware-reported active system clock source after
         * clock switching has completed.
         *
         * @return Active SYSCLK source status.
         */
        [[nodiscard]] static SCTSourceStatus get_source_status()
        {
            return static_cast<SCTSourceStatus>(LL_RCC_GetSysClkSource());
        }

        /**
         * @brief Convert a requested SYSCLK source into its corresponding status value.
         *
         * @param source Requested SYSCLK source.
         * @return Expected active source status corresponding to that source.
         */
        [[nodiscard]] static constexpr SCTSourceStatus to_status(const SCTSource source)
        {
            switch (source)
            {
                case SCTSource::kHSI:
                    return SCTSourceStatus::kHSI;
                case SCTSource::kHSE:
                    return SCTSourceStatus::kHSE;
                case SCTSource::kPLL:
                    return SCTSourceStatus::kPLL;
            }

            return SCTSourceStatus::kHSI;
        }

        /**
         * @brief Check whether the requested SYSCLK source is currently active.
         *
         * @param source Requested SYSCLK source.
         * @return true if the source switch has completed and the requested source
         *         is active, false otherwise.
         */
        [[nodiscard]] static bool is_source_active(const SCTSource source)
        {
            return get_source_status() == to_status(source);
        }

        // ---------------------------------------------------------------------
        // Prescaler control
        // ---------------------------------------------------------------------

        static void set_ahb_prescaler(const SCTAHBPrescaler prescaler)
        {
            LL_RCC_SetAHBPrescaler(static_cast<uint32_t>(prescaler));
        }

        static void set_apb1_prescaler(const SCTAPB1Prescaler prescaler)
        {
            LL_RCC_SetAPB1Prescaler(static_cast<uint32_t>(prescaler));
        }

        static void set_apb2_prescaler(const SCTAPB2Prescaler prescaler)
        {
            LL_RCC_SetAPB2Prescaler(static_cast<uint32_t>(prescaler));
        }

        [[nodiscard]] static SCTAHBPrescaler get_ahb_prescaler()
        {
            return static_cast<SCTAHBPrescaler>(LL_RCC_GetAHBPrescaler());
        }

        [[nodiscard]] static SCTAPB1Prescaler get_apb1_prescaler()
        {
            return static_cast<SCTAPB1Prescaler>(LL_RCC_GetAPB1Prescaler());
        }

        [[nodiscard]] static SCTAPB2Prescaler get_apb2_prescaler()
        {
            return static_cast<SCTAPB2Prescaler>(LL_RCC_GetAPB2Prescaler());
        }

        // ---------------------------------------------------------------------
        // Divider factor helpers
        // ---------------------------------------------------------------------
        [[nodiscard]] static constexpr uint32_t get_ahb_divider_factor(const SCTAHBPrescaler prescaler)
        {
            // NOLINTBEGIN(readability-magic-numbers)
            switch (prescaler)
            {
                case SCTAHBPrescaler::kDiv1:
                    return 1;
                case SCTAHBPrescaler::kDiv2:
                    return 2;
                case SCTAHBPrescaler::kDiv4:
                    return 4;
                case SCTAHBPrescaler::kDiv8:
                    return 8;
                case SCTAHBPrescaler::kDiv16:
                    return 16;
                case SCTAHBPrescaler::kDiv64:
                    return 64;
                case SCTAHBPrescaler::kDiv128:
                    return 128;
                case SCTAHBPrescaler::kDiv256:
                    return 256;
                case SCTAHBPrescaler::kDiv512:
                    return 512;
            }
            // NOLINTEND(readability-magic-numbers)

            return 0;
        }

        [[nodiscard]] static constexpr uint32_t get_apb1_divider_factor(const SCTAPB1Prescaler prescaler)
        {
            // NOLINTBEGIN(readability-magic-numbers)
            switch (prescaler)
            {
                case SCTAPB1Prescaler::kDiv1:
                    return 1;
                case SCTAPB1Prescaler::kDiv2:
                    return 2;
                case SCTAPB1Prescaler::kDiv4:
                    return 4;
                case SCTAPB1Prescaler::kDiv8:
                    return 8;
                case SCTAPB1Prescaler::kDiv16:
                    return 16;
            }
            // NOLINTEND(readability-magic-numbers)

            return 0;
        }

        [[nodiscard]] static constexpr uint32_t get_apb2_divider_factor(const SCTAPB2Prescaler prescaler)
        {
            // NOLINTBEGIN(readability-magic-numbers)
            switch (prescaler)
            {
                case SCTAPB2Prescaler::kDiv1:
                    return 1;
                case SCTAPB2Prescaler::kDiv2:
                    return 2;
                case SCTAPB2Prescaler::kDiv4:
                    return 4;
                case SCTAPB2Prescaler::kDiv8:
                    return 8;
                case SCTAPB2Prescaler::kDiv16:
                    return 16;
            }
            // NOLINTEND(readability-magic-numbers)

            return 0;
        }

        // ---------------------------------------------------------------------
        // Frequency Calculation Helpers
        // ---------------------------------------------------------------------
        [[nodiscard]] static constexpr uint32_t calculate_hclk_freq_hz(const uint32_t        sysclk_hz,
                                                                       const SCTAHBPrescaler ahb_prescaler)
        {
            const uint32_t ahb_div = get_ahb_divider_factor(ahb_prescaler);
            return (ahb_div == 0U) ? 0U : (sysclk_hz / ahb_div);
        }

        [[nodiscard]] static constexpr uint32_t calculate_flash_freq_hz(const uint32_t hclk_hz)
        {
            return hclk_hz;
        }

        [[nodiscard]] static constexpr uint32_t calculate_pclk1_freq_hz(const uint32_t         hclk_hz,
                                                                        const SCTAPB1Prescaler apb1_prescaler)
        {
            const uint32_t apb1_div = get_apb1_divider_factor(apb1_prescaler);
            return (apb1_div == 0U) ? 0U : (hclk_hz / apb1_div);
        }

        [[nodiscard]] static constexpr uint32_t calculate_apb1_peripheral_freq_hz(const uint32_t pclk1_hz)
        {
            return pclk1_hz;
        }

        [[nodiscard]] static constexpr uint32_t calculate_apb1_timer_freq_hz(const uint32_t         pclk1_hz,
                                                                             const SCTAPB1Prescaler apb1_prescaler)
        {
            if (apb1_prescaler == SCTAPB1Prescaler::kDiv1)
            {
                return pclk1_hz;
            }

            return pclk1_hz * 2;
        }

        [[nodiscard]] static constexpr uint32_t calculate_pclk2_freq_hz(const uint32_t         hclk_hz,
                                                                        const SCTAPB2Prescaler apb2_prescaler)
        {
            const uint32_t apb2_div = get_apb2_divider_factor(apb2_prescaler);
            return (apb2_div == 0U) ? 0U : (hclk_hz / apb2_div);
        }

        [[nodiscard]] static constexpr uint32_t calculate_apb2_peripheral_freq_hz(const uint32_t pclk2_hz)
        {
            return pclk2_hz;
        }

        [[nodiscard]] static constexpr uint32_t calculate_apb2_timer_freq_hz(const uint32_t         pclk2_hz,
                                                                             const SCTAPB2Prescaler apb2_prescaler)
        {
            if (apb2_prescaler == SCTAPB2Prescaler::kDiv1)
            {
                return pclk2_hz;
            }

            return pclk2_hz * 2;
        }

        // ---------------------------------------------------------------------
        // Frequency Helpers
        // ---------------------------------------------------------------------
        [[nodiscard]] static uint32_t get_hclk_freq_hz(const uint32_t sysclk_hz)
        {
            return calculate_hclk_freq_hz(sysclk_hz, get_ahb_prescaler());
        }

        [[nodiscard]] static uint32_t get_flash_freq_hz(const uint32_t hclk_hz)
        {
            return hclk_hz;
        }

        [[nodiscard]] static uint32_t get_pclk1_freq_hz(const uint32_t hclk_hz)
        {
            return calculate_pclk1_freq_hz(hclk_hz, get_apb1_prescaler());
        }

        [[nodiscard]] static uint32_t get_apb1_peripheral_freq_hz(const uint32_t pclk1_hz)
        {
            return calculate_apb1_peripheral_freq_hz(pclk1_hz);
        }

        [[nodiscard]] static uint32_t get_apb1_timer_freq_hz(const uint32_t pclk1_hz)
        {
            return calculate_apb1_timer_freq_hz(pclk1_hz, get_apb1_prescaler());
        }

        [[nodiscard]] static uint32_t get_pclk2_freq_hz(const uint32_t hclk_hz)
        {
            return calculate_pclk2_freq_hz(hclk_hz, get_apb2_prescaler());
        }

        [[nodiscard]] static uint32_t get_apb2_peripheral_freq_hz(const uint32_t pclk2_hz)
        {
            return calculate_apb2_peripheral_freq_hz(pclk2_hz);
        }

        [[nodiscard]] static uint32_t get_apb2_timer_freq_hz(const uint32_t pclk2_hz)
        {
            return calculate_apb2_timer_freq_hz(pclk2_hz, get_apb2_prescaler());
        }

        // ---------------------------------------------------------------------
        // Validation Helpers
        // ---------------------------------------------------------------------
        [[nodiscard]] static constexpr SCTFrequencyLimits get_frequency_limits(
            const PowerVoltageRangeMode voltage_range)
        {
            switch (voltage_range)
            {
                case PowerVoltageRangeMode::kRange1Boost:
                    return skRange1BoostFrequencyLimits;
                case PowerVoltageRangeMode::kRange1Normal:
                    return skRange1NormalFrequencyLimits;
                case PowerVoltageRangeMode::kRange2:
                    return skRange2FrequencyLimits;
                default:
                    return SCTFrequencyLimits{};
            }
        }

        [[nodiscard]] static constexpr std::optional<std::string_view> validate_sysclk_freq(
            const uint32_t sysclk_hz, const SCTFrequencyLimits& limits)
        {
            if (sysclk_hz < limits.sysclk_freq_hz_min || sysclk_hz > limits.sysclk_freq_hz_max)
            {
                return "SYSCLK frequency is out of range";
            }
            return std::nullopt;
        }

        [[nodiscard]] static constexpr std::optional<std::string_view> validate_hclk_freq(
            const uint32_t hclk_hz, const SCTFrequencyLimits& limits)
        {
            if (hclk_hz < limits.hclk_freq_hz_min || hclk_hz > limits.hclk_freq_hz_max)
            {
                return "HCLK frequency is out of range";
            }
            return std::nullopt;
        }

        [[nodiscard]] static constexpr std::optional<std::string_view> validate_pclk1_freq(
            const uint32_t pclk1_hz, const SCTFrequencyLimits& limits)
        {
            if (pclk1_hz < limits.pclk1_freq_hz_min || pclk1_hz > limits.pclk1_freq_hz_max)
            {
                return "PCLK1 frequency is out of range";
            }
            return std::nullopt;
        }

        [[nodiscard]] static constexpr std::optional<std::string_view> validate_pclk2_freq(
            const uint32_t pclk2_hz, const SCTFrequencyLimits& limits)
        {
            if (pclk2_hz < limits.pclk2_freq_hz_min || pclk2_hz > limits.pclk2_freq_hz_max)
            {
                return "PCLK2 frequency is out of range";
            }
            return std::nullopt;
        }

        [[nodiscard]] static constexpr std::optional<std::string_view> validate_flash_frequency(
            const uint32_t hclk_hz, const SCTFrequencyLimits& limits)
        {
            if (hclk_hz < limits.flash_freq_hz_min || hclk_hz > limits.flash_freq_hz_max)
            {
                return "Flash frequency is out of range";
            }
            return std::nullopt;
        }

        [[nodiscard]] static constexpr std::optional<std::string_view>
        validate_config(  // NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
            const uint32_t              sysclk_hz,
            const PowerVoltageRangeMode voltage_range,
            const SCTAHBPrescaler       ahb_prescaler,
            const SCTAPB1Prescaler      apb1_prescaler,
            const SCTAPB2Prescaler      apb2_prescaler)
        {
            const SCTFrequencyLimits frequency_limits = get_frequency_limits(voltage_range);

            if (sysclk_hz == 0U)
            {
                return "SYSCLK frequency cannot be zero";
            }

            if (const auto sysclk_freq_error = validate_sysclk_freq(sysclk_hz, frequency_limits))
            {
                return sysclk_freq_error;
            }

            const uint32_t hclk_hz = calculate_hclk_freq_hz(sysclk_hz, ahb_prescaler);
            if (const auto hclk_freq_error = validate_hclk_freq(hclk_hz, frequency_limits))
            {
                return hclk_freq_error;
            }

            const uint32_t flash_hz = calculate_flash_freq_hz(hclk_hz);
            if (const auto flash_freq_error = validate_flash_frequency(flash_hz, frequency_limits))
            {
                return flash_freq_error;
            }

            const uint32_t pclk1_hz = calculate_pclk1_freq_hz(hclk_hz, apb1_prescaler);
            if (const auto pclk1_freq_error = validate_pclk1_freq(pclk1_hz, frequency_limits))
            {
                return pclk1_freq_error;
            }

            const uint32_t pclk2_hz = calculate_pclk2_freq_hz(hclk_hz, apb2_prescaler);
            if (const auto pclk2_freq_error = validate_pclk2_freq(pclk2_hz, frequency_limits))
            {
                return pclk2_freq_error;
            }

            return std::nullopt;
        }
    };

}  // namespace valle::platform
