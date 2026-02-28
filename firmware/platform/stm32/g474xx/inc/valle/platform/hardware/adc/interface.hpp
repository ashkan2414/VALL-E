#pragma once

#include "stm32g4xx_ll_adc.h"
#include "valle/platform/hardware/ADC/id.hpp"
#include "valle/platform/hardware/adc_clk.hpp"


namespace valle
{
    // ============================================================================
    // ENUMERATIONS
    // ============================================================================

    enum class ADCResolution : uint32_t
    {
        k12Bit = LL_ADC_RESOLUTION_12B,
        k10Bit = LL_ADC_RESOLUTION_10B,
        k8Bit  = LL_ADC_RESOLUTION_8B,
        k6Bit  = LL_ADC_RESOLUTION_6B,
    };

    enum class ADCDataAlignment : uint32_t
    {
        kRight = LL_ADC_DATA_ALIGN_RIGHT,
        kLeft  = LL_ADC_DATA_ALIGN_LEFT,
    };

    enum class ADCLowPowerMode : uint32_t
    {
        kNone     = LL_ADC_LP_MODE_NONE,  /// No low power
        kAutoWait = LL_ADC_LP_AUTOWAIT,   /// Auto wait between conversions
    };

    enum class ADCInjectGroupTriggerSource : uint32_t
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

    enum class ADCInjectGroupTriggerEdge : uint32_t
    {
        kRising        = LL_ADC_INJ_TRIG_EXT_RISING,
        kFalling       = LL_ADC_INJ_TRIG_EXT_FALLING,
        kRisingFalling = LL_ADC_INJ_TRIG_EXT_RISINGFALLING
    };

    enum class ADCRegularGroupTriggerSource : uint32_t
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

    enum class ADCRegularGroupTriggerEdge : uint32_t
    {
        kRising        = LL_ADC_REG_TRIG_EXT_RISING,
        kFalling       = LL_ADC_REG_TRIG_EXT_FALLING,
        kRisingFalling = LL_ADC_REG_TRIG_EXT_RISINGFALLING
    };

    enum class ADCRegularGroupDMATransfer : uint32_t
    {
        kNone      = LL_ADC_REG_DMA_TRANSFER_NONE,
        kUnlimited = LL_ADC_REG_DMA_TRANSFER_UNLIMITED,
        kLimited   = LL_ADC_REG_DMA_TRANSFER_LIMITED
    };

    enum class ADCRegularGroupOverrunBehavior : uint32_t
    {
        kPreserve  = LL_ADC_REG_OVR_DATA_PRESERVED,
        kOverwrite = LL_ADC_REG_OVR_DATA_OVERWRITTEN,
    };

    enum class ADCRegularGroupConversionMode : uint32_t
    {
        kSingleShot = LL_ADC_REG_CONV_SINGLE,
        kContinuous = LL_ADC_REG_CONV_CONTINUOUS
    };

    enum class ADCRegularGroupOversamplingMode : uint32_t
    {
        kDiscontinuous = LL_ADC_OVS_REG_DISCONT,
        kContinuous    = LL_ADC_OVS_REG_CONT
    };

    enum class ADCOversamplingRatio : uint32_t
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

    enum class ADCOversamplingShiftBits : uint32_t
    {
        kNone   = LL_ADC_OVS_SHIFT_NONE,
        kRight1 = LL_ADC_OVS_SHIFT_RIGHT_1,
        kRight2 = LL_ADC_OVS_SHIFT_RIGHT_2,
        kRight3 = LL_ADC_OVS_SHIFT_RIGHT_3,
        kRight4 = LL_ADC_OVS_SHIFT_RIGHT_4,
        kRight5 = LL_ADC_OVS_SHIFT_RIGHT_5,
        kRight6 = LL_ADC_OVS_SHIFT_RIGHT_6,
        kRight7 = LL_ADC_OVS_SHIFT_RIGHT_7,
        kRight8 = LL_ADC_OVS_SHIFT_RIGHT_8
    };

    enum class ADCOversamplingScope : uint32_t
    {
        kDisable              = LL_ADC_OVS_DISABLE,
        kRegularContinued     = LL_ADC_OVS_GRP_REGULAR_CONTINUED,
        kRegularResumed       = LL_ADC_OVS_GRP_REGULAR_RESUMED,
        kInject               = LL_ADC_OVS_GRP_INJECTED,
        kInjectRegularResumed = LL_ADC_OVS_GRP_INJ_REG_RESUMED
    };

    enum class ADCChannelGroup
    {
        kRegular,
        kInject
    };

    enum class ADCChannelSampleTime
    {
        k2Cycles5   = LL_ADC_SAMPLINGTIME_2CYCLES_5,    /// 2.5 ADC clock cycles
        k6Cycles5   = LL_ADC_SAMPLINGTIME_6CYCLES_5,    /// 6.5 ADC clock cycles
        k12Cycles5  = LL_ADC_SAMPLINGTIME_12CYCLES_5,   /// 12.5 ADC clock cycles
        k24Cycles5  = LL_ADC_SAMPLINGTIME_24CYCLES_5,   /// 24.5 ADC clock cycles
        k47Cycles5  = LL_ADC_SAMPLINGTIME_47CYCLES_5,   /// 47.5 ADC clock cycles
        k92Cycles5  = LL_ADC_SAMPLINGTIME_92CYCLES_5,   /// 92.5 ADC clock cycles
        k247Cycles5 = LL_ADC_SAMPLINGTIME_247CYCLES_5,  /// 247.5 ADC clock cycles
        k640Cycles5 = LL_ADC_SAMPLINGTIME_640CYCLES_5   /// 640.5 ADC clock cycles
    };

