#pragma once

#include <array>
#include <chrono>

#include "stm32g4xx_ll_adc.h"
#include "valle/platform/hardware/adc/id.hpp"
#include "valle/platform/hardware/adc_common.hpp"
#include "valle/platform/hardware/dma.hpp"
#include "valle/utils/timing.hpp"

namespace valle::platform
{
    // ============================================================================
    // ENUMERATIONS
    // ============================================================================

    enum class AdcResolution : uint32_t
    {
        k12Bit = LL_ADC_RESOLUTION_12B,
        k10Bit = LL_ADC_RESOLUTION_10B,
        k8Bit  = LL_ADC_RESOLUTION_8B,
        k6Bit  = LL_ADC_RESOLUTION_6B,
    };

    enum class AdcDataAlignment : uint32_t
    {
        kRight = LL_ADC_DATA_ALIGN_RIGHT,
        kLeft  = LL_ADC_DATA_ALIGN_LEFT,
    };

    enum class AdcLowPowerMode : uint32_t
    {
        kNone     = LL_ADC_LP_MODE_NONE,  /// No low power
        kAutoWait = LL_ADC_LP_AUTOWAIT,   /// Auto wait between conversions
    };

    enum class AdcCommonSamplingTime : uint32_t
    {
        kDefault                   = LL_ADC_SAMPLINGTimE_COMMON_DEFAULT,
        k3P5CyclesReplace2P5Cycles = LL_ADC_SAMPLINGTimE_COMMON_3C5_REPL_2C5,
    };

    enum class AdcInjectGroupQueueMode : uint32_t
    {
        kDisable             = LL_ADC_INJ_QUEUE_DISABLE,
        k2ContextsLastActive = LL_ADC_INJ_QUEUE_2CONTEXTS_LAST_ACTIVE,
        k2ContextsEndEmpty   = LL_ADC_INJ_QUEUE_2CONTEXTS_END_EMPTY
    };

    enum class AdcInjectGroupTriggerMode : uint32_t
    {
        kIndependent      = LL_ADC_INJ_TRIG_INDEPENDENT,
        kFromRegularGroup = LL_ADC_INJ_TRIG_FROM_GRP_REGULAR
    };

    enum class AdcInjectGroupSequencerDiscontinuityMode : uint32_t
    {
        kDisable = LL_ADC_INJ_SEQ_DISCONT_DISABLE,
        k1Rank   = LL_ADC_INJ_SEQ_DISCONT_1RANK
    };

    enum class AdcInjectGroupTriggerSource : uint32_t
    {
        kSoftware      = LL_ADC_INJ_TRIG_SOFTWARE,
        kExtTim1TRGO   = LL_ADC_INJ_TRIG_EXT_TIM1_TRGO,
        kExtTim1TRGO2  = LL_ADC_INJ_TRIG_EXT_TIM1_TRGO2,
        kExtTim1CH3    = LL_ADC_INJ_TRIG_EXT_TIM1_CH3,
        kExtTim1CH4    = LL_ADC_INJ_TRIG_EXT_TIM1_CH4,
        kExtTim2TRGO   = LL_ADC_INJ_TRIG_EXT_TIM2_TRGO,
        kExtTim2CH1    = LL_ADC_INJ_TRIG_EXT_TIM2_CH1,
        kExtTim3TRGO   = LL_ADC_INJ_TRIG_EXT_TIM3_TRGO,
        kExtTim3CH1    = LL_ADC_INJ_TRIG_EXT_TIM3_CH1,
        kExtTim3CH3    = LL_ADC_INJ_TRIG_EXT_TIM3_CH3,
        kExtTim3CH4    = LL_ADC_INJ_TRIG_EXT_TIM3_CH4,
        kExtTim4TRGO   = LL_ADC_INJ_TRIG_EXT_TIM4_TRGO,
        kExtTim4CH3    = LL_ADC_INJ_TRIG_EXT_TIM4_CH3,
        kExtTim4CH4    = LL_ADC_INJ_TRIG_EXT_TIM4_CH4,
        kExtTim6TRGO   = LL_ADC_INJ_TRIG_EXT_TIM6_TRGO,
        kExtTim7TRGO   = LL_ADC_INJ_TRIG_EXT_TIM7_TRGO,
        kExtTim8TRGO   = LL_ADC_INJ_TRIG_EXT_TIM8_TRGO,
        kExtTim8TRGO2  = LL_ADC_INJ_TRIG_EXT_TIM8_TRGO2,
        kExtTim8CH2    = LL_ADC_INJ_TRIG_EXT_TIM8_CH2,
        kExtTim8CH4    = LL_ADC_INJ_TRIG_EXT_TIM8_CH4,
        kExtTim15TRGO  = LL_ADC_INJ_TRIG_EXT_TIM15_TRGO,
        kExtTim16CH1   = LL_ADC_INJ_TRIG_EXT_TIM16_CH1,
        kExtTim20TRGO  = LL_ADC_INJ_TRIG_EXT_TIM20_TRGO,
        kExtTim20TRGO2 = LL_ADC_INJ_TRIG_EXT_TIM20_TRGO2,
        kExtTim20CH2   = LL_ADC_INJ_TRIG_EXT_TIM20_CH2,
        kExtTim20CH4   = LL_ADC_INJ_TRIG_EXT_TIM20_CH4,
        kExtHrtimTRG1  = LL_ADC_INJ_TRIG_EXT_HRTIM_TRG1,
        kExtHrtimTRG2  = LL_ADC_INJ_TRIG_EXT_HRTIM_TRG2,
        kExtHrtimTRG3  = LL_ADC_INJ_TRIG_EXT_HRTIM_TRG3,
        kExtHrtimTRG4  = LL_ADC_INJ_TRIG_EXT_HRTIM_TRG4,
        kExtHrtimTRG5  = LL_ADC_INJ_TRIG_EXT_HRTIM_TRG5,
        kExtHrtimTRG6  = LL_ADC_INJ_TRIG_EXT_HRTIM_TRG6,
        kExtHrtimTRG7  = LL_ADC_INJ_TRIG_EXT_HRTIM_TRG7,
        kExtHrtimTRG8  = LL_ADC_INJ_TRIG_EXT_HRTIM_TRG8,
        kExtHrtimTRG9  = LL_ADC_INJ_TRIG_EXT_HRTIM_TRG9,
        kExtHrtimTRG10 = LL_ADC_INJ_TRIG_EXT_HRTIM_TRG10,
        kExtExtiLine3  = LL_ADC_INJ_TRIG_EXT_EXTI_LINE3,
        kExtExtiLine15 = LL_ADC_INJ_TRIG_EXT_EXTI_LINE15,
        kExtLptimOut   = LL_ADC_INJ_TRIG_EXT_LPTIM_OUT,
    };

