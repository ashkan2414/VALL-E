#pragma once

#include "stm32g4xx_ll_adc.h"
#include "stm32g4xx_ll_bus.h"
#include "valle/platform/hardware/rcc/sct.hpp"

namespace valle::platform
{
    enum class ADCCommonID : uint32_t
    {
        kADC12,
        kADC345
    };

    // ============================================================================
    // ENUMERATIONS
    // ============================================================================
    enum class ADCCommonAsyncClockPrescaler
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

    enum class ADCCommonSyncClockPrescaler
    {
        kDiv1 = LL_ADC_CLOCK_SYNC_PCLK_DIV1,
        kDiv2 = LL_ADC_CLOCK_SYNC_PCLK_DIV2,
        kDiv4 = LL_ADC_CLOCK_SYNC_PCLK_DIV4
    };

    // ============================================================================
    // HARDWARE TRAITS
    // ============================================================================
    template <ADCCommonID tkCommonID>
    struct ADCCommonTraits;

    template <>
    struct ADCCommonTraits<ADCCommonID::kADC12>
    {
        static inline ADC_Common_TypeDef* const skInstance = ADC12_COMMON;

        static void enable_clock()
        {
            LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_ADC12);
        }
    };

    template <>
    struct ADCCommonTraits<ADCCommonID::kADC345>
    {
        static inline ADC_Common_TypeDef* const skInstance = ADC345_COMMON;

        static void enable_clock()
        {
            LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_ADC345);
        }
    };

    // ============================================================================
    // INTERFACE
    // ============================================================================

    struct ADCCommonRootInterface
    {
        static constexpr uint32_t skMaxClockFreqHz = 60'000'000U;  // 60MHz

        static constexpr uint32_t get_prescaler_factor(const ADCCommonAsyncClockPrescaler prescaler)
        {
            // NOLINTBEGIN(readability-magic-numbers)
            switch (prescaler)
            {
                case ADCCommonAsyncClockPrescaler::kDiv1:
                    return 1;
                case ADCCommonAsyncClockPrescaler::kDiv2:
                    return 2;
                case ADCCommonAsyncClockPrescaler::kDiv4:
                    return 4;
                case ADCCommonAsyncClockPrescaler::kDiv6:
                    return 6;
                case ADCCommonAsyncClockPrescaler::kDiv8:
                    return 8;
                case ADCCommonAsyncClockPrescaler::kDiv10:
                    return 10;
                case ADCCommonAsyncClockPrescaler::kDiv12:
                    return 12;
                case ADCCommonAsyncClockPrescaler::kDiv16:
                    return 16;
                case ADCCommonAsyncClockPrescaler::kDiv32:
                    return 32;
                case ADCCommonAsyncClockPrescaler::kDiv64:
                    return 64;
                case ADCCommonAsyncClockPrescaler::kDiv128:
                    return 128;
                case ADCCommonAsyncClockPrescaler::kDiv256:
                    return 256;
                default:
                    return 1;
            }
            // NOLINTEND(readability-magic-numbers)
        }

        static constexpr uint32_t get_prescaler_factor(const ADCCommonSyncClockPrescaler prescaler)
        {
            // NOLINTBEGIN(readability-magic-numbers)
            switch (prescaler)
            {
                case ADCCommonSyncClockPrescaler::kDiv1:
                    return 1;
                case ADCCommonSyncClockPrescaler::kDiv2:
                    return 2;
                case ADCCommonSyncClockPrescaler::kDiv4:
                    return 4;
                default:
                    return 1;
            }
            // NOLINTEND(readability-magic-numbers)
        }

        static constexpr uint32_t calculate_async_clock_freq_hz(const uint32_t                     async_clock_freq_hz,
                                                                const ADCCommonAsyncClockPrescaler prescaler)
        {
            return async_clock_freq_hz / get_prescaler_factor(prescaler);
        }

        static constexpr uint32_t calculate_sync_clock_freq_hz(const uint32_t                    sync_clock_freq_hz,
                                                               const ADCCommonSyncClockPrescaler prescaler)
        {
            return sync_clock_freq_hz / get_prescaler_factor(prescaler);
        }
    };

}  // namespace valle::platform