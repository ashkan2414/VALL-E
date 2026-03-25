#pragma once

#include "stm32g4xx_ll_adc.h"
#include "stm32g4xx_ll_bus.h"
#include "valle/platform/hardware/rcc/sct.hpp"

namespace valle::platform
{
    enum class AdcCommonId : uint32_t
    {
        kAdc12,
        kAdc345
    };

    // ============================================================================
    // ENUMERATIONS
    // ============================================================================
    enum class AdcCommonAsyncClockPrescaler
    {
        kDiv1   = LL_ADC_CLOCK_ASYNC_DIV1,
        kDiv2   = LL_ADC_CLOCK_ASYNC_DIV2,
        kDiv4   = LL_ADC_CLOCK_ASYNC_DIV4,
        kDiv6   = LL_ADC_CLOCK_ASYNC_DIV6,
        kDiv8   = LL_ADC_CLOCK_ASYNC_DIV8,
        kDiv10  = LL_ADC_CLOCK_ASYNC_DIV10,
        kDiv12  = LL_ADC_CLOCK_ASYNC_DIV12,
        kDiv16  = LL_ADC_CLOCK_ASYNC_DIV16,
        kDiv32  = LL_ADC_CLOCK_ASYNC_DIV32,
        kDiv64  = LL_ADC_CLOCK_ASYNC_DIV64,
        kDiv128 = LL_ADC_CLOCK_ASYNC_DIV128,
        kDiv256 = LL_ADC_CLOCK_ASYNC_DIV256,
    };

    enum class AdcCommonSyncClockPrescaler
    {
        kDiv1 = LL_ADC_CLOCK_SYNC_PCLK_DIV1,
        kDiv2 = LL_ADC_CLOCK_SYNC_PCLK_DIV2,
        kDiv4 = LL_ADC_CLOCK_SYNC_PCLK_DIV4
    };

    // ============================================================================
    // HARDWARE TRAITS
    // ============================================================================
    template <AdcCommonId tkCommonId>
    struct AdcCommonTraits;

    template <>
    struct AdcCommonTraits<AdcCommonId::kAdc12>
    {
        static inline Adc_Common_TypeDef* const skInstance = Adc12_COMMON;

        static void enable_clock()
        {
            LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_ADC12);
        }
    };

    template <>
    struct AdcCommonTraits<AdcCommonId::kAdc345>
    {
        static inline Adc_Common_TypeDef* const skInstance = Adc345_COMMON;

        static void enable_clock()
        {
            LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_ADC345);
        }
    };

    // ============================================================================
    // INTERFACE
    // ============================================================================

    struct AdcCommonRootInterface
    {
        static constexpr uint32_t skMaxClockFreqHz = 60'000'000U;  // 60MHz

        static constexpr uint32_t get_prescaler_factor(const AdcCommonAsyncClockPrescaler prescaler)
        {
            // NOLINTBEGIN(readability-magic-numbers)
            switch (prescaler)
            {
                case AdcCommonAsyncClockPrescaler::kDiv1:
                    return 1;
                case AdcCommonAsyncClockPrescaler::kDiv2:
                    return 2;
                case AdcCommonAsyncClockPrescaler::kDiv4:
                    return 4;
                case AdcCommonAsyncClockPrescaler::kDiv6:
                    return 6;
                case AdcCommonAsyncClockPrescaler::kDiv8:
                    return 8;
                case AdcCommonAsyncClockPrescaler::kDiv10:
                    return 10;
                case AdcCommonAsyncClockPrescaler::kDiv12:
                    return 12;
                case AdcCommonAsyncClockPrescaler::kDiv16:
                    return 16;
                case AdcCommonAsyncClockPrescaler::kDiv32:
                    return 32;
                case AdcCommonAsyncClockPrescaler::kDiv64:
                    return 64;
                case AdcCommonAsyncClockPrescaler::kDiv128:
                    return 128;
                case AdcCommonAsyncClockPrescaler::kDiv256:
                    return 256;
                default:
                    return 1;
            }
            // NOLINTEND(readability-magic-numbers)
        }

        static constexpr uint32_t get_prescaler_factor(const AdcCommonSyncClockPrescaler prescaler)
        {
            // NOLINTBEGIN(readability-magic-numbers)
            switch (prescaler)
            {
                case AdcCommonSyncClockPrescaler::kDiv1:
                    return 1;
                case AdcCommonSyncClockPrescaler::kDiv2:
                    return 2;
                case AdcCommonSyncClockPrescaler::kDiv4:
                    return 4;
                default:
                    return 1;
            }
            // NOLINTEND(readability-magic-numbers)
        }

        static constexpr uint32_t calculate_async_clock_freq_hz(const uint32_t                     async_clock_freq_hz,
                                                                const AdcCommonAsyncClockPrescaler prescaler)
        {
            return async_clock_freq_hz / get_prescaler_factor(prescaler);
        }

        static constexpr uint32_t calculate_sync_clock_freq_hz(const uint32_t                    sync_clock_freq_hz,
                                                               const AdcCommonSyncClockPrescaler prescaler)
        {
            return sync_clock_freq_hz / get_prescaler_factor(prescaler);
        }
    };

}  // namespace valle::platform