    enum class ADCChannelInputMode
    {
        kSingleEnded  = LL_ADC_SINGLE_ENDED,       /// Single Ended Input
        kDifferential = LL_ADC_DIFFERENTIAL_ENDED  /// Differential Input
    };

    enum class ADCChannelOffsetIdx
    {
        kOffset1 = LL_ADC_OFFSET_1,  /// Offset 1
        kOffset2 = LL_ADC_OFFSET_2,  /// Offset 2
        kOffset3 = LL_ADC_OFFSET_3,  /// Offset 3
        kOffset4 = LL_ADC_OFFSET_4   /// Offset 4
    };

    // ============================================================================
    // HARDWARE TRAITS
    // ============================================================================
    template <ADCControllerID tkControllerID>
        requires(kValidADCControllerID<tkControllerID>)
    struct ADCControllerTraits;

    template <>
    struct ADCControllerTraits<1>
    {
        using ClockTraitsT = ADCClockTraits<ADCClockID::kADC12>;

        static inline ADC_TypeDef* const        skInstance      = ADC1;
        static constexpr IRQn_Type              skIRQn          = ADC1_2_IRQn;
        static inline constexpr DMAMuxRequestID skDMAMuxRequest = DMAMuxRequestID::kADC1;
    };
    template <>
    struct ADCControllerTraits<2>
    {
        using ClockTraitsT = ADCClockTraits<ADCClockID::kADC12>;

        static inline ADC_TypeDef* const        skInstance      = ADC2;
        static constexpr IRQn_Type              skIRQn          = ADC1_2_IRQn;
        static inline constexpr DMAMuxRequestID skDMAMuxRequest = DMAMuxRequestID::kADC2;
    };

    template <>
    struct ADCControllerTraits<3>
    {
        using ClockTraitsT = ADCClockTraits<ADCClockID::kADC345>;

        static inline ADC_TypeDef* const        skInstance      = ADC3;
        static constexpr IRQn_Type              skIRQn          = ADC3_IRQn;
        static inline constexpr DMAMuxRequestID skDMAMuxRequest = DMAMuxRequestID::kADC3;
    };

    template <>
    struct ADCControllerTraits<4>
    {
        using ClockTraitsT = ADCClockTraits<ADCClockID::kADC345>;

        static inline ADC_TypeDef* const        skInstance      = ADC4;
        static constexpr IRQn_Type              skIRQn          = ADC4_IRQn;
        static inline constexpr DMAMuxRequestID skDMAMuxRequest = DMAMuxRequestID::kADC4;
    };

    template <>
    struct ADCControllerTraits<5>
    {
        using ClockTraitsT = ADCClockTraits<ADCClockID::kADC345>;

        static inline ADC_TypeDef* const        skInstance      = ADC5;
        static constexpr IRQn_Type              skIRQn          = ADC5_IRQn;
        static inline constexpr DMAMuxRequestID skDMAMuxRequest = DMAMuxRequestID::kADC5;
    };

    // ============================================================================
    // RANK TRAITS
    // ============================================================================

    struct ADCInjectGroupTraits
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

        static constexpr uint32_t rank_to_rank_reg(uint8_t rank)
        {
            return skRankIdxToRankReg[rank - 1];
        }

        static constexpr uint8_t rank_reg_to_rank(const uint32_t rank_reg)
        {
            switch (rank_reg)
            {
                case LL_ADC_INJ_RANK_1:
                    return 1;
                case LL_ADC_INJ_RANK_2:
                    return 2;
                case LL_ADC_INJ_RANK_3:
                    return 3;
                case LL_ADC_INJ_RANK_4:
                    return 4;
                default:
                    return 0;
            }
        }
    };

    struct ADCRegularGroupTraits
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

        static constexpr uint32_t rank_to_rank_reg(uint8_t rank)
        {
            return skRankIdxToRankReg[rank - 1];
        }

        static constexpr uint8_t rank_reg_to_rank(uint32_t rank)
        {
            // NOLINTBEGIN(readability-magic-numbers)
            switch (rank)
            {
                case LL_ADC_REG_RANK_1:
                    return 1;
                case LL_ADC_REG_RANK_2:
                    return 2;
                case LL_ADC_REG_RANK_3:
                    return 3;
                case LL_ADC_REG_RANK_4:
                    return 4;
                case LL_ADC_REG_RANK_5:
                    return 5;
                case LL_ADC_REG_RANK_6:
                    return 6;
                case LL_ADC_REG_RANK_7:
                    return 7;
                case LL_ADC_REG_RANK_8:
                    return 8;
                case LL_ADC_REG_RANK_9:
                    return 9;
                case LL_ADC_REG_RANK_10:
                    return 10;
                case LL_ADC_REG_RANK_11:
                    return 11;
                case LL_ADC_REG_RANK_12:
                    return 12;
                case LL_ADC_REG_RANK_13:
                    return 13;
                case LL_ADC_REG_RANK_14:
                    return 14;
                case LL_ADC_REG_RANK_15:
                    return 15;
                case LL_ADC_REG_RANK_16:
                    return 16;
                default:
                    return 0;
            }

            // NOLINTEND(readability-magic-numbers)
        }
    };

    template <uint8_t tkRank>
    struct ADCInjectRankTraits
    {
        static constexpr uint32_t skRank = tkRank;
        static constexpr uint32_t skReg  = ADCInjectGroupTraits::rank_to_rank_reg(tkRank);
    };

    template <uint8_t tkRank>
    struct ADCRegularRankTraits
    {
        static constexpr uint32_t skRank = tkRank;
        static constexpr uint32_t skReg  = ADCRegularGroupTraits::rank_to_rank_reg(tkRank);
    };

}  // namespace valle