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
        kDefault                   = LL_ADC_SAMPLINGTIME_COMMON_DEFAULT,
        k3P5CyclesReplace2P5Cycles = LL_ADC_SAMPLINGTIME_COMMON_3C5_REPL_2C5,
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

    enum class AdcRegularGroupSequencerDiscontinuity : uint32_t
    {
        kDisable = LL_ADC_REG_SEQ_DISCONT_DISABLE,
        k1Rank   = LL_ADC_REG_SEQ_DISCONT_1RANK,
        k2Ranks  = LL_ADC_REG_SEQ_DISCONT_2RANKS,
        k3Ranks  = LL_ADC_REG_SEQ_DISCONT_3RANKS,
        k4Ranks  = LL_ADC_REG_SEQ_DISCONT_4RANKS,
        k5Ranks  = LL_ADC_REG_SEQ_DISCONT_5RANKS,
        k6Ranks  = LL_ADC_REG_SEQ_DISCONT_6RANKS,
        k7Ranks  = LL_ADC_REG_SEQ_DISCONT_7RANKS,
        k8Ranks  = LL_ADC_REG_SEQ_DISCONT_8RANKS,
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

    enum class AdcRegularGroupSamplingMode : uint32_t
    {
        kNormal            = LL_ADC_REG_SAMPLING_MODE_NORMAL,
        kBulb              = LL_ADC_REG_SAMPLING_MODE_BULB,
        kTriggerControlled = LL_ADC_REG_SAMPLING_MODE_TRIGGER_CONTROLED,
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
        k2Cycles5   = LL_ADC_SAMPLINGTIME_2CYCLES_5,    /// 2.5 ADC clock cycles
        k6Cycles5   = LL_ADC_SAMPLINGTIME_6CYCLES_5,    /// 6.5 ADC clock cycles
        k12Cycles5  = LL_ADC_SAMPLINGTIME_12CYCLES_5,   /// 12.5 ADC clock cycles
        k24Cycles5  = LL_ADC_SAMPLINGTIME_24CYCLES_5,   /// 24.5 ADC clock cycles
        k47Cycles5  = LL_ADC_SAMPLINGTIME_47CYCLES_5,   /// 47.5 ADC clock cycles
        k92Cycles5  = LL_ADC_SAMPLINGTIME_92CYCLES_5,   /// 92.5 ADC clock cycles
        k247Cycles5 = LL_ADC_SAMPLINGTIME_247CYCLES_5,  /// 247.5 ADC clock cycles
        k640Cycles5 = LL_ADC_SAMPLINGTIME_640CYCLES_5   /// 640.5 ADC clock cycles
    };

    enum class AdcChannelInputMode
    {
        kSingleEnded  = LL_ADC_SINGLE_ENDED,       /// Single Ended Input
        kDifferential = LL_ADC_DIFFERENTIAL_ENDED  /// Differential Input
    };

    enum class AdcControllerInterruptSource : uint8_t
    {
        kReady = 0,
        kRegularEndOfConversion,
        kRegularEndOfSequence,
        kRegularEndOfSampling,
        kInjectEndOfConversion,
        kInjectEndOfSequence,
        kInjectContextQueueOverflow,
        kOverrun,
        kAnalogWatchdog1,
        kAnalogWatchdog2,
        kAnalogWatchdog3,
    };

    enum class AdcDmaRegister : uint32_t
    {
        kRegularData      = LL_ADC_DMA_REG_REGULAR_DATA,
        kRegularDataMulti = LL_ADC_DMA_REG_REGULAR_DATA_MULTI,
    };

    enum class AdcDifferentialMode : uint32_t
    {
        kSingleEnded        = LL_ADC_SINGLE_ENDED,
        kDifferential       = LL_ADC_DIFFERENTIAL_ENDED,
        kSingleDifferential = LL_ADC_BOTH_SINGLE_DIFF_ENDED,
    };

    // ============================================================================
    // ROOT INTERFACE
    // ============================================================================
    struct AdcTraits
    {
        static constexpr size_t skNumChannels     = magic_enum::enum_count<AdcChannelId>();
        static constexpr size_t skNumInjChannels  = 4;
        static constexpr size_t skNumRegChannels  = 16;
        static constexpr size_t skNumOffsetBlocks = 4;



        static constexpr std::array<uint32_t, skNumInjChannels> skInjectRankLLIdTable = {
            LL_ADC_INJ_RANK_1, LL_ADC_INJ_RANK_2, LL_ADC_INJ_RANK_3, LL_ADC_INJ_RANK_4};

        static constexpr std::array<uint32_t, skNumRegChannels> skRegularRankLLIdTable = {LL_ADC_REG_RANK_1,
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

        static constexpr std::array<uint32_t, skNumOffsetBlocks> skOffsetBlockLLIdTable = {
            LL_ADC_OFFSET_1, LL_ADC_OFFSET_2, LL_ADC_OFFSET_3, LL_ADC_OFFSET_4};

        [[nodiscard]] static constexpr uint8_t controller_id_to_index(const AdcControllerId controller_id)
        {
            static_assert(static_cast<uint8_t>(AdcControllerId::kAdc1) == 1,
                          "AdcControllerId enum values must start at 1");
            return static_cast<uint8_t>(controller_id) - 1;
        }

        [[nodiscard]] static constexpr uint8_t controller_id_to_number(const AdcControllerId controller_id)
        {
            static_assert(static_cast<uint8_t>(AdcControllerId::kAdc1) == 1,
                          "AdcControllerId enum values must start at 1");
            return static_cast<uint8_t>(controller_id);
        }

        [[nodiscard]] static constexpr uint8_t channel_id_to_index(const AdcChannelId channel_id)
        {
            static_assert(static_cast<uint32_t>(AdcChannelId::kChannel0) == 0,
                          "AdcChannelId enum values must start at 0");
            return static_cast<uint8_t>(channel_id);
        }

        [[nodiscard]] static constexpr std::optional<uint8_t> channel_id_to_number(const AdcChannelId channel_id)
        {
            switch (channel_id)
            {
                case AdcChannelId::kChannel0:
                case AdcChannelId::kChannel1:
                case AdcChannelId::kChannel2:
                case AdcChannelId::kChannel3:
                case AdcChannelId::kChannel4:
                case AdcChannelId::kChannel5:
                case AdcChannelId::kChannel6:
                case AdcChannelId::kChannel7:
                case AdcChannelId::kChannel8:
                case AdcChannelId::kChannel9:
                case AdcChannelId::kChannel10:
                case AdcChannelId::kChannel11:
                case AdcChannelId::kChannel12:
                case AdcChannelId::kChannel13:
                case AdcChannelId::kChannel14:
                case AdcChannelId::kChannel15:
                case AdcChannelId::kChannel16:
                case AdcChannelId::kChannel17:
                case AdcChannelId::kChannel18:
                    return static_cast<uint8_t>(static_cast<uint32_t>(channel_id) & 0xFF);
                default:
                    return std::nullopt;  // Invalid channel number
            }
        }

        [[nodiscard]] static constexpr uint32_t channel_id_to_ll_id(const AdcChannelId channel_id)
        {
            return skChannelLLIdTable[AdcTraits::channel_id_to_index(channel_id)];
        }

        [[nodiscard]] static constexpr std::optional<AdcChannelId> channel_ll_id_to_id(const uint32_t ll_channel_id)
        {
            for (size_t i = 0; i < skNumChannels; ++i)
            {
                if (skChannelLLIdTable[i] == ll_channel_id)
                {
                    return static_cast<AdcChannelId>(i);
                }
            }
            return std::nullopt;  // Invalid LL channel ID
        }

        [[nodiscard]] static constexpr uint8_t inject_group_rank_to_index(const AdcInjectChannelRank rank)
        {
            static_assert(static_cast<uint8_t>(AdcInjectChannelRank::kRank1) == 1,
                          "AdcInjectChannelRank enum values must start at 1");
            return static_cast<uint8_t>(rank) - 1;
        }

        [[nodiscard]] static constexpr uint8_t inject_group_rank_to_number(const AdcInjectChannelRank rank)
        {
            static_assert(static_cast<uint8_t>(AdcInjectChannelRank::kRank1) == 1,
                          "AdcInjectChannelRank enum values must start at 1");
            return static_cast<uint8_t>(rank);
        }

        [[nodiscard]] static constexpr uint32_t inject_group_rank_to_ll_id(const AdcInjectChannelRank rank)
        {
            return skInjectRankLLIdTable[inject_group_rank_to_index(rank)];
        }

        [[nodiscard]] static constexpr std::optional<AdcInjectChannelRank> inject_group_ll_id_to_rank(
            const uint32_t rank_reg)
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
                    return std::nullopt;  // Invalid rank
            }
        }

        [[nodiscard]] static constexpr uint8_t regular_group_rank_to_index(const AdcRegularChannelRank rank)
        {
            static_assert(static_cast<uint8_t>(AdcRegularChannelRank::kRank1) == 1,
                          "AdcRegularChannelRank enum values must start at 1");
            return static_cast<uint8_t>(rank) - 1;
        }

        [[nodiscard]] static constexpr uint8_t regular_group_rank_to_number(const AdcRegularChannelRank rank)
        {
            static_assert(static_cast<uint8_t>(AdcRegularChannelRank::kRank1) == 1,
                          "AdcRegularChannelRank enum values must start at 1");
            return static_cast<uint8_t>(rank);
        }

        [[nodiscard]] static constexpr uint32_t regular_group_rank_to_ll_id(const AdcRegularChannelRank rank)
        {
            return skRegularRankLLIdTable[regular_group_rank_to_index(rank)];
        }

        [[nodiscard]] static constexpr std::optional<AdcRegularChannelRank> regular_group_ll_id_to_rank(
            const uint32_t rank)
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
                    return std::nullopt;  // Invalid rank
            }

            // NOLINTEND(readability-magic-numbers)
        }

        [[nodiscard]] static constexpr uint8_t offset_block_id_to_number(const AdcOffsetBlockId block)
        {
            static_assert(static_cast<uint8_t>(AdcOffsetBlockId::kOffset1) == 1,
                          "AdcOffsetBlockId enum values must start at 1");
            return static_cast<uint8_t>(block);
        }

        [[nodiscard]] static constexpr uint8_t offset_block_id_to_index(const AdcOffsetBlockId block)
        {
            static_assert(static_cast<uint8_t>(AdcOffsetBlockId::kOffset1) == 1,
                          "AdcOffsetBlockId enum values must start at 1");
            return static_cast<uint8_t>(block) - 1;
        }

        [[nodiscard]] static constexpr uint32_t offset_block_id_to_ll_id(const AdcOffsetBlockId block)
        {
            return skOffsetBlockLLIdTable[offset_block_id_to_index(block)];
        }

        static constexpr uint32_t calculate_async_clock_freq_hz(const uint32_t                     async_clock_freq_hz,
                                                                const AdcCommonAsyncClockPrescaler prescaler)
        {
            return AdcCommonRootTraits::calculate_async_clock_freq_hz(async_clock_freq_hz, prescaler);
        }

        static constexpr uint32_t calculate_sync_clock_freq_hz(const uint32_t                    sync_clock_freq_hz,
                                                               const AdcCommonSyncClockPrescaler prescaler)
        {
            return AdcCommonRootTraits::calculate_sync_clock_freq_hz(sync_clock_freq_hz, prescaler);
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
                    expect(false, "Invalid sample time");  // Invalid sample time
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
                    expect(false, "Invalid oversampling ratio");  // Invalid oversampling ratio
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

        static constexpr uint32_t inject_group_count_to_count_ll_id(const uint32_t count)
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
                    expect(false, "Invalid inject group count");
                    return LL_ADC_INJ_SEQ_SCAN_DISABLE;
            }
        }

        static constexpr uint32_t inject_group_count_ll_id_to_count(const uint32_t count_ll_id)
        {
            switch (count_ll_id)
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
                    expect(false, "Invalid inject group sequence length");
                    return 1;
            }
        }

        static constexpr uint32_t regular_group_count_to_count_ll_id(const uint32_t count)
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
                    expect(false, "Invalid regular group count");
                    return LL_ADC_REG_SEQ_SCAN_DISABLE;
            }
            // NOLINTEND(readability-magic-numbers)
        }

        static constexpr uint32_t regular_group_count_ll_id_to_count(const uint32_t count_ll_id)
        {
            // NOLINTBEGIN(readability-magic-numbers)
            switch (count_ll_id)
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
                    expect(false, "Invalid regular group sequence length");
                    return 1;
            }
            // NOLINTEND(readability-magic-numbers)
        }
    };

    // ============================================================================
    // HARDWARE TRAITS
    // ============================================================================
    template <AdcControllerSpec tkControllerSpec>
    struct AdcControllerTraits;

    template <>
    struct AdcControllerTraits<kAdc1ControllerSpec>
    {
        static constexpr AdcControllerSpec skControllerSpec   = kAdc1ControllerSpec;
        static constexpr AdcControllerId   skControllerId     = skControllerSpec.controller_id;
        static constexpr uint8_t           skControllerNumber = AdcTraits::controller_id_to_number(skControllerId);
        static inline ADC_TypeDef *const   skInstance         = ADC1;
        static constexpr IRQn_Type         skIRQn             = ADC1_2_IRQn;
        static constexpr DmaMuxRequestId   skDmaMuxRequest    = DmaMuxRequestId::kAdc1;
        static constexpr AdcCommonControllerSpec skCommonSpec = kAdc12CommonControllerSpec;
    };
    template <>
    struct AdcControllerTraits<kAdc2ControllerSpec>
    {
        static constexpr AdcControllerSpec skControllerSpec   = kAdc2ControllerSpec;
        static constexpr AdcControllerId   skControllerId     = skControllerSpec.controller_id;
        static constexpr uint8_t           skControllerNumber = AdcTraits::controller_id_to_number(skControllerId);
        static inline ADC_TypeDef *const   skInstance         = ADC2;
        static constexpr IRQn_Type         skIRQn             = ADC1_2_IRQn;
        static constexpr DmaMuxRequestId   skDmaMuxRequest    = DmaMuxRequestId::kAdc2;
        static constexpr AdcCommonControllerSpec skCommonSpec = kAdc12CommonControllerSpec;
    };

    template <>
    struct AdcControllerTraits<kAdc3ControllerSpec>
    {
        static constexpr AdcControllerSpec skControllerSpec   = kAdc3ControllerSpec;
        static constexpr AdcControllerId   skControllerId     = skControllerSpec.controller_id;
        static constexpr uint8_t           skControllerNumber = AdcTraits::controller_id_to_number(skControllerId);
        static inline ADC_TypeDef *const   skInstance         = ADC3;
        static constexpr IRQn_Type         skIRQn             = ADC3_IRQn;
        static constexpr DmaMuxRequestId   skDmaMuxRequest    = DmaMuxRequestId::kAdc3;
        static constexpr AdcCommonControllerSpec skCommonSpec = kAdc345CommonControllerSpec;
    };

    template <>
    struct AdcControllerTraits<kAdc4ControllerSpec>
    {
        static constexpr AdcControllerSpec skControllerSpec   = kAdc4ControllerSpec;
        static constexpr AdcControllerId   skControllerId     = skControllerSpec.controller_id;
        static constexpr uint8_t           skControllerNumber = AdcTraits::controller_id_to_number(skControllerId);
        static inline ADC_TypeDef *const   skInstance         = ADC4;
        static constexpr IRQn_Type         skIRQn             = ADC4_IRQn;
        static constexpr DmaMuxRequestId   skDmaMuxRequest    = DmaMuxRequestId::kAdc4;
        static constexpr AdcCommonControllerSpec skCommonSpec = kAdc345CommonControllerSpec;
    };

    template <>
    struct AdcControllerTraits<kAdc5ControllerSpec>
    {
        static constexpr AdcControllerSpec skControllerSpec   = kAdc5ControllerSpec;
        static constexpr AdcControllerId   skControllerId     = skControllerSpec.controller_id;
        static constexpr uint8_t           skControllerNumber = AdcTraits::controller_id_to_number(skControllerId);
        static inline ADC_TypeDef *const   skInstance         = ADC5;
        static constexpr IRQn_Type         skIRQn             = ADC5_IRQn;
        static constexpr DmaMuxRequestId   skDmaMuxRequest    = DmaMuxRequestId::kAdc5;
        static constexpr AdcCommonControllerSpec skCommonSpec = kAdc345CommonControllerSpec;
    };

    // -------------------------------------------------------------------------
    // Channel Traits
    // -------------------------------------------------------------------------
    template <AdcChannelSpec tkChannelSpec>
    struct AdcChannelTraits
    {
        static constexpr AdcChannelSpec         skChannelSpec   = tkChannelSpec;
        static constexpr AdcControllerId        skControllerId  = tkChannelSpec.controller_id;
        static constexpr AdcChannelId           skChannelId     = tkChannelSpec.channel_id;
        static constexpr uint8_t                skChannelIdx    = AdcTraits::channel_id_to_index(skChannelId);
        static constexpr std::optional<uint8_t> skChannelNumber = AdcTraits::channel_id_to_number(skChannelId);
        static constexpr uint32_t               skLLChannelId   = AdcTraits::channel_id_to_ll_id(skChannelId);
    };

    // ---------------------------------------------------------------------------
    // RANK TRAITS
    // ---------------------------------------------------------------------------
    template <AdcInjectChannelRankSpec tkRankSpec>
    struct AdcInjectRankTraits
    {
        static constexpr AdcInjectChannelRankSpec skRankSpec   = tkRankSpec;
        static constexpr AdcInjectChannelRank     skRank       = tkRankSpec.rank;
        static constexpr uint8_t                  skRankNumber = AdcTraits::inject_group_rank_to_number(skRank);
        static constexpr uint32_t                 skRankLLId   = AdcTraits::inject_group_rank_to_ll_id(skRank);
    };

    template <AdcRegularChannelRankSpec tkRankSpec>
    struct AdcRegularRankTraits
    {
        static constexpr AdcRegularChannelRankSpec skRankSpec   = tkRankSpec;
        static constexpr AdcRegularChannelRank     skRank       = tkRankSpec.rank;
        static constexpr uint8_t                   skRankNumber = AdcTraits::regular_group_rank_to_number(skRank);
        static constexpr uint32_t                  skRankLLId   = AdcTraits::regular_group_rank_to_ll_id(skRank);
    };

    // ---------------------------------------------------------------------------
    // OFFSET BLOCK TRAITS
    // ---------------------------------------------------------------------------
    template <AdcOffsetBlockSpec tkOffsetBlockSpec>
    struct AdcOffsetBlockTraits
    {
        static constexpr AdcOffsetBlockSpec skOffsetBlockSpec = tkOffsetBlockSpec;
        static constexpr AdcOffsetBlockId   skBlockId         = tkOffsetBlockSpec.block_id;
        static constexpr uint8_t            skBlockNumber     = AdcTraits::offset_block_id_to_number(skBlockId);
        static constexpr uint32_t           skBlockLLId       = AdcTraits::offset_block_id_to_ll_id(skBlockId);
    };

    // ============================================================================
    // INTERFACE
    // ============================================================================
    using AdcValue = uint16_t;

    template <AdcControllerSpec tkControllerSpec>
    struct AdcControllerInterface : public AdcTraits
    {
        using ControllerTraitsT = AdcControllerTraits<tkControllerSpec>;

        // ----------------------------------------------------------------------------
        // CORE
        // ----------------------------------------------------------------------------
        void disable() const
        {
            LL_ADC_Disable(ControllerTraitsT::skInstance);
        }

        void enable() const
        {
            LL_ADC_Enable(ControllerTraitsT::skInstance);
        }

        [[nodiscard]] bool is_enabled() const
        {
            return LL_ADC_IsEnabled(ControllerTraitsT::skInstance) == 1UL;
        }

        [[nodiscard]] bool deep_power_down_enabled() const
        {
            return LL_ADC_IsDeepPowerDownEnabled(ControllerTraitsT::skInstance) == 1UL;
        }

        void disable_deep_power_down() const
        {
            LL_ADC_DisableDeepPowerDown(ControllerTraitsT::skInstance);
        }

        [[nodiscard]] bool internal_regulator_enabled() const
        {
            return LL_ADC_IsInternalRegulatorEnabled(ControllerTraitsT::skInstance) == 1UL;
        }

        void enable_internal_regulator() const
        {
            LL_ADC_EnableInternalRegulator(ControllerTraitsT::skInstance);
        }

        [[nodiscard]] bool is_calibration_ongoing() const
        {
            return LL_ADC_IsCalibrationOnGoing(ControllerTraitsT::skInstance) != 0;
        }

        void start_calibration() const
        {
            LL_ADC_StartCalibration(ControllerTraitsT::skInstance, LL_ADC_SINGLE_ENDED);
        }

        // -----------------------------------------------------------------------------
        // CONFIGURATION
        // -----------------------------------------------------------------------------
        void set_resolution(const AdcResolution resolution) const
        {
            LL_ADC_SetResolution(ControllerTraitsT::skInstance, static_cast<uint32_t>(resolution));
        }

        void set_data_alignment(const AdcDataAlignment data_alignment) const
        {
            LL_ADC_SetDataAlignment(ControllerTraitsT::skInstance, static_cast<uint32_t>(data_alignment));
        }

        [[nodiscard]] AdcDataAlignment get_data_alignment() const
        {
            return static_cast<AdcDataAlignment>(LL_ADC_GetDataAlignment(ControllerTraitsT::skInstance));
        }

        void set_low_power_mode(const AdcLowPowerMode low_power_mode) const
        {
            LL_ADC_SetLowPowerMode(ControllerTraitsT::skInstance, static_cast<uint32_t>(low_power_mode));
        }

        [[nodiscard]] AdcLowPowerMode get_low_power_mode() const
        {
            return static_cast<AdcLowPowerMode>(LL_ADC_GetLowPowerMode(ControllerTraitsT::skInstance));
        }

        void set_calibration_factor(const AdcDifferentialMode single_diff, const uint32_t calibration_factor) const
        {
            LL_ADC_SetCalibrationFactor(
                ControllerTraitsT::skInstance, static_cast<uint32_t>(single_diff), calibration_factor);
        }

        [[nodiscard]] uint32_t get_calibration_factor(const AdcDifferentialMode single_diff) const
        {
            return LL_ADC_GetCalibrationFactor(ControllerTraitsT::skInstance, static_cast<uint32_t>(single_diff));
        }

        void set_gain_compensation(const uint16_t gain_compensation) const
        {
            LL_ADC_SetGainCompensation(ControllerTraitsT::skInstance, static_cast<uint32_t>(gain_compensation));
        }

        [[nodiscard]] uint32_t get_gain_compensation() const
        {
            return LL_ADC_GetGainCompensation(ControllerTraitsT::skInstance);
        }

        void set_common_sample_time(const AdcCommonSamplingTime sampling_time) const
        {
            LL_ADC_SetSamplingTimeCommonConfig(ControllerTraitsT::skInstance, static_cast<uint32_t>(sampling_time));
        }

        [[nodiscard]] AdcCommonSamplingTime get_sampling_time_common_config() const
        {
            return static_cast<AdcCommonSamplingTime>(
                LL_ADC_GetSamplingTimeCommonConfig(ControllerTraitsT::skInstance));
        }

        void set_oversampling_scope(const AdcOversamplingScope scope) const
        {
            LL_ADC_SetOverSamplingScope(ControllerTraitsT::skInstance, static_cast<uint32_t>(scope));
        }

        [[nodiscard]] AdcOversamplingScope get_oversampling_scope() const
        {
            return static_cast<AdcOversamplingScope>(LL_ADC_GetOverSamplingScope(ControllerTraitsT::skInstance));
        }

        [[nodiscard]] bool inject_group_oversampling_enabled() const
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

        [[nodiscard]] bool regular_group_oversampling_enabled() const
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

        void set_oversampling_ratio_shift(const AdcOversamplingRatio ratio, const AdcOversamplingShift shift) const
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
        void start_inject() const
        {
            LL_ADC_INJ_StartConversion(ControllerTraitsT::skInstance);
        }

        void stop_inject() const
        {
            LL_ADC_INJ_StopConversion(ControllerTraitsT::skInstance);
        }

        /**
         * @brief ARM THE TRIGGER (The most important part)
         * If Trigger is Hardware (HRTIM): ADC goes into "Waiting for Trigger" state.
         * If Trigger is Software: ADC converts immediately.
         */
        void start_regular() const
        {
            LL_ADC_REG_StartConversion(ControllerTraitsT::skInstance);
        }

        void stop_regular() const
        {
            LL_ADC_REG_StopConversion(ControllerTraitsT::skInstance);
        }

        // ----------------------------------------------------------------------------
        // RESOLUTION INFO
        // ----------------------------------------------------------------------------
        [[nodiscard]] uint8_t get_resolution_bits() const
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

        [[nodiscard]] uint32_t get_resolution_range() const
        {
            const uint8_t res_bits = get_resolution_bits();
            return (1UL << res_bits) - 1UL;
        }

        [[nodiscard]] uint8_t get_oversampling_ratio_factor() const
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

        [[nodiscard]] uint8_t get_oversampling_shift_bits() const
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

        [[nodiscard]] uint32_t get_effective_resolution_range() const
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

        [[nodiscard]] uint32_t get_inject_group_effective_resolution_range() const
        {
            if (inject_group_oversampling_enabled())
            {
                return get_effective_resolution_range();
            }

            return get_resolution_range();
        }

        [[nodiscard]] uint32_t get_regular_group_effective_resolution_range() const
        {
            if (regular_group_oversampling_enabled())
            {
                return get_effective_resolution_range();
            }

            return get_resolution_range();
        }

        [[nodiscard]] AdcInjectGroupTriggerSource inject_group_get_trigger_source() const
        {
            return static_cast<AdcInjectGroupTriggerSource>(LL_ADC_INJ_GetTriggerSource(ControllerTraitsT::skInstance));
        }

        [[nodiscard]] bool inject_group_is_trigger_source_sw_start() const
        {
            return LL_ADC_INJ_IsTriggerSourceSWStart(ControllerTraitsT::skInstance) != 0;
        }

        [[nodiscard]] AdcInjectGroupTriggerEdge inject_group_get_trigger_edge() const
        {
            return static_cast<AdcInjectGroupTriggerEdge>(LL_ADC_INJ_GetTriggerEdge(ControllerTraitsT::skInstance));
        }

        [[nodiscard]] uint32_t inject_group_get_sequencer_discontinuity_mode() const
        {
            return LL_ADC_INJ_GetSequencerDiscont(ControllerTraitsT::skInstance);
        }

        void inject_group_config_queue_context(const uint32_t channel,
                                               const uint32_t rank,
                                               const uint32_t sample_time,
                                               const uint32_t single_diff,
                                               const uint32_t trigger_source,
                                               const uint32_t trigger_edge,
                                               const uint32_t queue_mode) const
        {
            LL_ADC_INJ_ConfigQueueContext(ControllerTraitsT::skInstance,
                                          channel,
                                          rank,
                                          sample_time,
                                          single_diff,
                                          trigger_source,
                                          trigger_edge,
                                          queue_mode);
        }

        // -----------------------------------------------------------------------------
        // Inject Group Control
        // -----------------------------------------------------------------------------
        void inject_group_set_queue_mode(const AdcInjectGroupQueueMode queue_mode) const
        {
            LL_ADC_INJ_SetQueueMode(ControllerTraitsT::skInstance, static_cast<uint32_t>(queue_mode));
        }

        [[nodiscard]] AdcInjectGroupQueueMode inject_group_get_queue_mode() const
        {
            return static_cast<AdcInjectGroupQueueMode>(LL_ADC_INJ_GetQueueMode(ControllerTraitsT::skInstance));
        }

        void inject_group_set_trigger_mode(const AdcInjectGroupTriggerMode trigger_mode) const
        {
            LL_ADC_INJ_SetTrigAuto(ControllerTraitsT::skInstance, static_cast<uint32_t>(trigger_mode));
        }

        [[nodiscard]] AdcInjectGroupTriggerMode inject_group_get_trigger_mode() const
        {
            return static_cast<AdcInjectGroupTriggerMode>(LL_ADC_INJ_GetTrigAuto(ControllerTraitsT::skInstance));
        }

        void inject_group_set_sequencer_discontinuity_mode(
            const AdcInjectGroupSequencerDiscontinuityMode discontinuity_mode) const
        {
            LL_ADC_INJ_SetSequencerDiscont(ControllerTraitsT::skInstance, static_cast<uint32_t>(discontinuity_mode));
        }

        void inject_group_set_trigger_source(const AdcInjectGroupTriggerSource trigger_source) const
        {
            LL_ADC_INJ_SetTriggerSource(ControllerTraitsT::skInstance, static_cast<uint32_t>(trigger_source));
        }

        void inject_group_set_trigger_edge(const AdcInjectGroupTriggerEdge trigger_edge) const
        {
            LL_ADC_INJ_SetTriggerEdge(ControllerTraitsT::skInstance, static_cast<uint32_t>(trigger_edge));
        }

        static void inject_group_config_trigger(const AdcInjectGroupTriggerSource trigger_source,
                                                const AdcInjectGroupTriggerEdge   trigger_edge)
        {
            set_trigger_source(trigger_source);
            if (trigger_source != AdcInjectGroupTriggerSource::kSoftware)
            {
                set_trigger_edge(trigger_edge);
            }
        }

        // Sequence Config
        void inject_group_set_sequencer_length(const uint32_t inj_count) const
        {
            LL_ADC_INJ_SetSequencerLength(ControllerTraitsT::skInstance,
                                          AdcTraits::inject_group_count_to_count_ll_id(inj_count));
        }

        [[nodiscard]] uint32_t inject_group_get_sequencer_length() const
        {
            return AdcTraits::inject_group_count_ll_id_to_count(
                LL_ADC_INJ_GetSequencerLength(ControllerTraitsT::skInstance));
        }

        template <AdcChannelId tkChannelId>
        void inject_group_set_sequencer_ranks(const AdcInjectChannelRank rank) const
        {
            using ChannelTraitsT = AdcChannelTraits<tkControllerId, tkChannelId>;
            LL_ADC_INJ_SetSequencerRanks(ControllerTraitsT::skInstance,
                                         AdcTraits::inject_group_rank_to_ll_id(rank),
                                         ChannelTraitsT::skLLChannelId);
        }

        [[nodiscard]] AdcChannelId inject_group_get_sequencer_ranks(const AdcInjectChannelRank rank) const
        {
            return AdcTraits::channel_ll_id_to_id(LL_ADC_INJ_GetSequencerRanks(
                ControllerTraitsT::skInstance, AdcTraits::inject_group_rank_to_ll_id(rank)));
        }

        [[nodiscard]] uint32_t inject_group_read(const AdcInjectChannelRank rank) const
        {
            return LL_ADC_INJ_ReadConversionData32(ControllerTraitsT::skInstance,
                                                   AdcTraits::inject_group_rank_to_ll_id(rank));
        }

        // -----------------------------------------------------------------------------
        // Regular Group Control
        // -----------------------------------------------------------------------------
        void regular_group_set_oversampling_mode(const AdcRegularGroupOversamplingMode oversampling_mode) const
        {
            LL_ADC_SetOverSamplingDiscont(ControllerTraitsT::skInstance, static_cast<uint32_t>(oversampling_mode));
        }

        void regular_group_set_trigger_source(const AdcRegularGroupTriggerSource trigger_source) const
        {
            LL_ADC_REG_SetTriggerSource(ControllerTraitsT::skInstance, static_cast<uint32_t>(trigger_source));
        }

        [[nodiscard]] AdcRegularGroupTriggerSource regular_group_get_trigger_source() const
        {
            return static_cast<AdcRegularGroupTriggerSource>(
                LL_ADC_REG_GetTriggerSource(ControllerTraitsT::skInstance));
        }

        void regular_group_set_trigger_edge(const AdcRegularGroupTriggerEdge trigger_edge) const
        {
            LL_ADC_REG_SetTriggerEdge(ControllerTraitsT::skInstance, static_cast<uint32_t>(trigger_edge));
        }

        [[nodiscard]] AdcRegularGroupTriggerEdge regular_group_get_trigger_edge() const
        {
            return static_cast<AdcRegularGroupTriggerEdge>(LL_ADC_REG_GetTriggerEdge(ControllerTraitsT::skInstance));
        }

        static void regular_group_config_trigger(const AdcRegularGroupTriggerSource trigger_source,
                                                 const AdcRegularGroupTriggerEdge   trigger_edge)
        {
            regular_group_set_trigger_source(trigger_source);
            if (trigger_source != AdcRegularGroupTriggerSource::kSoftware)
            {
                regular_group_set_trigger_edge(trigger_edge);
            }
        }

        void regular_group_set_overrun_behavior(const AdcRegularGroupOverrunBehavior overrun_behavior) const
        {
            LL_ADC_REG_SetOverrun(ControllerTraitsT::skInstance, static_cast<uint32_t>(overrun_behavior));
        }

        [[nodiscard]] AdcRegularGroupOverrunBehavior regular_group_get_overrun_behavior() const
        {
            return static_cast<AdcRegularGroupOverrunBehavior>(LL_ADC_REG_GetOverrun(ControllerTraitsT::skInstance));
        }

        void regular_group_set_conversion_mode(const AdcRegularGroupConversionMode conversion_mode) const
        {
            LL_ADC_REG_SetContinuousMode(ControllerTraitsT::skInstance, static_cast<uint32_t>(conversion_mode));
        }

        [[nodiscard]] AdcRegularGroupConversionMode regular_group_get_continuous_mode() const
        {
            return static_cast<AdcRegularGroupConversionMode>(
                LL_ADC_REG_GetContinuousMode(ControllerTraitsT::skInstance));
        }

        void regular_group_set_sequencer_discontinuity_mode(
            const AdcRegularGroupSequencerDiscontinuity seq_discont) const
        {
            LL_ADC_REG_SetSequencerDiscont(ControllerTraitsT::skInstance, static_cast<uint32_t>(seq_discont));
        }

        [[nodiscard]] AdcRegularGroupSequencerDiscontinuity regular_group_get_sequencer_discontinuity_mode() const
        {
            return static_cast<AdcRegularGroupSequencerDiscontinuity>(
                LL_ADC_REG_GetSequencerDiscont(ControllerTraitsT::skInstance));
        }

        void regular_group_set_sampling_mode(const AdcRegularGroupSamplingMode sampling_mode) const
        {
            LL_ADC_REG_SetSamplingMode(ControllerTraitsT::skInstance, static_cast<uint32_t>(sampling_mode));
        }

        [[nodiscard]] AdcRegularGroupSamplingMode regular_group_get_sampling_mode() const
        {
            return static_cast<AdcRegularGroupSamplingMode>(LL_ADC_REG_GetSamplingMode(ControllerTraitsT::skInstance));
        }

        [[nodiscard]] bool regular_group_is_trigger_source_sw_start() const
        {
            return LL_ADC_REG_IsTriggerSourceSWStart(ControllerTraitsT::skInstance) != 0;
        }

        // Sequence Config
        void regular_group_set_sequencer_length(const uint32_t reg_count) const
        {
            LL_ADC_REG_SetSequencerLength(ControllerTraitsT::skInstance,
                                          AdcTraits::regular_group_count_to_count_ll_id(reg_count));
        }

        [[nodiscard]] uint32_t regular_group_get_sequencer_length() const
        {
            return AdcTraits::regular_group_count_ll_id_to_count(
                LL_ADC_REG_GetSequencerLength(ControllerTraitsT::skInstance));
        }

        template <AdcChannelId tkChannelId>
        void regular_group_set_sequencer_ranks(const AdcRegularChannelRank rank) const
        {
            using ChannelTraitsT = AdcChannelTraits<tkControllerId, tkChannelId>;
            LL_ADC_REG_SetSequencerRanks(ControllerTraitsT::skInstance,
                                         AdcTraits::regular_group_rank_to_ll_id(rank),
                                         ChannelTraitsT::skLLChannelId);
        }

        [[nodiscard]] AdcChannelId regular_group_get_sequencer_ranks(const AdcRegularChannelRank rank) const
        {
            return AdcTraits::channel_ll_id_to_id(LL_ADC_REG_GetSequencerRanks(
                ControllerTraitsT::skInstance, AdcTraits::regular_group_rank_to_ll_id(rank)));
        }

        void regular_group_set_dma_transfer(const AdcRegularGroupDmaTransfer dma_transfer) const
        {
            LL_ADC_REG_SetDmaTransfer(ControllerTraitsT::skInstance, static_cast<uint32_t>(dma_transfer));
        }

        [[nodiscard]] AdcRegularGroupDmaTransfer regular_group_get_dma_transfer() const
        {
            return static_cast<AdcRegularGroupDmaTransfer>(LL_ADC_REG_GetDMATransfer(ControllerTraitsT::skInstance));
        }

        // -----------------------------------------------------------------------------
        // Interrupts and Flags
        // -----------------------------------------------------------------------------
        [[nodiscard]] bool is_active_flag_ready() const
        {
            return LL_ADC_IsActiveFlag_ADRDY(ControllerTraitsT::skInstance) != 0;
        }

        void clear_ready_flag() const
        {
            LL_ADC_ClearFlag_ADRDY(ControllerTraitsT::skInstance);
        }

        void enable_ready_interrupt() const
        {
            LL_ADC_EnableIT_ADRDY(ControllerTraitsT::skInstance);
        }

        void disable_ready_interrupt() const
        {
            LL_ADC_DisableIT_ADRDY(ControllerTraitsT::skInstance);
        }

        [[nodiscard]] bool is_ready_interrupt_enabled() const
        {
            return LL_ADC_IsEnabledIT_ADRDY(ControllerTraitsT::skInstance) != 0;
        }

        [[nodiscard]] bool is_active_flag_eoc() const
        {
            return LL_ADC_IsActiveFlag_EOC(ControllerTraitsT::skInstance) != 0;
        }

        void clear_eoc_flag() const
        {
            LL_ADC_ClearFlag_EOC(ControllerTraitsT::skInstance);
        }

        void enable_eoc_interrupt() const
        {
            LL_ADC_EnableIT_EOC(ControllerTraitsT::skInstance);
        }

        void disable_eoc_interrupt() const
        {
            LL_ADC_DisableIT_EOC(ControllerTraitsT::skInstance);
        }

        [[nodiscard]] bool is_eoc_interrupt_enabled() const
        {
            return LL_ADC_IsEnabledIT_EOC(ControllerTraitsT::skInstance) != 0;
        }

        [[nodiscard]] bool is_active_flag_eos() const
        {
            return LL_ADC_IsActiveFlag_EOS(ControllerTraitsT::skInstance) != 0;
        }

        void clear_eos_flag() const
        {
            LL_ADC_ClearFlag_EOS(ControllerTraitsT::skInstance);
        }

        void enable_eos_interrupt() const
        {
            LL_ADC_EnableIT_EOS(ControllerTraitsT::skInstance);
        }

        void disable_eos_interrupt() const
        {
            LL_ADC_DisableIT_EOS(ControllerTraitsT::skInstance);
        }

        [[nodiscard]] bool is_eos_interrupt_enabled() const
        {
            return LL_ADC_IsEnabledIT_EOS(ControllerTraitsT::skInstance) != 0;
        }

        [[nodiscard]] bool is_active_flag_ovr() const
        {
            return LL_ADC_IsActiveFlag_OVR(ControllerTraitsT::skInstance) != 0;
        }

        void clear_ovr_flag() const
        {
            LL_ADC_ClearFlag_OVR(ControllerTraitsT::skInstance);
        }

        void enable_ovr_interrupt() const
        {
            LL_ADC_EnableIT_OVR(ControllerTraitsT::skInstance);
        }

        void disable_ovr_interrupt() const
        {
            LL_ADC_DisableIT_OVR(ControllerTraitsT::skInstance);
        }

        [[nodiscard]] bool is_ovr_interrupt_enabled() const
        {
            return LL_ADC_IsEnabledIT_OVR(ControllerTraitsT::skInstance) != 0;
        }

        [[nodiscard]] bool is_active_flag_eosmp() const
        {
            return LL_ADC_IsActiveFlag_EOSMP(ControllerTraitsT::skInstance) != 0;
        }

        void clear_eosmp_flag() const
        {
            LL_ADC_ClearFlag_EOSMP(ControllerTraitsT::skInstance);
        }

        void enable_eosmp_interrupt() const
        {
            LL_ADC_EnableIT_EOSMP(ControllerTraitsT::skInstance);
        }

        void disable_eosmp_interrupt() const
        {
            LL_ADC_DisableIT_EOSMP(ControllerTraitsT::skInstance);
        }

        [[nodiscard]] bool is_eosmp_interrupt_enabled() const
        {
            return LL_ADC_IsEnabledIT_EOSMP(ControllerTraitsT::skInstance) != 0;
        }

        [[nodiscard]] bool is_active_flag_jeoc() const
        {
            return LL_ADC_IsActiveFlag_JEOC(ControllerTraitsT::skInstance) != 0;
        }

        void clear_jeoc_flag() const
        {
            LL_ADC_ClearFlag_JEOC(ControllerTraitsT::skInstance);
        }

        void enable_jeoc_interrupt() const
        {
            LL_ADC_EnableIT_JEOC(ControllerTraitsT::skInstance);
        }

        void disable_jeoc_interrupt() const
        {
            LL_ADC_DisableIT_JEOC(ControllerTraitsT::skInstance);
        }

        [[nodiscard]] bool is_jeoc_interrupt_enabled() const
        {
            return LL_ADC_IsEnabledIT_JEOC(ControllerTraitsT::skInstance) != 0;
        }

        [[nodiscard]] bool is_active_flag_jeos() const
        {
            return LL_ADC_IsActiveFlag_JEOS(ControllerTraitsT::skInstance) != 0;
        }

        void clear_jeos_flag() const
        {
            LL_ADC_ClearFlag_JEOS(ControllerTraitsT::skInstance);
        }

        void enable_jeos_interrupt() const
        {
            LL_ADC_EnableIT_JEOS(ControllerTraitsT::skInstance);
        }

        void disable_jeos_interrupt() const
        {
            LL_ADC_DisableIT_JEOS(ControllerTraitsT::skInstance);
        }

        [[nodiscard]] bool is_jeos_interrupt_enabled() const
        {
            return LL_ADC_IsEnabledIT_JEOS(ControllerTraitsT::skInstance) != 0;
        }

        [[nodiscard]] bool is_active_flag_jqovf() const
        {
            return LL_ADC_IsActiveFlag_JQOVF(ControllerTraitsT::skInstance) != 0;
        }

        void clear_jqovf_flag() const
        {
            LL_ADC_ClearFlag_JQOVF(ControllerTraitsT::skInstance);
        }

        void enable_jqovf_interrupt() const
        {
            LL_ADC_EnableIT_JQOVF(ControllerTraitsT::skInstance);
        }

        void disable_jqovf_interrupt() const
        {
            LL_ADC_DisableIT_JQOVF(ControllerTraitsT::skInstance);
        }

        [[nodiscard]] bool is_jqovf_interrupt_enabled() const
        {
            return LL_ADC_IsEnabledIT_JQOVF(ControllerTraitsT::skInstance) != 0;
        }

        [[nodiscard]] bool is_active_flag_awd1() const
        {
            return LL_ADC_IsActiveFlag_AWD1(ControllerTraitsT::skInstance) != 0;
        }

        void clear_awd1_flag() const
        {
            LL_ADC_ClearFlag_AWD1(ControllerTraitsT::skInstance);
        }

        void enable_awd1_interrupt() const
        {
            LL_ADC_EnableIT_AWD1(ControllerTraitsT::skInstance);
        }

        void disable_awd1_interrupt() const
        {
            LL_ADC_DisableIT_AWD1(ControllerTraitsT::skInstance);
        }

        [[nodiscard]] bool is_awd1_interrupt_enabled() const
        {
            return LL_ADC_IsEnabledIT_AWD1(ControllerTraitsT::skInstance) != 0;
        }

        [[nodiscard]] bool is_active_flag_awd2() const
        {
            return LL_ADC_IsActiveFlag_AWD2(ControllerTraitsT::skInstance) != 0;
        }

        void clear_awd2_flag() const
        {
            LL_ADC_ClearFlag_AWD2(ControllerTraitsT::skInstance);
        }

        void enable_awd2_interrupt() const
        {
            LL_ADC_EnableIT_AWD2(ControllerTraitsT::skInstance);
        }

        void disable_awd2_interrupt() const
        {
            LL_ADC_DisableIT_AWD2(ControllerTraitsT::skInstance);
        }

        [[nodiscard]] bool is_awd2_interrupt_enabled() const
        {
            return LL_ADC_IsEnabledIT_AWD2(ControllerTraitsT::skInstance) != 0;
        }

        [[nodiscard]] bool is_active_flag_awd3() const
        {
            return LL_ADC_IsActiveFlag_AWD3(ControllerTraitsT::skInstance) != 0;
        }

        void clear_awd3_flag() const
        {
            LL_ADC_ClearFlag_AWD3(ControllerTraitsT::skInstance);
        }

        void enable_awd3_interrupt() const
        {
            LL_ADC_EnableIT_AWD3(ControllerTraitsT::skInstance);
        }

        void disable_awd3_interrupt() const
        {
            LL_ADC_DisableIT_AWD3(ControllerTraitsT::skInstance);
        }

        [[nodiscard]] bool is_awd3_interrupt_enabled() const
        {
            return LL_ADC_IsEnabledIT_AWD3(ControllerTraitsT::skInstance) != 0;
        }

        // -----------------------------------------------------------------------------
        // MISC
        // -----------------------------------------------------------------------------
        [[nodiscard]] uint32_t get_dma_reg_addr(const AdcDmaRegister dma_register) const
        {
            return LL_ADC_DMA_GetRegAddr(ControllerTraitsT::skInstance, static_cast<uint32_t>(dma_register));
        }

        void set_analog_wd_monit_channels(const uint32_t awdy, const uint32_t awd_channel_group) const
        {
            LL_ADC_SetAnalogWDMonitChannels(ControllerTraitsT::skInstance, awdy, awd_channel_group);
        }

        [[nodiscard]] uint32_t get_analog_wd_monit_channels(const uint32_t awdy) const
        {
            return LL_ADC_GetAnalogWDMonitChannels(ControllerTraitsT::skInstance, awdy);
        }

        void config_analog_wd_thresholds(const uint32_t awdy,
                                         const uint32_t awd_threshold_high_value,
                                         const uint32_t awd_threshold_low_value) const
        {
            LL_ADC_ConfigAnalogWDThresholds(
                ControllerTraitsT::skInstance, awdy, awd_threshold_high_value, awd_threshold_low_value);
        }

        void set_analog_wd_thresholds(const uint32_t awdy, const uint32_t awd_thresholds_high_low) const
        {
            LL_ADC_SetAnalogWDThresholds(ControllerTraitsT::skInstance, awdy, awd_thresholds_high_low);
        }

        [[nodiscard]] uint32_t get_analog_wd_thresholds(const uint32_t awdy) const
        {
            return LL_ADC_GetAnalogWDThresholds(ControllerTraitsT::skInstance, awdy);
        }

        void set_awd_filtering_configuration(const uint32_t awdy, const uint32_t filtering_config) const
        {
            LL_ADC_SetAWDFilteringConfiguration(ControllerTraitsT::skInstance, awdy, filtering_config);
        }

        [[nodiscard]] uint32_t get_awd_filtering_configuration(const uint32_t awdy) const
        {
            return LL_ADC_GetAWDFilteringConfiguration(ControllerTraitsT::skInstance, awdy);
        }

        [[nodiscard]] uint32_t get_oversampling_discont() const
        {
            return LL_ADC_GetOverSamplingDiscont(ControllerTraitsT::skInstance);
        }

        void enable_deep_power_down() const
        {
            LL_ADC_EnableDeepPowerDown(ControllerTraitsT::skInstance);
        }

        void disable_internal_regulator() const
        {
            LL_ADC_DisableInternalRegulator(ControllerTraitsT::skInstance);
        }

        [[nodiscard]] bool is_disable_ongoing() const
        {
            return LL_ADC_IsDisableOngoing(ControllerTraitsT::skInstance) != 0;
        }

        [[nodiscard]] bool regular_group_is_conversion_ongoing() const
        {
            return LL_ADC_REG_IsConversionOngoing(ControllerTraitsT::skInstance) != 0;
        }

        [[nodiscard]] bool regular_group_is_stop_conversion_ongoing() const
        {
            return LL_ADC_REG_IsStopConversionOngoing(ControllerTraitsT::skInstance) != 0;
        }

        void regular_group_start_sampling_phase() const
        {
            LL_ADC_REG_StartSamplingPhase(ControllerTraitsT::skInstance);
        }

        void regular_group_stop_sampling_phase() const
        {
            LL_ADC_REG_StopSamplingPhase(ControllerTraitsT::skInstance);
        }

        [[nodiscard]] uint32_t regular_group_read_conversion_data32() const
        {
            return LL_ADC_REG_ReadConversionData32(ControllerTraitsT::skInstance);
        }

        [[nodiscard]] uint16_t regular_group_read_conversion_data12() const
        {
            return LL_ADC_REG_ReadConversionData12(ControllerTraitsT::skInstance);
        }

        [[nodiscard]] uint16_t regular_group_read_conversion_data10() const
        {
            return LL_ADC_REG_ReadConversionData10(ControllerTraitsT::skInstance);
        }

        [[nodiscard]] uint8_t regular_group_read_conversion_data8() const
        {
            return LL_ADC_REG_ReadConversionData8(ControllerTraitsT::skInstance);
        }

        [[nodiscard]] uint8_t regular_group_read_conversion_data6() const
        {
            return LL_ADC_REG_ReadConversionData6(ControllerTraitsT::skInstance);
        }

        [[nodiscard]] bool inject_group_is_conversion_ongoing() const
        {
            return LL_ADC_INJ_IsConversionOngoing(ControllerTraitsT::skInstance) != 0;
        }

        [[nodiscard]] bool inject_group_is_stop_conversion_ongoing() const
        {
            return LL_ADC_INJ_IsStopConversionOngoing(ControllerTraitsT::skInstance) != 0;
        }

        [[nodiscard]] uint16_t inject_group_read_conversion_data12(const uint32_t rank) const
        {
            return LL_ADC_INJ_ReadConversionData12(ControllerTraitsT::skInstance, rank);
        }

        [[nodiscard]] uint16_t inject_group_read_conversion_data10(const uint32_t rank) const
        {
            return LL_ADC_INJ_ReadConversionData10(ControllerTraitsT::skInstance, rank);
        }

        [[nodiscard]] uint8_t inject_group_read_conversion_data8(const uint32_t rank) const
        {
            return LL_ADC_INJ_ReadConversionData8(ControllerTraitsT::skInstance, rank);
        }

        [[nodiscard]] uint8_t inject_group_read_conversion_data6(const uint32_t rank) const
        {
            return LL_ADC_INJ_ReadConversionData6(ControllerTraitsT::skInstance, rank);
        }
    };

    template <AdcChannelSpec tkChannelSpec>
    struct AdcChannelInterface
    {
        static constexpr AdcChannelSpec skChannelSpec = tkChannelSpec;
        using ControllerTraitsT                       = AdcControllerTraits<tkChannelSpec.controller_spec()>;
        using ChannelTraitsT                          = AdcChannelTraits<tkChannelSpec>;

        void set_sampling_time(const AdcChannelSampleTime sample_time) const
        {
            LL_ADC_SetChannelSamplingTime(
                ControllerTraitsT::skInstance, ChannelTraitsT::skLLChannelId, static_cast<uint32_t>(sample_time));
        }

        [[nodiscard]] AdcChannelSampleTime get_sampling_time() const
        {
            return static_cast<AdcChannelSampleTime>(
                LL_ADC_GetChannelSamplingTime(ControllerTraitsT::skInstance, ChannelTraitsT::skLLChannelId));
        }

        void set_input_mode(const AdcChannelInputMode input_mode) const
        {
            LL_ADC_SetChannelSingleDiff(
                ControllerTraitsT::skInstance, ChannelTraitsT::skLLChannelId, static_cast<uint32_t>(input_mode));
        }

        [[nodiscard]] AdcChannelInputMode get_input_mode() const
        {
            return static_cast<AdcChannelInputMode>(
                LL_ADC_GetChannelSingleDiff(ControllerTraitsT::skInstance, ChannelTraitsT::skLLChannelId));
        }
    };

    template <AdcInjectChannelRankSpec tkRankSpec>
    struct AdcInjectChannelInterface
    {
        static constexpr AdcInjectChannelRankSpec skRankSpec = tkRankSpec;
        using ControllerTraitsT                              = AdcControllerTraits<tkRankSpec.controller_spec()>;
        using RankTraitsT                                    = AdcInjectRankTraits<tkRankSpec>;
    };

    template <AdcRegularChannelRankSpec tkRankSpec>
    struct AdcRegularChannelInterface
    {
    };

    template <AdcOffsetBlockSpec tkOffsetBlockSpec>
    struct AdcOffsetBlockInterface
    {
        static constexpr AdcOffsetBlockSpec skOffsetBlockSpec = tkOffsetBlockSpec;
        using ControllerTraitsT  = AdcControllerTraits<tkOffsetBlockSpec.controller_spec()>;
        using OffsetBlockTraitsT = AdcOffsetBlockTraits<tkOffsetBlockSpec>;

        void set_offset(const AdcChannelId channel_id, const uint32_t offset_value) const
        {
            LL_ADC_SetOffset(ControllerTraitsT::skInstance,
                             OffsetBlockTraitsT::skLLBlockId,
                             AdcTraits::channel_id_to_ll_id(channel_id),
                             offset_value);
        }

        [[nodiscard]] AdcChannelId get_offset_channel() const
        {
            return AdcTraits::channel_ll_id_to_id(
                LL_ADC_GetOffsetChannel(ControllerTraitsT::skInstance, OffsetBlockTraitsT::skLLBlockId));
        }

        [[nodiscard]] uint32_t get_offset_level() const
        {
            return LL_ADC_GetOffsetLevel(ControllerTraitsT::skInstance, OffsetBlockTraitsT::skLLBlockId);
        }

        void set_offset_state(const bool enabled) const
        {
            LL_ADC_SetOffsetState(ControllerTraitsT::skInstance,
                                  OffsetBlockTraitsT::skLLBlockId,
                                  enabled ? LL_ADC_OFFSET_ENABLE : LL_ADC_OFFSET_DISABLE);
        }

        [[nodiscard]] bool get_offset_state() const
        {
            return LL_ADC_GetOffsetState(ControllerTraitsT::skInstance, OffsetBlockTraitsT::skLLBlockId) ==
                   LL_ADC_OFFSET_ENABLE;
        }

        void set_offset_sign(const bool positive) const
        {
            LL_ADC_SetOffsetSign(ControllerTraitsT::skInstance,
                                 OffsetBlockTraitsT::skLLBlockId,
                                 positive ? LL_ADC_OFFSET_SIGN_POSITIVE : LL_ADC_OFFSET_SIGN_NEGATIVE);
        }

        [[nodiscard]] bool get_offset_sign_positive() const
        {
            return LL_ADC_GetOffsetSign(ControllerTraitsT::skInstance, OffsetBlockTraitsT::skLLBlockId) ==
                   LL_ADC_OFFSET_SIGN_POSITIVE;
        }

        [[nodiscard]] bool get_offset_sign_negative() const
        {
            return LL_ADC_GetOffsetSign(ControllerTraitsT::skInstance, OffsetBlockTraitsT::skLLBlockId) ==
                   LL_ADC_OFFSET_SIGN_NEGATIVE;
        }

        void set_offset_saturation(const bool enabled) const
        {
            LL_ADC_SetOffsetSaturation(ControllerTraitsT::skInstance,
                                       OffsetBlockTraitsT::skLLBlockId,
                                       enabled ? LL_ADC_OFFSET_SATURATION_ENABLE : LL_ADC_OFFSET_SATURATION_DISABLE);
        }

        [[nodiscard]] bool get_offset_saturation_enabled() const
        {
            return LL_ADC_GetOffsetSaturation(ControllerTraitsT::skInstance, OffsetBlockTraitsT::skLLBlockId) ==
                   LL_ADC_OFFSET_SATURATION_ENABLE;
        }
    };

}  // namespace valle::platform
