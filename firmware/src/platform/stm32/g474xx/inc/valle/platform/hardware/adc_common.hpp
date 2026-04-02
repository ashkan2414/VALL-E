#pragma once

#include "stm32g4xx_ll_adc.h"
#include "stm32g4xx_ll_bus.h"
#include "valle/platform/hardware/rcc/sct.hpp"

namespace valle::platform
{
    // ============================================================================
    // ID
    // ============================================================================
    enum class AdcCommonControllerId : uint8_t
    {
        kAdc12 = 0,
        kAdc345
    };

    // =============================================================================
    // SPECIFICATIONS
    // ============================================================================
    struct AdcCommonControllerSpec
    {
        AdcCommonControllerId controller_id;
    };

    inline constexpr auto kAdc12CommonControllerSpec  = AdcCommonControllerSpec{AdcCommonControllerId::kAdc12};
    inline constexpr auto kAdc345CommonControllerSpec = AdcCommonControllerSpec{AdcCommonControllerId::kAdc345};

    // ============================================================================
    // ENUMERATIONS
    // ============================================================================
    enum class AdcCommonAsyncClockPrescaler : uint32_t
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

    enum class AdcCommonSyncClockPrescaler : uint32_t
    {
        kDiv1 = LL_ADC_CLOCK_SYNC_PCLK_DIV1,
        kDiv2 = LL_ADC_CLOCK_SYNC_PCLK_DIV2,
        kDiv4 = LL_ADC_CLOCK_SYNC_PCLK_DIV4
    };

    // =========================================================================
    // ROOT TRAITS
    // ===========================================================================

    struct AdcCommonRootTraits
    {
        static constexpr uint32_t skMaxClockFreqHz = 60'000'000U;  // 60MHz

        [[nodiscard]] static constexpr uint8_t controller_id_to_index(const AdcCommonControllerId controller_id)
        {
            static_assert(static_cast<uint8_t>(AdcCommonControllerId::kAdc12) == 0,
                          "AdcCommonControllerId enum values must start at 0");
            return static_cast<uint8_t>(controller_id);
        }

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

    // ============================================================================
    // HARDWARE TRAITS
    // ============================================================================
    template <AdcCommonControllerSpec tkControllerSpec>
    struct AdcCommonControllerTraits;

    template <>
    struct AdcCommonControllerTraits<AdcCommonControllerSpec{AdcCommonControllerId::kAdc12}>
    {
        static inline ADC_Common_TypeDef *const skInstance = ADC12_COMMON;

        static void enable_clock()
        {
            LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_ADC12);
        }
    };

    template <>
    struct AdcCommonControllerTraits<AdcCommonControllerSpec{AdcCommonControllerId::kAdc345}>
    {
        static inline ADC_Common_TypeDef *const skInstance = ADC345_COMMON;

        static void enable_clock()
        {
            LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_ADC345);
        }
    };

    // ============================================================================
    // INTERFACE
    // ============================================================================

    template <AdcCommonControllerSpec tkControllerSpec>
    struct AdcCommonControllerInterface : public AdcCommonRootTraits
    {
        using TraitsT = AdcCommonControllerTraits<tkControllerSpec>;

        void enable_clock()
        {
            if constexpr (tkControllerSpec.controller_id == AdcCommonControllerId::kAdc12)
            {
                LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_ADC12);
            }
            else if constexpr (tkControllerSpec.controller_id == AdcCommonControllerId::kAdc345)
            {
                LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_ADC345);
            }
            else
            {
                static_assert(kAlwaysFalseV<tkControllerSpec.controller_id>, "Unsupported ADC Common ID");
            }
        }

        void set_common_clock_async(const AdcCommonAsyncClockPrescaler prescaler) const
        {
            LL_ADC_SetCommonClock(TraitsT::skInstance, static_cast<uint32_t>(prescaler));
        }

        void set_common_clock_sync(const AdcCommonSyncClockPrescaler prescaler) const
        {
            LL_ADC_SetCommonClock(TraitsT::skInstance, static_cast<uint32_t>(prescaler));
        }
    };

}  // namespace valle::platform

__STATIC_INLINE void LL_ADC_SetCommonPathInternalCh(ADC_Common_TypeDef *ADCxy_COMMON, uint32_t PathInternal)
__STATIC_INLINE void LL_ADC_SetCommonPathInternalChAdd(ADC_Common_TypeDef *ADCxy_COMMON, uint32_t PathInternal)
__STATIC_INLINE void LL_ADC_SetCommonPathInternalChRem(ADC_Common_TypeDef *ADCxy_COMMON, uint32_t PathInternal)
__STATIC_INLINE uint32_t LL_ADC_GetCommonPathInternalCh(const ADC_Common_TypeDef *ADCxy_COMMON)