    enum class AdcInjectGroupTriggerEdge : uint32_t
    {
        kRising        = LL_ADC_INJ_TRIG_EXT_RISING,
        kFalling       = LL_ADC_INJ_TRIG_EXT_FALLING,
        kRisingFalling = LL_ADC_INJ_TRIG_EXT_RISINGFALLING
    };

    enum class AdcRegularGroupTriggerSource : uint32_t
    {
        kSoftware      = LL_ADC_REG_TRIG_SOFTWARE,
        kExtTim1TRGO   = LL_ADC_REG_TRIG_EXT_TIM1_TRGO,
        kExtTim1TRGO2  = LL_ADC_REG_TRIG_EXT_TIM1_TRGO2,
        kExtTim1CH1    = LL_ADC_REG_TRIG_EXT_TIM1_CH1,
        kExtTim1CH2    = LL_ADC_REG_TRIG_EXT_TIM1_CH2,
        kExtTim1CH3    = LL_ADC_REG_TRIG_EXT_TIM1_CH3,
        kExtTim2TRGO   = LL_ADC_REG_TRIG_EXT_TIM2_TRGO,
        kExtTim2CH1    = LL_ADC_REG_TRIG_EXT_TIM2_CH1,
        kExtTim2CH2    = LL_ADC_REG_TRIG_EXT_TIM2_CH2,
        kExtTim2CH3    = LL_ADC_REG_TRIG_EXT_TIM2_CH3,
        kExtTim3TRGO   = LL_ADC_REG_TRIG_EXT_TIM3_TRGO,
        kExtTim3CH1    = LL_ADC_REG_TRIG_EXT_TIM3_CH1,
        kExtTim3CH4    = LL_ADC_REG_TRIG_EXT_TIM3_CH4,
        kExtTim4TRGO   = LL_ADC_REG_TRIG_EXT_TIM4_TRGO,
        kExtTim4CH1    = LL_ADC_REG_TRIG_EXT_TIM4_CH1,
        kExtTim4CH4    = LL_ADC_REG_TRIG_EXT_TIM4_CH4,
        kExtTim6TRGO   = LL_ADC_REG_TRIG_EXT_TIM6_TRGO,
        kExtTim7TRGO   = LL_ADC_REG_TRIG_EXT_TIM7_TRGO,
        kExtTim8TRGO   = LL_ADC_REG_TRIG_EXT_TIM8_TRGO,
        kExtTim8TRGO2  = LL_ADC_REG_TRIG_EXT_TIM8_TRGO2,
        kExtTim8CH1    = LL_ADC_REG_TRIG_EXT_TIM8_CH1,
        kExtTim15TRGO  = LL_ADC_REG_TRIG_EXT_TIM15_TRGO,
        kExtTim20TRGO  = LL_ADC_REG_TRIG_EXT_TIM20_TRGO,
        kExtTim20TRGO2 = LL_ADC_REG_TRIG_EXT_TIM20_TRGO2,
        kExtTim20CH1   = LL_ADC_REG_TRIG_EXT_TIM20_CH1,
        kExtTim20CH2   = LL_ADC_REG_TRIG_EXT_TIM20_CH2,
        kExtTim20CH3   = LL_ADC_REG_TRIG_EXT_TIM20_CH3,
        kExtHrtimTRG1  = LL_ADC_REG_TRIG_EXT_HRTIM_TRG1,
        kExtHrtimTRG2  = LL_ADC_REG_TRIG_EXT_HRTIM_TRG2,
        kExtHrtimTRG3  = LL_ADC_REG_TRIG_EXT_HRTIM_TRG3,
        kExtHrtimTRG4  = LL_ADC_REG_TRIG_EXT_HRTIM_TRG4,
        kExtHrtimTRG5  = LL_ADC_REG_TRIG_EXT_HRTIM_TRG5,
        kExtHrtimTRG6  = LL_ADC_REG_TRIG_EXT_HRTIM_TRG6,
        kExtHrtimTRG7  = LL_ADC_REG_TRIG_EXT_HRTIM_TRG7,
        kExtHrtimTRG8  = LL_ADC_REG_TRIG_EXT_HRTIM_TRG8,
        kExtHrtimTRG9  = LL_ADC_REG_TRIG_EXT_HRTIM_TRG9,
        kExtHrtimTRG10 = LL_ADC_REG_TRIG_EXT_HRTIM_TRG10,
        kExtExtiLine11 = LL_ADC_REG_TRIG_EXT_EXTI_LINE11,
        kExtExtiLine2  = LL_ADC_REG_TRIG_EXT_EXTI_LINE2,
        kExtLptimOut   = LL_ADC_REG_TRIG_EXT_LPTIM_OUT,
    };

    enum class AdcRegularGroupTriggerEdge : uint32_t
    {
        kRising        = LL_ADC_REG_TRIG_EXT_RISING,
        kFalling       = LL_ADC_REG_TRIG_EXT_FALLING,
        kRisingFalling = LL_ADC_REG_TRIG_EXT_RISINGFALLING
    };

    enum class AdcRegularGroupDmaTransfer : uint32_t
    {
        kNone      = LL_ADC_REG_DMA_TRANSFER_NONE,
        kUnlimited = LL_ADC_REG_DMA_TRANSFER_UNLIMITED,
        kLimited   = LL_ADC_REG_DMA_TRANSFER_LIMITED
    };

    enum class AdcRegularGroupOverrunBehavior : uint32_t
    {
        kPreserve  = LL_ADC_REG_OVR_DATA_PRESERVED,
        kOverwrite = LL_ADC_REG_OVR_DATA_OVERWRITTEN,
    };

    enum class AdcRegularGroupConversionMode : uint32_t
    {
        kSingleShot = LL_ADC_REG_CONV_SINGLE,
        kContinuous = LL_ADC_REG_CONV_CONTINUOUS
    };

    enum class AdcRegularGroupOversamplingMode : uint32_t
    {
        kDiscontinuous = LL_ADC_OVS_REG_DISCONT,
        kContinuous    = LL_ADC_OVS_REG_CONT
    };

    enum class AdcOversamplingRatio : uint32_t
    {
        k2x   = LL_ADC_OVS_RATIO_2,
        k4x   = LL_ADC_OVS_RATIO_4,
        k8x   = LL_ADC_OVS_RATIO_8,
        k16x  = LL_ADC_OVS_RATIO_16,
        k32x  = LL_ADC_OVS_RATIO_32,
        k64x  = LL_ADC_OVS_RATIO_64,
        k128x = LL_ADC_OVS_RATIO_128,
        k256x = LL_ADC_OVS_RATIO_256
    };

