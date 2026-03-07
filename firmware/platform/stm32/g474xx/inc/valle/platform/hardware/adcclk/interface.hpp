#pragma once

#include "stm32g4xx_ll_adc.h"
#include "stm32g4xx_ll_bus.h"
#include "valle/platform/hardware/adcclk/id.hpp"

namespace valle
{
    // ============================================================================
    // ENUMERATIONS
    // ============================================================================
    enum class ADCAsyncClockSource
    {
        kSysclk,
        kPLL
    };

    enum class ADCAsyncClockPrescaler
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

    enum class ADCSyncClockPrescaler
    {
        kDiv1 = LL_ADC_CLOCK_SYNC_PCLK_DIV1,
        kDiv2 = LL_ADC_CLOCK_SYNC_PCLK_DIV2,
        kDiv4 = LL_ADC_CLOCK_SYNC_PCLK_DIV4
    };

    // ============================================================================
    // HARDWARE TRAITS
    // ============================================================================
    template <ADCClockID tkClockID>
    struct ADCClockTraits;

    template <>
    struct ADCClockTraits<ADCClockID::kADC12>
    {
        static constexpr uint32_t               skPeripClock     = RCC_PERIPHCLK_ADC12;
        static constexpr uint32_t               skClockBusEnable = LL_AHB2_GRP1_PERIPH_ADC12;
        static inline ADC_Common_TypeDef* const skCommon         = ADC12_COMMON;

        static void enable_clock()
        {
            LL_AHB2_GRP1_EnableClock(skClockBusEnable);
        }

        static constexpr uint32_t get_async_clock_selection(const ADCAsyncClockSource source)
        {
            switch (source)
            {
                case ADCAsyncClockSource::kSysclk:  // NOLINT(bugprone-branch-clone)
                    return RCC_ADC12CLKSOURCE_SYSCLK;
                case ADCAsyncClockSource::kPLL:
                    return RCC_ADC12CLKSOURCE_PLL;
                default:
                    return RCC_ADC12CLKSOURCE_SYSCLK;
            }
        }
    };

    template <>
    struct ADCClockTraits<ADCClockID::kADC345>
    {
        static constexpr uint32_t               skPeripClock     = RCC_PERIPHCLK_ADC345;
        static constexpr uint32_t               skClockBusEnable = LL_AHB2_GRP1_PERIPH_ADC345;
        static inline ADC_Common_TypeDef* const skCommon         = ADC345_COMMON;

        static void enable_clock()
        {
            LL_AHB2_GRP1_EnableClock(skClockBusEnable);
        }

        static constexpr uint32_t get_async_clock_selection(const ADCAsyncClockSource source)
        {
            switch (source)
            {
                case ADCAsyncClockSource::kSysclk:  // NOLINT(bugprone-branch-clone)
                    return RCC_ADC345CLKSOURCE_SYSCLK;
                case ADCAsyncClockSource::kPLL:
                    return RCC_ADC345CLKSOURCE_PLL;
                default:
                    return RCC_ADC345CLKSOURCE_SYSCLK;
            }
        }
    };

    // ============================================================================
    // INTERFACE
    // ============================================================================

    struct ADCClockRootInterface
    {
        static constexpr uint32_t get_prescaler_factor(const ADCAsyncClockPrescaler prescaler)
        {
            // NOLINTBEGIN(readability-magic-numbers)
            switch (prescaler)
            {
                case ADCAsyncClockPrescaler::kDiv1:
                    return 1;
                case ADCAsyncClockPrescaler::kDiv2:
                    return 2;
                case ADCAsyncClockPrescaler::kDiv4:
                    return 4;
                case ADCAsyncClockPrescaler::kDiv6:
                    return 6;
                case ADCAsyncClockPrescaler::kDiv8:
                    return 8;
                case ADCAsyncClockPrescaler::kDiv10:
                    return 10;
                case ADCAsyncClockPrescaler::kDiv12:
                    return 12;
                case ADCAsyncClockPrescaler::kDiv16:
                    return 16;
                case ADCAsyncClockPrescaler::kDiv32:
                    return 32;
                case ADCAsyncClockPrescaler::kDiv64:
                    return 64;
                case ADCAsyncClockPrescaler::kDiv128:
                    return 128;
                case ADCAsyncClockPrescaler::kDiv256:
                    return 256;
                default:
                    return 1;
            }
            // NOLINTEND(readability-magic-numbers)
        }

        static constexpr uint32_t get_prescaler_factor(const ADCSyncClockPrescaler prescaler)
        {
            // NOLINTBEGIN(readability-magic-numbers)
            switch (prescaler)
            {
                case ADCSyncClockPrescaler::kDiv1:
                    return 1;
                case ADCSyncClockPrescaler::kDiv2:
                    return 2;
                case ADCSyncClockPrescaler::kDiv4:
                    return 4;
                default:
                    return 1;
            }
            // NOLINTEND(readability-magic-numbers)
        }

        static constexpr uint32_t calculate_clock_freq_hz(const uint32_t               async_clock_freq_hz,
                                                          const ADCAsyncClockPrescaler prescaler)
        {
            return async_clock_freq_hz / get_prescaler_factor(prescaler);
        }

        static constexpr uint32_t calculate_clock_freq_hz(const uint32_t              sync_clock_freq_hz,
                                                          const ADCSyncClockPrescaler prescaler)
        {
            return sync_clock_freq_hz / get_prescaler_factor(prescaler);
        }
    };

}  // namespace valle