__STATIC_INLINE void LL_ADC_SetMultimode(ADC_Common_TypeDef *ADCxy_COMMON, uint32_t Multimode)
__STATIC_INLINE uint32_t LL_ADC_GetMultimode(const ADC_Common_TypeDef *ADCxy_COMMON)
__STATIC_INLINE void LL_ADC_SetMultiDMATransfer(ADC_Common_TypeDef *ADCxy_COMMON, uint32_t MultiDMATransfer)
__STATIC_INLINE uint32_t LL_ADC_GetMultiDMATransfer(const ADC_Common_TypeDef *ADCxy_COMMON)
__STATIC_INLINE void LL_ADC_SetMultiTwoSamplingDelay(ADC_Common_TypeDef *ADCxy_COMMON, uint32_t MultiTwoSamplingDelay)
__STATIC_INLINE uint32_t LL_ADC_GetMultiTwoSamplingDelay(const ADC_Common_TypeDef *ADCxy_COMMON)

__STATIC_INLINE uint32_t LL_ADC_REG_ReadMultiConversionData32(const ADC_Common_TypeDef *ADCxy_COMMON,

__STATIC_INLINE uint32_t LL_ADC_IsActiveFlag_MST_ADRDY(const ADC_Common_TypeDef *ADCxy_COMMON)
__STATIC_INLINE uint32_t LL_ADC_IsActiveFlag_SLV_ADRDY(const ADC_Common_TypeDef *ADCxy_COMMON)
__STATIC_INLINE uint32_t LL_ADC_IsActiveFlag_MST_EOC(const ADC_Common_TypeDef *ADCxy_COMMON)
__STATIC_INLINE uint32_t LL_ADC_IsActiveFlag_SLV_EOC(const ADC_Common_TypeDef *ADCxy_COMMON)
__STATIC_INLINE uint32_t LL_ADC_IsActiveFlag_MST_EOS(const ADC_Common_TypeDef *ADCxy_COMMON)
__STATIC_INLINE uint32_t LL_ADC_IsActiveFlag_SLV_EOS(const ADC_Common_TypeDef *ADCxy_COMMON)
__STATIC_INLINE uint32_t LL_ADC_IsActiveFlag_MST_OVR(const ADC_Common_TypeDef *ADCxy_COMMON)
__STATIC_INLINE uint32_t LL_ADC_IsActiveFlag_SLV_OVR(const ADC_Common_TypeDef *ADCxy_COMMON)
__STATIC_INLINE uint32_t LL_ADC_IsActiveFlag_MST_EOSMP(const ADC_Common_TypeDef *ADCxy_COMMON)
__STATIC_INLINE uint32_t LL_ADC_IsActiveFlag_SLV_EOSMP(const ADC_Common_TypeDef *ADCxy_COMMON)
__STATIC_INLINE uint32_t LL_ADC_IsActiveFlag_MST_JEOC(const ADC_Common_TypeDef *ADCxy_COMMON)
__STATIC_INLINE uint32_t LL_ADC_IsActiveFlag_SLV_JEOC(const ADC_Common_TypeDef *ADCxy_COMMON)
__STATIC_INLINE uint32_t LL_ADC_IsActiveFlag_MST_JEOS(const ADC_Common_TypeDef *ADCxy_COMMON)
__STATIC_INLINE uint32_t LL_ADC_IsActiveFlag_SLV_JEOS(const ADC_Common_TypeDef *ADCxy_COMMON)
__STATIC_INLINE uint32_t LL_ADC_IsActiveFlag_MST_JQOVF(const ADC_Common_TypeDef *ADCxy_COMMON)
__STATIC_INLINE uint32_t LL_ADC_IsActiveFlag_SLV_JQOVF(const ADC_Common_TypeDef *ADCxy_COMMON)
__STATIC_INLINE uint32_t LL_ADC_IsActiveFlag_MST_AWD1(const ADC_Common_TypeDef *ADCxy_COMMON)
__STATIC_INLINE uint32_t LL_ADC_IsActiveFlag_SLV_AWD1(const ADC_Common_TypeDef *ADCxy_COMMON)
__STATIC_INLINE uint32_t LL_ADC_IsActiveFlag_MST_AWD2(const ADC_Common_TypeDef *ADCxy_COMMON)
__STATIC_INLINE uint32_t LL_ADC_IsActiveFlag_SLV_AWD2(const ADC_Common_TypeDef *ADCxy_COMMON)
__STATIC_INLINE uint32_t LL_ADC_IsActiveFlag_MST_AWD3(const ADC_Common_TypeDef *ADCxy_COMMON)
__STATIC_INLINE uint32_t LL_ADC_IsActiveFlag_SLV_AWD3(const ADC_Common_TypeDef *ADCxy_COMMON)