    enum class AdcOversamplingShift : uint32_t
    {
        kNone   = LL_ADC_OVS_SHIFT_NONE,
        kDiv2   = LL_ADC_OVS_SHIFT_RIGHT_1,
        kDiv4   = LL_ADC_OVS_SHIFT_RIGHT_2,
        kDiv8   = LL_ADC_OVS_SHIFT_RIGHT_3,
        kDiv16  = LL_ADC_OVS_SHIFT_RIGHT_4,
        kDiv32  = LL_ADC_OVS_SHIFT_RIGHT_5,
        kDiv64  = LL_ADC_OVS_SHIFT_RIGHT_6,
        kDiv128 = LL_ADC_OVS_SHIFT_RIGHT_7,
        kDiv256 = LL_ADC_OVS_SHIFT_RIGHT_8
    };

    enum class AdcOversamplingScope : uint32_t
    {
        kDisable              = LL_ADC_OVS_DISABLE,
        kRegularContinued     = LL_ADC_OVS_GRP_REGULAR_CONTINUED,
        kRegularResumed       = LL_ADC_OVS_GRP_REGULAR_RESUMED,
        kInject               = LL_ADC_OVS_GRP_INJECTED,
        kInjectRegularResumed = LL_ADC_OVS_GRP_INJ_REG_RESUMED
    };

    enum class AdcChannelGroup
    {
        kRegular,
        kInject
    };

    enum class AdcChannelSampleTime
    {
        k2Cycles5   = LL_ADC_SAMPLINGTimE_2CYCLES_5,    /// 2.5 ADC clock cycles
        k6Cycles5   = LL_ADC_SAMPLINGTimE_6CYCLES_5,    /// 6.5 ADC clock cycles
        k12Cycles5  = LL_ADC_SAMPLINGTimE_12CYCLES_5,   /// 12.5 ADC clock cycles
        k24Cycles5  = LL_ADC_SAMPLINGTimE_24CYCLES_5,   /// 24.5 ADC clock cycles
        k47Cycles5  = LL_ADC_SAMPLINGTimE_47CYCLES_5,   /// 47.5 ADC clock cycles
        k92Cycles5  = LL_ADC_SAMPLINGTimE_92CYCLES_5,   /// 92.5 ADC clock cycles
        k247Cycles5 = LL_ADC_SAMPLINGTimE_247CYCLES_5,  /// 247.5 ADC clock cycles
        k640Cycles5 = LL_ADC_SAMPLINGTimE_640CYCLES_5   /// 640.5 ADC clock cycles
    };

    enum class AdcChannelInputMode
    {
        kSingleEnded  = LL_ADC_SINGLE_ENDED,       /// Single Ended Input
        kDifferential = LL_ADC_DIFFERENTIAL_ENDED  /// Differential Input
    };

    enum class AdcChannelOffsetIdx
    {
        kOffset1 = LL_ADC_OFFSET_1,  /// Offset 1
        kOffset2 = LL_ADC_OFFSET_2,  /// Offset 2
        kOffset3 = LL_ADC_OFFSET_3,  /// Offset 3
        kOffset4 = LL_ADC_OFFSET_4   /// Offset 4
    };

    // ============================================================================
    // HARDWARE TRAITS
    // ============================================================================
    template <AdcPeripheralId tkPeripheralId>
    struct AdcControllerTraits;

    template <>
    struct AdcControllerTraits<AdcPeripheralId::kAdc1>
    {
        using ClockTraitsT = AdcCommonTraits<AdcCommonId::kAdc12>;

        static inline Adc_TypeDef* const        skInstance      = Adc1;
        static constexpr IRQn_Type              skIRQn          = Adc1_2_IRQn;
        static inline constexpr DmaMuxRequestId skDmaMuxRequest = DmaMuxRequestId::kAdc1;
        static constexpr AdcCommonId            skCommonId      = AdcCommonId::kAdc12;
    };
    template <>
    struct AdcControllerTraits<AdcPeripheralId::kAdc2>
    {
        using ClockTraitsT = AdcCommonTraits<AdcCommonId::kAdc12>;

        static inline Adc_TypeDef* const        skInstance      = Adc2;
        static constexpr IRQn_Type              skIRQn          = Adc1_2_IRQn;
        static inline constexpr DmaMuxRequestId skDmaMuxRequest = DmaMuxRequestId::kAdc2;
        static constexpr AdcCommonId            skCommonId      = AdcCommonId::kAdc12;
    };

    template <>
    struct AdcControllerTraits<AdcPeripheralId::kAdc3>
    {
        using ClockTraitsT = AdcCommonTraits<AdcCommonId::kAdc345>;

        static inline Adc_TypeDef* const        skInstance      = Adc3;
        static constexpr IRQn_Type              skIRQn          = Adc3_IRQn;
        static inline constexpr DmaMuxRequestId skDmaMuxRequest = DmaMuxRequestId::kAdc3;
        static constexpr AdcCommonId            skCommonId      = AdcCommonId::kAdc345;
    };

    template <>
    struct AdcControllerTraits<AdcPeripheralId::kAdc4>
    {
        using ClockTraitsT = AdcCommonTraits<AdcCommonId::kAdc345>;

        static inline Adc_TypeDef* const        skInstance      = Adc4;
        static constexpr IRQn_Type              skIRQn          = Adc4_IRQn;
        static inline constexpr DmaMuxRequestId skDmaMuxRequest = DmaMuxRequestId::kAdc4;
        static constexpr AdcCommonId            skCommonId      = AdcCommonId::kAdc345;
    };

    template <>
    struct AdcControllerTraits<AdcPeripheralId::kAdc5>
    {
        using ClockTraitsT = AdcCommonTraits<AdcCommonId::kAdc345>;

        static inline Adc_TypeDef* const        skInstance      = Adc5;
        static constexpr IRQn_Type              skIRQn          = Adc5_IRQn;
        static inline constexpr DmaMuxRequestId skDmaMuxRequest = DmaMuxRequestId::kAdc5;
        static constexpr AdcCommonId            skCommonId      = AdcCommonId::kAdc345;
    };

