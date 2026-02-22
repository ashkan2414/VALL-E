#pragma once

#include "Valle/Board/Traits/ADCCLK/id.hpp"
#include "stm32g4xx_ll_adc.h"

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

}  // namespace valle