    // -------------------------------------------------------------------------
    // Channel Traits
    // -------------------------------------------------------------------------
    template <AdcPeripheralId tkPeripheralId, AdcChannelId tkChannelId>
    struct AdcChannelTraits
    {
    private:
        [[nodiscard]] static consteval uint32_t get_ll_channel_id()
        {
            switch (tkChannelId)
            {
                case AdcChannelId::kChannel0:
                    return LL_ADC_CHANNEL_0;
                case AdcChannelId::kChannel1:
                    return LL_ADC_CHANNEL_1;
                case AdcChannelId::kChannel2:
                    return LL_ADC_CHANNEL_2;
                case AdcChannelId::kChannel3:
                    return LL_ADC_CHANNEL_3;
                case AdcChannelId::kChannel4:
                    return LL_ADC_CHANNEL_4;
                case AdcChannelId::kChannel5:
                    return LL_ADC_CHANNEL_5;
                case AdcChannelId::kChannel6:
                    return LL_ADC_CHANNEL_6;
                case AdcChannelId::kChannel7:
                    return LL_ADC_CHANNEL_7;
                case AdcChannelId::kChannel8:
                    return LL_ADC_CHANNEL_8;
                case AdcChannelId::kChannel9:
                    return LL_ADC_CHANNEL_9;
                case AdcChannelId::kChannel10:
                    return LL_ADC_CHANNEL_10;
                case AdcChannelId::kChannel11:
                    return LL_ADC_CHANNEL_11;
                case AdcChannelId::kChannel12:
                    return LL_ADC_CHANNEL_12;
                case AdcChannelId::kChannel13:
                    return LL_ADC_CHANNEL_13;
                case AdcChannelId::kChannel14:
                    return LL_ADC_CHANNEL_14;
                case AdcChannelId::kChannel15:
                    return LL_ADC_CHANNEL_15;
                case AdcChannelId::kChannel16:
                    return LL_ADC_CHANNEL_16;
                case AdcChannelId::kChannel17:
                    return LL_ADC_CHANNEL_17;
                case AdcChannelId::kChannel18:
                    return LL_ADC_CHANNEL_18;
                case AdcChannelId::kChannelVRefInt:
                    return LL_ADC_CHANNEL_VREFINT;
                case AdcChannelId::kChannelTempSensorAdc1:
                    return LL_ADC_CHANNEL_TEMPSENSOR_ADC1;
                case AdcChannelId::kChannelTempSensorAdc5:
                    return LL_ADC_CHANNEL_TEMPSENSOR_ADC5;
                case AdcChannelId::kChannelVBat:
                    return LL_ADC_CHANNEL_VBAT;
                case AdcChannelId::kChannelVOPAmp1:
                    return LL_ADC_CHANNEL_VOPAMP1;
                case AdcChannelId::kChannelVOPAmp2:
                    return LL_ADC_CHANNEL_VOPAMP2;
                case AdcChannelId::kChannelVOPAmp3Adc2:
                    return LL_ADC_CHANNEL_VOPAMP3_ADC2;
                case AdcChannelId::kChannelVOPAmp3Adc3:
                    return LL_ADC_CHANNEL_VOPAMP3_ADC3;
                case AdcChannelId::kChannelVOPAmp4:
                    return LL_ADC_CHANNEL_VOPAMP4;
                case AdcChannelId::kChannelVOPAmp5:
                    return LL_ADC_CHANNEL_VOPAMP5;
                case AdcChannelId::kChannelVOPAmp6:
                    return LL_ADC_CHANNEL_VOPAMP6;
                default:
                    return 0;
            }
        }

    public:
        static constexpr AdcPeripheralId skPeripheralId = tkPeripheralId;
        static constexpr AdcChannelId    skChannelId    = tkChannelId;
        static constexpr uint32_t        skChannelIdx   = static_cast<uint32_t>(tkChannelId);
        static constexpr uint32_t        skLLChannelId  = get_ll_channel_id();
    };

    // ============================================================================
    // RANK TRAITS
    // ============================================================================

    struct AdcInjectGroupTraits
    {
        static constexpr std::array<uint32_t, 4> skRankIdxToRankReg = {
            LL_ADC_INJ_RANK_1, LL_ADC_INJ_RANK_2, LL_ADC_INJ_RANK_3, LL_ADC_INJ_RANK_4};

        static uint32_t count_to_sequence_length(uint32_t count)
        {
            switch (count)
            {
                case 1:  // NOLINT(bugprone-branch-clone)
                    return LL_ADC_INJ_SEQ_SCAN_DISABLE;
                case 2:
                    return LL_ADC_INJ_SEQ_SCAN_ENABLE_2RANKS;
                case 3:
                    return LL_ADC_INJ_SEQ_SCAN_ENABLE_3RANKS;
                case 4:
                    return LL_ADC_INJ_SEQ_SCAN_ENABLE_4RANKS;
                default:
                    return LL_ADC_INJ_SEQ_SCAN_DISABLE;
            }
        }

        static constexpr uint32_t sequence_length_to_count(const uint32_t sequence_length)
        {
            switch (sequence_length)
            {
                case LL_ADC_INJ_SEQ_SCAN_DISABLE:
                    return 1;
                case LL_ADC_INJ_SEQ_SCAN_ENABLE_2RANKS:
                    return 2;
                case LL_ADC_INJ_SEQ_SCAN_ENABLE_3RANKS:
                    return 3;
                case LL_ADC_INJ_SEQ_SCAN_ENABLE_4RANKS:
                    return 4;
                default:
                    return 1;
            }
        }

        static constexpr uint32_t rank_to_rank_reg(AdcInjectChannelRank rank)
        {
            return skRankIdxToRankReg[static_cast<uint8_t>(rank) - 1];
        }

        static constexpr AdcInjectChannelRank rank_reg_to_rank(const uint32_t rank_reg)
        {
            switch (rank_reg)
            {
                case LL_ADC_INJ_RANK_1:
                    return AdcInjectChannelRank::kRank1;
                case LL_ADC_INJ_RANK_2:
                    return AdcInjectChannelRank::kRank2;
                case LL_ADC_INJ_RANK_3:
                    return AdcInjectChannelRank::kRank3;
                case LL_ADC_INJ_RANK_4:
                    return AdcInjectChannelRank::kRank4;
                default:
                    return AdcInjectChannelRank::kRank1;
            }
        }
    };

    struct AdcRegularGroupTraits
    {
        static constexpr std::array<uint32_t, 16> skRankIdxToRankReg = {LL_ADC_REG_RANK_1,
                                                                        LL_ADC_REG_RANK_2,
                                                                        LL_ADC_REG_RANK_3,
                                                                        LL_ADC_REG_RANK_4,
                                                                        LL_ADC_REG_RANK_5,
                                                                        LL_ADC_REG_RANK_6,
                                                                        LL_ADC_REG_RANK_7,
                                                                        LL_ADC_REG_RANK_8,
                                                                        LL_ADC_REG_RANK_9,
                                                                        LL_ADC_REG_RANK_10,
                                                                        LL_ADC_REG_RANK_11,
                                                                        LL_ADC_REG_RANK_12,
                                                                        LL_ADC_REG_RANK_13,
                                                                        LL_ADC_REG_RANK_14,
                                                                        LL_ADC_REG_RANK_15,
                                                                        LL_ADC_REG_RANK_16};

        static constexpr uint32_t count_to_sequence_length(const uint32_t count)
        {
            // NOLINTBEGIN(readability-magic-numbers)
            switch (count)
            {
                case 1:  // NOLINT(bugprone-branch-clone)
                    return LL_ADC_REG_SEQ_SCAN_DISABLE;
                case 2:
                    return LL_ADC_REG_SEQ_SCAN_ENABLE_2RANKS;
                case 3:
                    return LL_ADC_REG_SEQ_SCAN_ENABLE_3RANKS;
                case 4:
                    return LL_ADC_REG_SEQ_SCAN_ENABLE_4RANKS;
                case 5:
                    return LL_ADC_REG_SEQ_SCAN_ENABLE_5RANKS;
                case 6:
                    return LL_ADC_REG_SEQ_SCAN_ENABLE_6RANKS;
                case 7:
                    return LL_ADC_REG_SEQ_SCAN_ENABLE_7RANKS;
                case 8:
                    return LL_ADC_REG_SEQ_SCAN_ENABLE_8RANKS;
                case 9:
                    return LL_ADC_REG_SEQ_SCAN_ENABLE_9RANKS;
                case 10:
                    return LL_ADC_REG_SEQ_SCAN_ENABLE_10RANKS;
                case 11:
                    return LL_ADC_REG_SEQ_SCAN_ENABLE_11RANKS;
                case 12:
                    return LL_ADC_REG_SEQ_SCAN_ENABLE_12RANKS;
                case 13:
                    return LL_ADC_REG_SEQ_SCAN_ENABLE_13RANKS;
                case 14:
                    return LL_ADC_REG_SEQ_SCAN_ENABLE_14RANKS;
                case 15:
                    return LL_ADC_REG_SEQ_SCAN_ENABLE_15RANKS;
                case 16:
                    return LL_ADC_REG_SEQ_SCAN_ENABLE_16RANKS;
                default:
                    return LL_ADC_REG_SEQ_SCAN_DISABLE;
            }
            // NOLINTEND(readability-magic-numbers)
        }

        static constexpr uint32_t sequence_length_to_count(const uint32_t sequence_length)
        {
            // NOLINTBEGIN(readability-magic-numbers)
            switch (sequence_length)
            {
                case LL_ADC_REG_SEQ_SCAN_DISABLE:
                    return 1;
                case LL_ADC_REG_SEQ_SCAN_ENABLE_2RANKS:
                    return 2;
                case LL_ADC_REG_SEQ_SCAN_ENABLE_3RANKS:
                    return 3;
                case LL_ADC_REG_SEQ_SCAN_ENABLE_4RANKS:
                    return 4;
                case LL_ADC_REG_SEQ_SCAN_ENABLE_5RANKS:
                    return 5;
                case LL_ADC_REG_SEQ_SCAN_ENABLE_6RANKS:
                    return 6;
                case LL_ADC_REG_SEQ_SCAN_ENABLE_7RANKS:
                    return 7;
                case LL_ADC_REG_SEQ_SCAN_ENABLE_8RANKS:
                    return 8;
                case LL_ADC_REG_SEQ_SCAN_ENABLE_9RANKS:
                    return 9;
                case LL_ADC_REG_SEQ_SCAN_ENABLE_10RANKS:
                    return 10;
                case LL_ADC_REG_SEQ_SCAN_ENABLE_11RANKS:
                    return 11;
                case LL_ADC_REG_SEQ_SCAN_ENABLE_12RANKS:
                    return 12;
                case LL_ADC_REG_SEQ_SCAN_ENABLE_13RANKS:
                    return 13;
                case LL_ADC_REG_SEQ_SCAN_ENABLE_14RANKS:
                    return 14;
                case LL_ADC_REG_SEQ_SCAN_ENABLE_15RANKS:
                    return 15;
                case LL_ADC_REG_SEQ_SCAN_ENABLE_16RANKS:
                    return 16;
                default:
                    return 1;
            }
            // NOLINTEND(readability-magic-numbers)
        }

        static constexpr uint32_t rank_to_rank_reg(AdcRegularChannelRank rank)
        {
            return skRankIdxToRankReg[static_cast<uint8_t>(rank) - 1];
        }

        static constexpr AdcRegularChannelRank rank_reg_to_rank(uint32_t rank)
        {
            // NOLINTBEGIN(readability-magic-numbers)
            switch (rank)
            {
                case LL_ADC_REG_RANK_1:
                    return AdcRegularChannelRank::kRank1;
                case LL_ADC_REG_RANK_2:
                    return AdcRegularChannelRank::kRank2;
                case LL_ADC_REG_RANK_3:
                    return AdcRegularChannelRank::kRank3;
                case LL_ADC_REG_RANK_4:
                    return AdcRegularChannelRank::kRank4;
                case LL_ADC_REG_RANK_5:
                    return AdcRegularChannelRank::kRank5;
                case LL_ADC_REG_RANK_6:
                    return AdcRegularChannelRank::kRank6;
                case LL_ADC_REG_RANK_7:
                    return AdcRegularChannelRank::kRank7;
                case LL_ADC_REG_RANK_8:
                    return AdcRegularChannelRank::kRank8;
                case LL_ADC_REG_RANK_9:
                    return AdcRegularChannelRank::kRank9;
                case LL_ADC_REG_RANK_10:
                    return AdcRegularChannelRank::kRank10;
                case LL_ADC_REG_RANK_11:
                    return AdcRegularChannelRank::kRank11;
                case LL_ADC_REG_RANK_12:
                    return AdcRegularChannelRank::kRank12;
                case LL_ADC_REG_RANK_13:
                    return AdcRegularChannelRank::kRank13;
                case LL_ADC_REG_RANK_14:
                    return AdcRegularChannelRank::kRank14;
                case LL_ADC_REG_RANK_15:
                    return AdcRegularChannelRank::kRank15;
                case LL_ADC_REG_RANK_16:
                    return AdcRegularChannelRank::kRank16;
                default:
                    return AdcRegularChannelRank::kRank1;
            }

            // NOLINTEND(readability-magic-numbers)
        }
    };

    template <AdcInjectChannelRank tkRank>
    struct AdcInjectRankTraits
    {
        static constexpr AdcInjectChannelRank skRank = tkRank;
        static constexpr uint32_t             skReg  = AdcInjectGroupTraits::rank_to_rank_reg(tkRank);
    };

    template <AdcRegularChannelRank tkRank>
    struct AdcRegularRankTraits
    {
        static constexpr AdcRegularChannelRank skRank = tkRank;
        static constexpr uint32_t              skReg  = AdcRegularGroupTraits::rank_to_rank_reg(tkRank);
    };

    // ============================================================================
    // INTERFACE
    // ============================================================================
    using AdcValue = uint16_t;

    struct AdcRootInterface
    {
        static constexpr uint32_t calculate_async_clock_freq_hz(const uint32_t                     async_clock_freq_hz,
                                                                const AdcCommonAsyncClockPrescaler prescaler)
        {
            return AdcCommonRootInterface::calculate_async_clock_freq_hz(async_clock_freq_hz, prescaler);
        }

        static constexpr uint32_t calculate_sync_clock_freq_hz(const uint32_t                    sync_clock_freq_hz,
                                                               const AdcCommonSyncClockPrescaler prescaler)
        {
            return AdcCommonRootInterface::calculate_sync_clock_freq_hz(sync_clock_freq_hz, prescaler);
        }

        static constexpr float get_channel_sample_time_cycles(const AdcChannelSampleTime sample_time)
        {
            // NOLINTBEGIN(readability-magic-numbers)
            switch (sample_time)
            {
                case AdcChannelSampleTime::k2Cycles5:
                    return 2.5F;
                case AdcChannelSampleTime::k6Cycles5:
                    return 6.5F;
                case AdcChannelSampleTime::k12Cycles5:
                    return 12.5F;
                case AdcChannelSampleTime::k24Cycles5:
                    return 24.5F;
                case AdcChannelSampleTime::k47Cycles5:
                    return 47.5F;
                case AdcChannelSampleTime::k92Cycles5:
                    return 92.5F;
                case AdcChannelSampleTime::k247Cycles5:
                    return 247.5F;
                case AdcChannelSampleTime::k640Cycles5:
                    return 640.5F;
                default:
                    return 0.0F;
            }
            // NOLINTEND(readability-magic-numbers)
        }

        static constexpr uint32_t get_oversampling_ratio_factor(const AdcOversamplingRatio oversampling_ratio)
        {
            // NOLINTBEGIN(readability-magic-numbers)
            switch (oversampling_ratio)
            {
                case AdcOversamplingRatio::k2x:
                    return 2;
                case AdcOversamplingRatio::k4x:
                    return 4;
                case AdcOversamplingRatio::k8x:
                    return 8;
                case AdcOversamplingRatio::k16x:
                    return 16;
                case AdcOversamplingRatio::k32x:
                    return 32;
                case AdcOversamplingRatio::k64x:
                    return 64;
                case AdcOversamplingRatio::k128x:
                    return 128;
                case AdcOversamplingRatio::k256x:
                    return 256;
                default:
                    return 1;
            }
            // NOLINTEND(readability-magic-numbers)
        }

        static constexpr DurationSecondsF calculate_channel_sample_time(
            const uint32_t                            clock_freq_hz,
            const AdcChannelSampleTime                sample_time,
            const std::optional<AdcOversamplingRatio> oversampling_ratio)
        {
            const float    cycles = get_channel_sample_time_cycles(sample_time);
            const uint32_t oversampling_factor =
                oversampling_ratio.has_value() ? get_oversampling_ratio_factor(oversampling_ratio.value()) : 1;
            const float duration_s =
                (cycles * static_cast<float>(oversampling_factor)) / static_cast<float>(clock_freq_hz);
            return DurationSecondsF(duration_s);
        }
    };

    template <AdcPeripheralId tkPeripheralId>
    struct AdcControllerInterface
    {
        using ControllerTraitsT = AdcControllerTraits<tkPeripheralId>;

        // ----------------------------------------------------------------------------
        // CORE
        // ----------------------------------------------------------------------------
        static void disable()
        {
            LL_ADC_Disable(ControllerTraitsT::skInstance);
        }

        static void enable()
        {
            LL_ADC_Enable(ControllerTraitsT::skInstance);
        }

        [[nodiscard]] static bool is_enabled()
        {
            return LL_ADC_IsEnabled(ControllerTraitsT::skInstance) == 1UL;
        }

        [[nodiscard]] static bool deep_power_down_enabled()
        {
            return LL_ADC_IsDeepPowerDownEnabled(ControllerTraitsT::skInstance) == 1UL;
        }

        static void disable_deep_power_down()
        {
            LL_ADC_DisableDeepPowerDown(ControllerTraitsT::skInstance);
        }

        [[nodiscard]] static bool internal_regulator_enabled()
        {
            return LL_ADC_IsInternalRegulatorEnabled(ControllerTraitsT::skInstance) == 1UL;
        }

        static void enable_internal_regulator()
        {
            LL_ADC_EnableInternalRegulator(ControllerTraitsT::skInstance);
        }

        [[nodiscard]] static bool is_calibration_ongoing()
        {
            return LL_ADC_IsCalibrationOnGoing(ControllerTraitsT::skInstance) != 0;
        }

        static void start_calibration()
        {
            LL_ADC_StartCalibration(ControllerTraitsT::skInstance, LL_ADC_SINGLE_ENDED);
        }

        // -----------------------------------------------------------------------------
        // CONFIGURATION
        // -----------------------------------------------------------------------------
        static void set_resolution(const AdcResolution resolution)
        {
            LL_ADC_SetResolution(ControllerTraitsT::skInstance, static_cast<uint32_t>(resolution));
        }

        static void set_data_alignment(const AdcDataAlignment data_alignment)
        {
            LL_ADC_SetDataAlignment(ControllerTraitsT::skInstance, static_cast<uint32_t>(data_alignment));
        }

        static void set_low_power_mode(const AdcLowPowerMode low_power_mode)
        {
            LL_ADC_SetLowPowerMode(ControllerTraitsT::skInstance, static_cast<uint32_t>(low_power_mode));
        }

        static void set_gain_compensation(const uint16_t gain_compensation)
        {
            LL_ADC_SetGainCompensation(ControllerTraitsT::skInstance, static_cast<uint32_t>(gain_compensation));
        }

        static void set_common_sample_time(const AdcCommonSamplingTime sampling_time)
        {
            LL_ADC_SetSamplingTimeCommonConfig(ControllerTraitsT::skInstance, static_cast<uint32_t>(sampling_time));
        }

        static void set_oversampling_scope(const AdcOversamplingScope scope)
        {
            LL_ADC_SetOverSamplingScope(ControllerTraitsT::skInstance, static_cast<uint32_t>(scope));
        }

        [[nodiscard]] static AdcOversamplingScope get_oversampling_scope()
        {
            return static_cast<AdcOversamplingScope>(LL_ADC_GetOverSamplingScope(ControllerTraitsT::skInstance));
        }

        [[nodiscard]] static bool inject_group_oversampling_enabled()
        {
            switch (get_oversampling_scope())
            {
                case AdcOversamplingScope::kInject:
                case AdcOversamplingScope::kInjectRegularResumed:
                    return true;
                default:
                    return false;
            }
        }

        [[nodiscard]] static bool regular_group_oversampling_enabled()
        {
            switch (get_oversampling_scope())
            {
                case AdcOversamplingScope::kRegularContinued:
                case AdcOversamplingScope::kRegularResumed:
                case AdcOversamplingScope::kInjectRegularResumed:
                    return true;
                default:
                    return false;
            }
        }

        static void set_oversampling_ratio_shift(const AdcOversamplingRatio ratio, const AdcOversamplingShift shift)
        {
            LL_ADC_ConfigOverSamplingRatioShift(
                ControllerTraitsT::skInstance, static_cast<uint32_t>(ratio), static_cast<uint32_t>(shift));
        }

        // ----------------------------------------------------------------------------
        // CONVERSION CONTROL
        // ----------------------------------------------------------------------------
        /**
         * @brief ARM THE TRIGGER (The most important part)
         * If Trigger is Hardware (HRTIM): ADC goes into "Waiting for Trigger" state.
         * If Trigger is Software: ADC converts immediately.
         */
        static void start_inject()
        {
            LL_ADC_INJ_StartConversion(ControllerTraitsT::skInstance);
        }

        static void stop_inject()
        {
            LL_ADC_INJ_StopConversion(ControllerTraitsT::skInstance);
        }

        /**
         * @brief ARM THE TRIGGER (The most important part)
         * If Trigger is Hardware (HRTIM): ADC goes into "Waiting for Trigger" state.
         * If Trigger is Software: ADC converts immediately.
         */
        static void start_regular()
        {
            LL_ADC_REG_StartConversion(ControllerTraitsT::skInstance);
        }

        static void stop_regular()
        {
            LL_ADC_REG_StopConversion(ControllerTraitsT::skInstance);
        }

        // ----------------------------------------------------------------------------
        // RESOLUTION INFO
        // ----------------------------------------------------------------------------
        [[nodiscard]] static uint8_t get_resolution_bits()
        {
            // NOLINTBEGIN(readability-magic-numbers)
            switch (LL_ADC_GetResolution(ControllerTraitsT::skInstance))
            {
                case LL_ADC_RESOLUTION_12B:
                    return 12;
                case LL_ADC_RESOLUTION_10B:
                    return 10;
                case LL_ADC_RESOLUTION_8B:
                    return 8;
                case LL_ADC_RESOLUTION_6B:
                    return 6;
                default:
                    return 12;
            }
            // NOLINTEND(readability-magic-numbers)
        }

        [[nodiscard]] static uint32_t get_resolution_range()
        {
            const uint8_t res_bits = get_resolution_bits();
            return (1UL << res_bits) - 1UL;
        }

        [[nodiscard]] static uint8_t get_oversampling_ratio_factor()
        {
            // NOLINTBEGIN(readability-magic-numbers)
            switch (LL_ADC_GetOverSamplingRatio(ControllerTraitsT::skInstance))
            {
                case LL_ADC_OVS_RATIO_2:
                    return 2;
                case LL_ADC_OVS_RATIO_4:
                    return 4;
                case LL_ADC_OVS_RATIO_8:
                    return 8;
                case LL_ADC_OVS_RATIO_16:
                    return 16;
                case LL_ADC_OVS_RATIO_32:
                    return 32;
                case LL_ADC_OVS_RATIO_64:
                    return 64;
                case LL_ADC_OVS_RATIO_128:
                    return 128;
                case LL_ADC_OVS_RATIO_256:
                    return 256;
                default:
                    return 1;  // No oversampling
            }
            // NOLINTEND(readability-magic-numbers)
        }

        [[nodiscard]] static uint8_t get_oversampling_shift_bits()
        {
            // NOLINTBEGIN(readability-magic-numbers)
            switch (LL_ADC_GetOverSamplingShift(ControllerTraitsT::skInstance))
            {
                case LL_ADC_OVS_SHIFT_NONE:
                    return 0;
                case LL_ADC_OVS_SHIFT_RIGHT_1:
                    return 1;
                case LL_ADC_OVS_SHIFT_RIGHT_2:
                    return 2;
                case LL_ADC_OVS_SHIFT_RIGHT_3:
                    return 3;
                case LL_ADC_OVS_SHIFT_RIGHT_4:
                    return 4;
                case LL_ADC_OVS_SHIFT_RIGHT_5:
                    return 5;
                case LL_ADC_OVS_SHIFT_RIGHT_6:
                    return 6;
                case LL_ADC_OVS_SHIFT_RIGHT_7:
                    return 7;
                case LL_ADC_OVS_SHIFT_RIGHT_8:
                    return 8;
                default:
                    return 0;  // No shift
            }
            // NOLINTEND(readability-magic-numbers)
        }

        [[nodiscard]] static uint32_t get_effective_resolution_range()
        {
            // Max value of a single ADC conversion
            const uint32_t max_single_sample = get_resolution_range();

            // Max value when 'ovs_ratio' samples are accumulated
            const uint32_t max_accumulated = max_single_sample * get_oversampling_ratio_factor();

            // Hardware shifts the accumulated sum right
            const uint32_t effective_max = max_accumulated >> get_oversampling_shift_bits();

            // The ADC Data Register is 16 bits.
            // If the math results in > 65535, the hardware truncates the top bits!
            return std::min(effective_max, static_cast<uint32_t>(std::numeric_limits<uint16_t>::max()));
        }

        [[nodiscard]] static uint32_t get_inject_group_effective_resolution_range()
        {
            if (inject_group_oversampling_enabled())
            {
                return get_effective_resolution_range();
            }

            return get_resolution_range();
        }

        [[nodiscard]] static uint32_t get_regular_group_effective_resolution_range()
        {
            if (regular_group_oversampling_enabled())
            {
                return get_effective_resolution_range();
            }

            return get_resolution_range();
        }
    };

    template <AdcPeripheralId tkPeripheralId>
    struct AdcInjectGroupInterface
    {
        using ControllerTraitsT = AdcControllerTraits<tkPeripheralId>;

        static void set_queue_mode(const AdcInjectGroupQueueMode queue_mode)
        {
            LL_ADC_INJ_SetQueueMode(ControllerTraitsT::skInstance, static_cast<uint32_t>(queue_mode));
        }

        static void set_trigger_mode(const AdcInjectGroupTriggerMode trigger_mode)
        {
            LL_ADC_INJ_SetTrigAuto(ControllerTraitsT::skInstance, static_cast<uint32_t>(trigger_mode));
        }

        static void set_sequencer_discontinuity_mode(const AdcInjectGroupSequencerDiscontinuityMode discontinuity_mode)
        {
            LL_ADC_INJ_SetSequencerDiscont(ControllerTraitsT::skInstance, static_cast<uint32_t>(discontinuity_mode));
        }

        static void set_trigger_source(const AdcInjectGroupTriggerSource trigger_source)
        {
            LL_ADC_INJ_SetTriggerSource(ControllerTraitsT::skInstance, static_cast<uint32_t>(trigger_source));
        }

        static void set_trigger_edge(const AdcInjectGroupTriggerEdge trigger_edge)
        {
            LL_ADC_INJ_SetTriggerEdge(ControllerTraitsT::skInstance, static_cast<uint32_t>(trigger_edge));
        }

        static void config_trigger(const AdcInjectGroupTriggerSource trigger_source,
                                   const AdcInjectGroupTriggerEdge   trigger_edge)
        {
            set_trigger_source(trigger_source);
            if (trigger_source != AdcInjectGroupTriggerSource::kSoftware)
            {
                set_trigger_edge(trigger_edge);
            }
        }

        // Sequence Config
        static void set_sequencer_length(const uint32_t inj_count)
        {
            LL_ADC_INJ_SetSequencerLength(ControllerTraitsT::skInstance,
                                          AdcInjectGroupTraits::count_to_sequence_length(inj_count));
        }

        [[nodiscard]] static uint32_t get_sequencer_length()
        {
            return AdcInjectGroupTraits::sequence_length_to_count(
                LL_ADC_INJ_GetSequencerLength(ControllerTraitsT::skInstance));
        }

        template <AdcChannelId tkChannelId>
        static void set_sequencer_ranks(const AdcInjectChannelRank rank)
        {
            using ChannelTraitsT = AdcChannelTraits<tkPeripheralId, tkChannelId>;
            LL_ADC_INJ_SetSequencerRanks(ControllerTraitsT::skInstance,
                                         AdcInjectGroupTraits::rank_to_rank_reg(rank),
                                         ChannelTraitsT::skLLChannelId);
        }

        [[nodiscard]] static uint32_t read(const AdcInjectChannelRank rank)
        {
            return LL_ADC_INJ_ReadConversionData32(ControllerTraitsT::skInstance,
                                                   AdcInjectGroupTraits::rank_to_rank_reg(rank));
        }
    };

    template <AdcPeripheralId tkPeripheralId>
    struct AdcRegularGroupInterface
    {
        using ControllerTraitsT = AdcControllerTraits<tkPeripheralId>;

        static void set_oversampling_mode(const AdcRegularGroupOversamplingMode oversampling_mode)
        {
            LL_ADC_SetOverSamplingDiscont(ControllerTraitsT::skInstance, static_cast<uint32_t>(oversampling_mode));
        }

        static void set_trigger_source(const AdcRegularGroupTriggerSource trigger_source)
        {
            LL_ADC_REG_SetTriggerSource(ControllerTraitsT::skInstance, static_cast<uint32_t>(trigger_source));
        }

        static void set_trigger_edge(const AdcRegularGroupTriggerEdge trigger_edge)
        {
            LL_ADC_REG_SetTriggerEdge(ControllerTraitsT::skInstance, static_cast<uint32_t>(trigger_edge));
        }

        static void config_trigger(const AdcRegularGroupTriggerSource trigger_source,
                                   const AdcRegularGroupTriggerEdge   trigger_edge)
        {
            set_trigger_source(trigger_source);
            if (trigger_source != AdcRegularGroupTriggerSource::kSoftware)
            {
                set_trigger_edge(trigger_edge);
            }
        }

        static void set_overrun_behavior(const AdcRegularGroupOverrunBehavior overrun_behavior)
        {
            LL_ADC_REG_SetOverrun(ControllerTraitsT::skInstance, static_cast<uint32_t>(overrun_behavior));
        }

        static void set_conversion_mode(const AdcRegularGroupConversionMode conversion_mode)
        {
            LL_ADC_REG_SetContinuousMode(ControllerTraitsT::skInstance, static_cast<uint32_t>(conversion_mode));
        }

        // Sequence Config
        static void set_sequencer_length(const uint32_t reg_count)
        {
            LL_ADC_REG_SetSequencerLength(ControllerTraitsT::skInstance,
                                          AdcRegularGroupTraits::count_to_sequence_length(reg_count));
        }

        [[nodiscard]] static uint32_t get_sequencer_length()
        {
            return AdcRegularGroupTraits::sequence_length_to_count(
                LL_ADC_REG_GetSequencerLength(ControllerTraitsT::skInstance));
        }

        template <AdcChannelId tkChannelId>
        static void set_sequencer_ranks(const AdcRegularChannelRank rank)
        {
            using ChannelTraitsT = AdcChannelTraits<tkPeripheralId, tkChannelId>;
            LL_ADC_REG_SetSequencerRanks(ControllerTraitsT::skInstance,
                                         AdcRegularGroupTraits::rank_to_rank_reg(rank),
                                         ChannelTraitsT::skLLChannelId);
        }

        static void set_dma_transfer(const AdcRegularGroupDmaTransfer dma_transfer)
        {
            LL_ADC_REG_SetDmaTransfer(ControllerTraitsT::skInstance, static_cast<uint32_t>(dma_transfer));
        }
    };

    template <AdcPeripheralId tkPeripheralId, AdcChannelId tkChannelId>
    struct AdcChannelInterface
    {
        static constexpr AdcPeripheralId skPeripheralId = tkPeripheralId;
        static constexpr AdcChannelId    skChannelId    = tkChannelId;

        using ControllerTraitsT = AdcControllerTraits<tkPeripheralId>;
        using ChannelTraitsT    = AdcChannelTraits<tkPeripheralId, tkChannelId>;

        static void set_sampling_time(const AdcChannelSampleTime sample_time)
        {
            LL_ADC_SetChannelSamplingTime(
                ControllerTraitsT::skInstance, ChannelTraitsT::skLLChannelId, static_cast<uint32_t>(sample_time));
        }

        static void set_input_mode(const AdcChannelInputMode input_mode)
        {
            LL_ADC_SetChannelSingleDiff(
                ControllerTraitsT::skInstance, ChannelTraitsT::skLLChannelId, static_cast<uint32_t>(input_mode));
        }

        static void config_offset(const AdcChannelOffsetIdx offset_idx, const uint32_t offset_value)
        {
            LL_ADC_SetOffset(ControllerTraitsT::skInstance,
                             static_cast<uint32_t>(offset_idx),
                             ChannelTraitsT::skLLChannelId,
                             offset_value);
        }
    };

}  // namespace valle::platform