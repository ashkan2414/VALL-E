#pragma once

#include <array>
#include <chrono>

#include "stm32g4xx_ll_adc.h"
#include "valle/platform/hardware/adc/id.hpp"
#include "valle/platform/hardware/adc_common.hpp"
#include "valle/platform/hardware/dma.hpp"
#include "valle/utils/timing.hpp"

VALLE_OPTIMIZE_PUSH

namespace valle::platform
{
    // ============================================================================
    // ENUMERATIONS
    // ============================================================================

    inline constexpr auto kAdcResolutionNumberMapping =
        LLDriverEnumValueNumberMapping<4>{.mapping = {
                                              {LL_ADC_RESOLUTION_12B, 12},
                                              {LL_ADC_RESOLUTION_10B, 10},
                                              {LL_ADC_RESOLUTION_8B, 8},
                                              {LL_ADC_RESOLUTION_6B, 6},
                                          }};
    struct AdcResolution : public NumberedLLDriverEnumValue<AdcResolution, kAdcResolutionNumberMapping>
    {
        static const AdcResolution k12Bit;
        static const AdcResolution k10Bit;
        static const AdcResolution k8Bit;
        static const AdcResolution k6Bit;
    };

    inline constexpr AdcResolution AdcResolution::k12Bit = AdcResolution::from_number<12>();
    inline constexpr AdcResolution AdcResolution::k10Bit = AdcResolution::from_number<10>();
    inline constexpr AdcResolution AdcResolution::k8Bit  = AdcResolution::from_number<8>();
    inline constexpr AdcResolution AdcResolution::k6Bit  = AdcResolution::from_number<6>();

    struct AdcDataAlignment : public LLDriverEnumValue<AdcDataAlignment>
    {
        static const AdcDataAlignment kRight;
        static const AdcDataAlignment kLeft;
    };

    inline constexpr AdcDataAlignment AdcDataAlignment::kRight = AdcDataAlignment::from_ll<LL_ADC_DATA_ALIGN_RIGHT>();
    inline constexpr AdcDataAlignment AdcDataAlignment::kLeft  = AdcDataAlignment::from_ll<LL_ADC_DATA_ALIGN_LEFT>();

    struct AdcLowPowerMode : public LLDriverEnumValue<AdcLowPowerMode>
    {
        static const AdcLowPowerMode kNone;
        static const AdcLowPowerMode kAutoWait;
    };

    inline constexpr AdcLowPowerMode AdcLowPowerMode::kNone     = AdcLowPowerMode::from_ll<LL_ADC_LP_MODE_NONE>();
    inline constexpr AdcLowPowerMode AdcLowPowerMode::kAutoWait = AdcLowPowerMode::from_ll<LL_ADC_LP_AUTOWAIT>();

    struct AdcCommonSamplingTime : public LLDriverEnumValue<AdcCommonSamplingTime>
    {
        static const AdcCommonSamplingTime kDefault;
        static const AdcCommonSamplingTime k3P5CyclesReplace2P5Cycles;
    };

    inline constexpr AdcCommonSamplingTime AdcCommonSamplingTime::kDefault =
        AdcCommonSamplingTime::from_ll<LL_ADC_SAMPLINGTIME_COMMON_DEFAULT>();
    inline constexpr AdcCommonSamplingTime AdcCommonSamplingTime::k3P5CyclesReplace2P5Cycles =
        AdcCommonSamplingTime::from_ll<LL_ADC_SAMPLINGTIME_COMMON_3C5_REPL_2C5>();

    inline constexpr auto kInjectGroupScanModeNumberMapping =
        LLDriverEnumValueSequentialNumberMapping4>{.start_number = 1,
                                                   .ll_id_table  = {
                                                       LL_ADC_INJ_SEQ_SCAN_DISABLE,
                                                       LL_ADC_INJ_SEQ_SCAN_ENABLE_2RANKS,
                                                       LL_ADC_INJ_SEQ_SCAN_ENABLE_3RANKS,
                                                       LL_ADC_INJ_SEQ_SCAN_ENABLE_4RANKS,
                                                   }};

    struct AdcInjectGroupSequenceScanMode
        : public NumberedLLDriverEnumValue<AdcInjectGroupSequenceScanMode, kInjectGroupScanModeNumberMapping>
    {
        static const AdcInjectGroupSequenceScanMode kDisable;
        static const AdcInjectGroupSequenceScanMode k2Ranks;
        static const AdcInjectGroupSequenceScanMode k3Ranks;
        static const AdcInjectGroupSequenceScanMode k4Ranks;
    };

    inline constexpr AdcInjectGroupSequenceScanMode AdcInjectGroupSequenceScanMode::kDisable =
        AdcInjectGroupSequenceScanMode::from_number<1>();
    inline constexpr AdcInjectGroupSequenceScanMode AdcInjectGroupSequenceScanMode::k2Ranks =
        AdcInjectGroupSequenceScanMode::from_number<2>();
    inline constexpr AdcInjectGroupSequenceScanMode AdcInjectGroupSequenceScanMode::k3Ranks =
        AdcInjectGroupSequenceScanMode::from_number<3>();
    inline constexpr AdcInjectGroupSequenceScanMode AdcInjectGroupSequenceScanMode::k4Ranks =
        AdcInjectGroupSequenceScanMode::from_number<4>();

    struct AdcInjectGroupQueueMode : public LLDriverEnumValue<AdcInjectGroupQueueMode>
    {
        static const AdcInjectGroupQueueMode kDisable;
        static const AdcInjectGroupQueueMode k2ContextsLastActive;
        static const AdcInjectGroupQueueMode k2ContextsEndEmpty;
    };

    inline constexpr AdcInjectGroupQueueMode AdcInjectGroupQueueMode::kDisable =
        AdcInjectGroupQueueMode::from_ll<LL_ADC_INJ_QUEUE_DISABLE>();
    inline constexpr AdcInjectGroupQueueMode AdcInjectGroupQueueMode::k2ContextsLastActive =
        AdcInjectGroupQueueMode::from_ll<LL_ADC_INJ_QUEUE_2CONTEXTS_LAST_ACTIVE>();
    inline constexpr AdcInjectGroupQueueMode AdcInjectGroupQueueMode::k2ContextsEndEmpty =
        AdcInjectGroupQueueMode::from_ll<LL_ADC_INJ_QUEUE_2CONTEXTS_END_EMPTY>();

    struct AdcInjectGroupTriggerMode : public LLDriverEnumValue<AdcInjectGroupTriggerMode>
    {
        static const AdcInjectGroupTriggerMode kIndependent;
        static const AdcInjectGroupTriggerMode kFromRegularGroup;
    };

    inline constexpr AdcInjectGroupTriggerMode AdcInjectGroupTriggerMode::kIndependent =
        AdcInjectGroupTriggerMode::from_ll<LL_ADC_INJ_TRIG_INDEPENDENT>();
    inline constexpr AdcInjectGroupTriggerMode AdcInjectGroupTriggerMode::kFromRegularGroup =
        AdcInjectGroupTriggerMode::from_ll<LL_ADC_INJ_TRIG_FROM_GRP_REGULAR>();

    struct AdcInjectGroupSequencerDiscontinuityMode : public LLDriverEnumValue<AdcInjectGroupSequencerDiscontinuityMode>
    {
        static const AdcInjectGroupSequencerDiscontinuityMode kDisable;
        static const AdcInjectGroupSequencerDiscontinuityMode k1Rank;
    };

    inline constexpr AdcInjectGroupSequencerDiscontinuityMode AdcInjectGroupSequencerDiscontinuityMode::kDisable =
        AdcInjectGroupSequencerDiscontinuityMode::from_ll<LL_ADC_INJ_SEQ_DISCONT_DISABLE>();
    inline constexpr AdcInjectGroupSequencerDiscontinuityMode AdcInjectGroupSequencerDiscontinuityMode::k1Rank =
        AdcInjectGroupSequencerDiscontinuityMode::from_ll<LL_ADC_INJ_SEQ_DISCONT_1RANK>();

    struct AdcInjectGroupTriggerSource : public LLDriverEnumValue<AdcInjectGroupTriggerSource>
    {
        static const AdcInjectGroupTriggerSource kSoftware;
        static const AdcInjectGroupTriggerSource kExtTim1TRGO;
        static const AdcInjectGroupTriggerSource kExtTim1TRGO2;
        static const AdcInjectGroupTriggerSource kExtTim1CH3;
        static const AdcInjectGroupTriggerSource kExtTim1CH4;
        static const AdcInjectGroupTriggerSource kExtTim2TRGO;
        static const AdcInjectGroupTriggerSource kExtTim2CH1;
        static const AdcInjectGroupTriggerSource kExtTim3TRGO;
        static const AdcInjectGroupTriggerSource kExtTim3CH1;
        static const AdcInjectGroupTriggerSource kExtTim3CH3;
        static const AdcInjectGroupTriggerSource kExtTim3CH4;
        static const AdcInjectGroupTriggerSource kExtTim4TRGO;
        static const AdcInjectGroupTriggerSource kExtTim4CH3;
        static const AdcInjectGroupTriggerSource kExtTim4CH4;
        static const AdcInjectGroupTriggerSource kExtTim6TRGO;
        static const AdcInjectGroupTriggerSource kExtTim7TRGO;
        static const AdcInjectGroupTriggerSource kExtTim8TRGO;
        static const AdcInjectGroupTriggerSource kExtTim8TRGO2;
        static const AdcInjectGroupTriggerSource kExtTim8CH2;
        static const AdcInjectGroupTriggerSource kExtTim8CH4;
        static const AdcInjectGroupTriggerSource kExtTim15TRGO;
        static const AdcInjectGroupTriggerSource kExtTim16CH1;
        static const AdcInjectGroupTriggerSource kExtTim20TRGO;
        static const AdcInjectGroupTriggerSource kExtTim20TRGO2;
        static const AdcInjectGroupTriggerSource kExtTim20CH2;
        static const AdcInjectGroupTriggerSource kExtTim20CH4;
        static const AdcInjectGroupTriggerSource kExtHrtimTRG1;
        static const AdcInjectGroupTriggerSource kExtHrtimTRG2;
        static const AdcInjectGroupTriggerSource kExtHrtimTRG3;
        static const AdcInjectGroupTriggerSource kExtHrtimTRG4;
        static const AdcInjectGroupTriggerSource kExtHrtimTRG5;
        static const AdcInjectGroupTriggerSource kExtHrtimTRG6;
        static const AdcInjectGroupTriggerSource kExtHrtimTRG7;
        static const AdcInjectGroupTriggerSource kExtHrtimTRG8;
        static const AdcInjectGroupTriggerSource kExtHrtimTRG9;
        static const AdcInjectGroupTriggerSource kExtHrtimTRG10;
        static const AdcInjectGroupTriggerSource kExtExtiLine3;
        static const AdcInjectGroupTriggerSource kExtExtiLine15;
        static const AdcInjectGroupTriggerSource kExtLptimOut;
    };

    inline constexpr AdcInjectGroupTriggerSource AdcInjectGroupTriggerSource::kSoftware =
        AdcInjectGroupTriggerSource::from_ll<LL_ADC_INJ_TRIG_SOFTWARE>();
    inline constexpr AdcInjectGroupTriggerSource AdcInjectGroupTriggerSource::kExtTim1TRGO =
        AdcInjectGroupTriggerSource::from_ll<LL_ADC_INJ_TRIG_EXT_TIM1_TRGO>();
    inline constexpr AdcInjectGroupTriggerSource AdcInjectGroupTriggerSource::kExtTim1TRGO2 =
        AdcInjectGroupTriggerSource::from_ll<LL_ADC_INJ_TRIG_EXT_TIM1_TRGO2>();
    inline constexpr AdcInjectGroupTriggerSource AdcInjectGroupTriggerSource::kExtTim1CH3 =
        AdcInjectGroupTriggerSource::from_ll<LL_ADC_INJ_TRIG_EXT_TIM1_CH3>();
    inline constexpr AdcInjectGroupTriggerSource AdcInjectGroupTriggerSource::kExtTim1CH4 =
        AdcInjectGroupTriggerSource::from_ll<LL_ADC_INJ_TRIG_EXT_TIM1_CH4>();
    inline constexpr AdcInjectGroupTriggerSource AdcInjectGroupTriggerSource::kExtTim2TRGO =
        AdcInjectGroupTriggerSource::from_ll<LL_ADC_INJ_TRIG_EXT_TIM2_TRGO>();
    inline constexpr AdcInjectGroupTriggerSource AdcInjectGroupTriggerSource::kExtTim2CH1 =
        AdcInjectGroupTriggerSource::from_ll<LL_ADC_INJ_TRIG_EXT_TIM2_CH1>();
    inline constexpr AdcInjectGroupTriggerSource AdcInjectGroupTriggerSource::kExtTim3TRGO =
        AdcInjectGroupTriggerSource::from_ll<LL_ADC_INJ_TRIG_EXT_TIM3_TRGO>();
    inline constexpr AdcInjectGroupTriggerSource AdcInjectGroupTriggerSource::kExtTim3CH1 =
        AdcInjectGroupTriggerSource::from_ll<LL_ADC_INJ_TRIG_EXT_TIM3_CH1>();
    inline constexpr AdcInjectGroupTriggerSource AdcInjectGroupTriggerSource::kExtTim3CH3 =
        AdcInjectGroupTriggerSource::from_ll<LL_ADC_INJ_TRIG_EXT_TIM3_CH3>();
    inline constexpr AdcInjectGroupTriggerSource AdcInjectGroupTriggerSource::kExtTim3CH4 =
        AdcInjectGroupTriggerSource::from_ll<LL_ADC_INJ_TRIG_EXT_TIM3_CH4>();
    inline constexpr AdcInjectGroupTriggerSource AdcInjectGroupTriggerSource::kExtTim4TRGO =
        AdcInjectGroupTriggerSource::from_ll<LL_ADC_INJ_TRIG_EXT_TIM4_TRGO>();
    inline constexpr AdcInjectGroupTriggerSource AdcInjectGroupTriggerSource::kExtTim4CH3 =
        AdcInjectGroupTriggerSource::from_ll<LL_ADC_INJ_TRIG_EXT_TIM4_CH3>();
    inline constexpr AdcInjectGroupTriggerSource AdcInjectGroupTriggerSource::kExtTim4CH4 =
        AdcInjectGroupTriggerSource::from_ll<LL_ADC_INJ_TRIG_EXT_TIM4_CH4>();
    inline constexpr AdcInjectGroupTriggerSource AdcInjectGroupTriggerSource::kExtTim6TRGO =
        AdcInjectGroupTriggerSource::from_ll<LL_ADC_INJ_TRIG_EXT_TIM6_TRGO>();
    inline constexpr AdcInjectGroupTriggerSource AdcInjectGroupTriggerSource::kExtTim7TRGO =
        AdcInjectGroupTriggerSource::from_ll<LL_ADC_INJ_TRIG_EXT_TIM7_TRGO>();
    inline constexpr AdcInjectGroupTriggerSource AdcInjectGroupTriggerSource::kExtTim8TRGO =
        AdcInjectGroupTriggerSource::from_ll<LL_ADC_INJ_TRIG_EXT_TIM8_TRGO>();
    inline constexpr AdcInjectGroupTriggerSource AdcInjectGroupTriggerSource::kExtTim8TRGO2 =
        AdcInjectGroupTriggerSource::from_ll<LL_ADC_INJ_TRIG_EXT_TIM8_TRGO2>();
    inline constexpr AdcInjectGroupTriggerSource AdcInjectGroupTriggerSource::kExtTim8CH2 =
        AdcInjectGroupTriggerSource::from_ll<LL_ADC_INJ_TRIG_EXT_TIM8_CH2>();
    inline constexpr AdcInjectGroupTriggerSource AdcInjectGroupTriggerSource::kExtTim8CH4 =
        AdcInjectGroupTriggerSource::from_ll<LL_ADC_INJ_TRIG_EXT_TIM8_CH4>();
    inline constexpr AdcInjectGroupTriggerSource AdcInjectGroupTriggerSource::kExtTim15TRGO =
        AdcInjectGroupTriggerSource::from_ll<LL_ADC_INJ_TRIG_EXT_TIM15_TRGO>();
    inline constexpr AdcInjectGroupTriggerSource AdcInjectGroupTriggerSource::kExtTim16CH1 =
        AdcInjectGroupTriggerSource::from_ll<LL_ADC_INJ_TRIG_EXT_TIM16_CH1>();
    inline constexpr AdcInjectGroupTriggerSource AdcInjectGroupTriggerSource::kExtTim20TRGO =
        AdcInjectGroupTriggerSource::from_ll<LL_ADC_INJ_TRIG_EXT_TIM20_TRGO>();
    inline constexpr AdcInjectGroupTriggerSource AdcInjectGroupTriggerSource::kExtTim20TRGO2 =
        AdcInjectGroupTriggerSource::from_ll<LL_ADC_INJ_TRIG_EXT_TIM20_TRGO2>();
    inline constexpr AdcInjectGroupTriggerSource AdcInjectGroupTriggerSource::kExtTim20CH2 =
        AdcInjectGroupTriggerSource::from_ll<LL_ADC_INJ_TRIG_EXT_TIM20_CH2>();
    inline constexpr AdcInjectGroupTriggerSource AdcInjectGroupTriggerSource::kExtTim20CH4 =
        AdcInjectGroupTriggerSource::from_ll<LL_ADC_INJ_TRIG_EXT_TIM20_CH4>();
    inline constexpr AdcInjectGroupTriggerSource AdcInjectGroupTriggerSource::kExtHrtimTRG1 =
        AdcInjectGroupTriggerSource::from_ll<LL_ADC_INJ_TRIG_EXT_HRTIM_TRG1>();
    inline constexpr AdcInjectGroupTriggerSource AdcInjectGroupTriggerSource::kExtHrtimTRG2 =
        AdcInjectGroupTriggerSource::from_ll<LL_ADC_INJ_TRIG_EXT_HRTIM_TRG2>();
    inline constexpr AdcInjectGroupTriggerSource AdcInjectGroupTriggerSource::kExtHrtimTRG3 =
        AdcInjectGroupTriggerSource::from_ll<LL_ADC_INJ_TRIG_EXT_HRTIM_TRG3>();
    inline constexpr AdcInjectGroupTriggerSource AdcInjectGroupTriggerSource::kExtHrtimTRG4 =
        AdcInjectGroupTriggerSource::from_ll<LL_ADC_INJ_TRIG_EXT_HRTIM_TRG4>();
    inline constexpr AdcInjectGroupTriggerSource AdcInjectGroupTriggerSource::kExtHrtimTRG5 =
        AdcInjectGroupTriggerSource::from_ll<LL_ADC_INJ_TRIG_EXT_HRTIM_TRG5>();
    inline constexpr AdcInjectGroupTriggerSource AdcInjectGroupTriggerSource::kExtHrtimTRG6 =
        AdcInjectGroupTriggerSource::from_ll<LL_ADC_INJ_TRIG_EXT_HRTIM_TRG6>();
    inline constexpr AdcInjectGroupTriggerSource AdcInjectGroupTriggerSource::kExtHrtimTRG7 =
        AdcInjectGroupTriggerSource::from_ll<LL_ADC_INJ_TRIG_EXT_HRTIM_TRG7>();
    inline constexpr AdcInjectGroupTriggerSource AdcInjectGroupTriggerSource::kExtHrtimTRG8 =
        AdcInjectGroupTriggerSource::from_ll<LL_ADC_INJ_TRIG_EXT_HRTIM_TRG8>();
    inline constexpr AdcInjectGroupTriggerSource AdcInjectGroupTriggerSource::kExtHrtimTRG9 =
        AdcInjectGroupTriggerSource::from_ll<LL_ADC_INJ_TRIG_EXT_HRTIM_TRG9>();
    inline constexpr AdcInjectGroupTriggerSource AdcInjectGroupTriggerSource::kExtHrtimTRG10 =
        AdcInjectGroupTriggerSource::from_ll<LL_ADC_INJ_TRIG_EXT_HRTIM_TRG10>();
    inline constexpr AdcInjectGroupTriggerSource AdcInjectGroupTriggerSource::kExtExtiLine3 =
        AdcInjectGroupTriggerSource::from_ll<LL_ADC_INJ_TRIG_EXT_EXTI_LINE3>();
    inline constexpr AdcInjectGroupTriggerSource AdcInjectGroupTriggerSource::kExtExtiLine15 =
        AdcInjectGroupTriggerSource::from_ll<LL_ADC_INJ_TRIG_EXT_EXTI_LINE15>();
    inline constexpr AdcInjectGroupTriggerSource AdcInjectGroupTriggerSource::kExtLptimOut =
        AdcInjectGroupTriggerSource::from_ll<LL_ADC_INJ_TRIG_EXT_LPTIM_OUT>();

    struct AdcInjectGroupTriggerEdge : public LLDriverEnumValue<AdcInjectGroupTriggerEdge>
    {
        static const AdcInjectGroupTriggerEdge kRising;
        static const AdcInjectGroupTriggerEdge kFalling;
        static const AdcInjectGroupTriggerEdge kRisingFalling;
    };

    inline constexpr AdcInjectGroupTriggerEdge AdcInjectGroupTriggerEdge::kRising =
        AdcInjectGroupTriggerEdge::from_ll<LL_ADC_INJ_TRIG_EXT_RISING>();
    inline constexpr AdcInjectGroupTriggerEdge AdcInjectGroupTriggerEdge::kFalling =
        AdcInjectGroupTriggerEdge::from_ll<LL_ADC_INJ_TRIG_EXT_FALLING>();
    inline constexpr AdcInjectGroupTriggerEdge AdcInjectGroupTriggerEdge::kRisingFalling =
        AdcInjectGroupTriggerEdge::from_ll<LL_ADC_INJ_TRIG_EXT_RISINGFALLING>();

    struct AdcRegularGroupTriggerSource : public LLDriverEnumValue<AdcRegularGroupTriggerSource>
    {
        static const AdcRegularGroupTriggerSource kSoftware;
        static const AdcRegularGroupTriggerSource kExtTim1TRGO;
        static const AdcRegularGroupTriggerSource kExtTim1TRGO2;
        static const AdcRegularGroupTriggerSource kExtTim1CH1;
        static const AdcRegularGroupTriggerSource kExtTim1CH2;
        static const AdcRegularGroupTriggerSource kExtTim1CH3;
        static const AdcRegularGroupTriggerSource kExtTim2TRGO;
        static const AdcRegularGroupTriggerSource kExtTim2CH1;
        static const AdcRegularGroupTriggerSource kExtTim2CH2;
        static const AdcRegularGroupTriggerSource kExtTim2CH3;
        static const AdcRegularGroupTriggerSource kExtTim3TRGO;
        static const AdcRegularGroupTriggerSource kExtTim3CH1;
        static const AdcRegularGroupTriggerSource kExtTim3CH4;
        static const AdcRegularGroupTriggerSource kExtTim4TRGO;
        static const AdcRegularGroupTriggerSource kExtTim4CH1;
        static const AdcRegularGroupTriggerSource kExtTim4CH4;
        static const AdcRegularGroupTriggerSource kExtTim6TRGO;
        static const AdcRegularGroupTriggerSource kExtTim7TRGO;
        static const AdcRegularGroupTriggerSource kExtTim8TRGO;
        static const AdcRegularGroupTriggerSource kExtTim8TRGO2;
        static const AdcRegularGroupTriggerSource kExtTim8CH1;
        static const AdcRegularGroupTriggerSource kExtTim15TRGO;
        static const AdcRegularGroupTriggerSource kExtTim20TRGO;
        static const AdcRegularGroupTriggerSource kExtTim20TRGO2;
        static const AdcRegularGroupTriggerSource kExtTim20CH1;
        static const AdcRegularGroupTriggerSource kExtTim20CH2;
        static const AdcRegularGroupTriggerSource kExtTim20CH3;
        static const AdcRegularGroupTriggerSource kExtHrtimTRG1;
        static const AdcRegularGroupTriggerSource kExtHrtimTRG2;
        static const AdcRegularGroupTriggerSource kExtHrtimTRG3;
        static const AdcRegularGroupTriggerSource kExtHrtimTRG4;
        static const AdcRegularGroupTriggerSource kExtHrtimTRG5;
        static const AdcRegularGroupTriggerSource kExtHrtimTRG6;
        static const AdcRegularGroupTriggerSource kExtHrtimTRG7;
        static const AdcRegularGroupTriggerSource kExtHrtimTRG8;
        static const AdcRegularGroupTriggerSource kExtHrtimTRG9;
        static const AdcRegularGroupTriggerSource kExtHrtimTRG10;
        static const AdcRegularGroupTriggerSource kExtExtiLine11;
        static const AdcRegularGroupTriggerSource kExtExtiLine2;
        static const AdcRegularGroupTriggerSource kExtLptimOut;
    };

    inline constexpr AdcRegularGroupTriggerSource AdcRegularGroupTriggerSource::kSoftware =
        AdcRegularGroupTriggerSource::from_ll<LL_ADC_REG_TRIG_SOFTWARE>();
    inline constexpr AdcRegularGroupTriggerSource AdcRegularGroupTriggerSource::kExtTim1TRGO =
        AdcRegularGroupTriggerSource::from_ll<LL_ADC_REG_TRIG_EXT_TIM1_TRGO>();
    inline constexpr AdcRegularGroupTriggerSource AdcRegularGroupTriggerSource::kExtTim1TRGO2 =
        AdcRegularGroupTriggerSource::from_ll<LL_ADC_REG_TRIG_EXT_TIM1_TRGO2>();
    inline constexpr AdcRegularGroupTriggerSource AdcRegularGroupTriggerSource::kExtTim1CH1 =
        AdcRegularGroupTriggerSource::from_ll<LL_ADC_REG_TRIG_EXT_TIM1_CH1>();
    inline constexpr AdcRegularGroupTriggerSource AdcRegularGroupTriggerSource::kExtTim1CH2 =
        AdcRegularGroupTriggerSource::from_ll<LL_ADC_REG_TRIG_EXT_TIM1_CH2>();
    inline constexpr AdcRegularGroupTriggerSource AdcRegularGroupTriggerSource::kExtTim1CH3 =
        AdcRegularGroupTriggerSource::from_ll<LL_ADC_REG_TRIG_EXT_TIM1_CH3>();
    inline constexpr AdcRegularGroupTriggerSource AdcRegularGroupTriggerSource::kExtTim2TRGO =
        AdcRegularGroupTriggerSource::from_ll<LL_ADC_REG_TRIG_EXT_TIM2_TRGO>();
    inline constexpr AdcRegularGroupTriggerSource AdcRegularGroupTriggerSource::kExtTim2CH1 =
        AdcRegularGroupTriggerSource::from_ll<LL_ADC_REG_TRIG_EXT_TIM2_CH1>();
    inline constexpr AdcRegularGroupTriggerSource AdcRegularGroupTriggerSource::kExtTim2CH2 =
        AdcRegularGroupTriggerSource::from_ll<LL_ADC_REG_TRIG_EXT_TIM2_CH2>();
    inline constexpr AdcRegularGroupTriggerSource AdcRegularGroupTriggerSource::kExtTim2CH3 =
        AdcRegularGroupTriggerSource::from_ll<LL_ADC_REG_TRIG_EXT_TIM2_CH3>();
    inline constexpr AdcRegularGroupTriggerSource AdcRegularGroupTriggerSource::kExtTim3TRGO =
        AdcRegularGroupTriggerSource::from_ll<LL_ADC_REG_TRIG_EXT_TIM3_TRGO>();
    inline constexpr AdcRegularGroupTriggerSource AdcRegularGroupTriggerSource::kExtTim3CH1 =
        AdcRegularGroupTriggerSource::from_ll<LL_ADC_REG_TRIG_EXT_TIM3_CH1>();
    inline constexpr AdcRegularGroupTriggerSource AdcRegularGroupTriggerSource::kExtTim3CH4 =
        AdcRegularGroupTriggerSource::from_ll<LL_ADC_REG_TRIG_EXT_TIM3_CH4>();
    inline constexpr AdcRegularGroupTriggerSource AdcRegularGroupTriggerSource::kExtTim4TRGO =
        AdcRegularGroupTriggerSource::from_ll<LL_ADC_REG_TRIG_EXT_TIM4_TRGO>();
    inline constexpr AdcRegularGroupTriggerSource AdcRegularGroupTriggerSource::kExtTim4CH1 =
        AdcRegularGroupTriggerSource::from_ll<LL_ADC_REG_TRIG_EXT_TIM4_CH1>();
    inline constexpr AdcRegularGroupTriggerSource AdcRegularGroupTriggerSource::kExtTim4CH4 =
        AdcRegularGroupTriggerSource::from_ll<LL_ADC_REG_TRIG_EXT_TIM4_CH4>();
    inline constexpr AdcRegularGroupTriggerSource AdcRegularGroupTriggerSource::kExtTim6TRGO =
        AdcRegularGroupTriggerSource::from_ll<LL_ADC_REG_TRIG_EXT_TIM6_TRGO>();
    inline constexpr AdcRegularGroupTriggerSource AdcRegularGroupTriggerSource::kExtTim7TRGO =
        AdcRegularGroupTriggerSource::from_ll<LL_ADC_REG_TRIG_EXT_TIM7_TRGO>();
    inline constexpr AdcRegularGroupTriggerSource AdcRegularGroupTriggerSource::kExtTim8TRGO =
        AdcRegularGroupTriggerSource::from_ll<LL_ADC_REG_TRIG_EXT_TIM8_TRGO>();
    inline constexpr AdcRegularGroupTriggerSource AdcRegularGroupTriggerSource::kExtTim8TRGO2 =
        AdcRegularGroupTriggerSource::from_ll<LL_ADC_REG_TRIG_EXT_TIM8_TRGO2>();
    inline constexpr AdcRegularGroupTriggerSource AdcRegularGroupTriggerSource::kExtTim8CH1 =
        AdcRegularGroupTriggerSource::from_ll<LL_ADC_REG_TRIG_EXT_TIM8_CH1>();
    inline constexpr AdcRegularGroupTriggerSource AdcRegularGroupTriggerSource::kExtTim15TRGO =
        AdcRegularGroupTriggerSource::from_ll<LL_ADC_REG_TRIG_EXT_TIM15_TRGO>();
    inline constexpr AdcRegularGroupTriggerSource AdcRegularGroupTriggerSource::kExtTim20TRGO =
        AdcRegularGroupTriggerSource::from_ll<LL_ADC_REG_TRIG_EXT_TIM20_TRGO>();
    inline constexpr AdcRegularGroupTriggerSource AdcRegularGroupTriggerSource::kExtTim20TRGO2 =
        AdcRegularGroupTriggerSource::from_ll<LL_ADC_REG_TRIG_EXT_TIM20_TRGO2>();
    inline constexpr AdcRegularGroupTriggerSource AdcRegularGroupTriggerSource::kExtTim20CH1 =
        AdcRegularGroupTriggerSource::from_ll<LL_ADC_REG_TRIG_EXT_TIM20_CH1>();
    inline constexpr AdcRegularGroupTriggerSource AdcRegularGroupTriggerSource::kExtTim20CH2 =
        AdcRegularGroupTriggerSource::from_ll<LL_ADC_REG_TRIG_EXT_TIM20_CH2>();
    inline constexpr AdcRegularGroupTriggerSource AdcRegularGroupTriggerSource::kExtTim20CH3 =
        AdcRegularGroupTriggerSource::from_ll<LL_ADC_REG_TRIG_EXT_TIM20_CH3>();
    inline constexpr AdcRegularGroupTriggerSource AdcRegularGroupTriggerSource::kExtHrtimTRG1 =
        AdcRegularGroupTriggerSource::from_ll<LL_ADC_REG_TRIG_EXT_HRTIM_TRG1>();
    inline constexpr AdcRegularGroupTriggerSource AdcRegularGroupTriggerSource::kExtHrtimTRG2 =
        AdcRegularGroupTriggerSource::from_ll<LL_ADC_REG_TRIG_EXT_HRTIM_TRG2>();
    inline constexpr AdcRegularGroupTriggerSource AdcRegularGroupTriggerSource::kExtHrtimTRG3 =
        AdcRegularGroupTriggerSource::from_ll<LL_ADC_REG_TRIG_EXT_HRTIM_TRG3>();
    inline constexpr AdcRegularGroupTriggerSource AdcRegularGroupTriggerSource::kExtHrtimTRG4 =
        AdcRegularGroupTriggerSource::from_ll<LL_ADC_REG_TRIG_EXT_HRTIM_TRG4>();
    inline constexpr AdcRegularGroupTriggerSource AdcRegularGroupTriggerSource::kExtHrtimTRG5 =
        AdcRegularGroupTriggerSource::from_ll<LL_ADC_REG_TRIG_EXT_HRTIM_TRG5>();
    inline constexpr AdcRegularGroupTriggerSource AdcRegularGroupTriggerSource::kExtHrtimTRG6 =
        AdcRegularGroupTriggerSource::from_ll<LL_ADC_REG_TRIG_EXT_HRTIM_TRG6>();
    inline constexpr AdcRegularGroupTriggerSource AdcRegularGroupTriggerSource::kExtHrtimTRG7 =
        AdcRegularGroupTriggerSource::from_ll<LL_ADC_REG_TRIG_EXT_HRTIM_TRG7>();
    inline constexpr AdcRegularGroupTriggerSource AdcRegularGroupTriggerSource::kExtHrtimTRG8 =
        AdcRegularGroupTriggerSource::from_ll<LL_ADC_REG_TRIG_EXT_HRTIM_TRG8>();
    inline constexpr AdcRegularGroupTriggerSource AdcRegularGroupTriggerSource::kExtHrtimTRG9 =
        AdcRegularGroupTriggerSource::from_ll<LL_ADC_REG_TRIG_EXT_HRTIM_TRG9>();
    inline constexpr AdcRegularGroupTriggerSource AdcRegularGroupTriggerSource::kExtHrtimTRG10 =
        AdcRegularGroupTriggerSource::from_ll<LL_ADC_REG_TRIG_EXT_HRTIM_TRG10>();
    inline constexpr AdcRegularGroupTriggerSource AdcRegularGroupTriggerSource::kExtExtiLine11 =
        AdcRegularGroupTriggerSource::from_ll<LL_ADC_REG_TRIG_EXT_EXTI_LINE11>();
    inline constexpr AdcRegularGroupTriggerSource AdcRegularGroupTriggerSource::kExtExtiLine2 =
        AdcRegularGroupTriggerSource::from_ll<LL_ADC_REG_TRIG_EXT_EXTI_LINE2>();
    inline constexpr AdcRegularGroupTriggerSource AdcRegularGroupTriggerSource::kExtLptimOut =
        AdcRegularGroupTriggerSource::from_ll<LL_ADC_REG_TRIG_EXT_LPTIM_OUT>();

    struct AdcRegularGroupTriggerEdge : public LLDriverEnumValue<AdcRegularGroupTriggerEdge>
    {
        static const AdcRegularGroupTriggerEdge kRising;
        static const AdcRegularGroupTriggerEdge kFalling;
        static const AdcRegularGroupTriggerEdge kRisingFalling;
    };

    inline constexpr AdcRegularGroupTriggerEdge AdcRegularGroupTriggerEdge::kRising =
        AdcRegularGroupTriggerEdge::from_ll<LL_ADC_REG_TRIG_EXT_RISING>();
    inline constexpr AdcRegularGroupTriggerEdge AdcRegularGroupTriggerEdge::kFalling =
        AdcRegularGroupTriggerEdge::from_ll<LL_ADC_REG_TRIG_EXT_FALLING>();
    inline constexpr AdcRegularGroupTriggerEdge AdcRegularGroupTriggerEdge::kRisingFalling =
        AdcRegularGroupTriggerEdge::from_ll<LL_ADC_REG_TRIG_EXT_RISINGFALLING>();

    inline constexpr auto kRegularGroupScanModeNumberMapping =
        LLDriverEnumValueSequentialNumberMapping<16>{.start_number = 1,
                                                     .ll_id_table  = {
                                                         LL_ADC_REG_SEQ_SCAN_DISABLE,
                                                         LL_ADC_REG_SEQ_SCAN_ENABLE_2RANKS,
                                                         LL_ADC_REG_SEQ_SCAN_ENABLE_3RANKS,
                                                         LL_ADC_REG_SEQ_SCAN_ENABLE_4RANKS,
                                                         LL_ADC_REG_SEQ_SCAN_ENABLE_5RANKS,
                                                         LL_ADC_REG_SEQ_SCAN_ENABLE_6RANKS,
                                                         LL_ADC_REG_SEQ_SCAN_ENABLE_7RANKS,
                                                         LL_ADC_REG_SEQ_SCAN_ENABLE_8RANKS,
                                                         LL_ADC_REG_SEQ_SCAN_ENABLE_9RANKS,
                                                         LL_ADC_REG_SEQ_SCAN_ENABLE_10RANKS,
                                                         LL_ADC_REG_SEQ_SCAN_ENABLE_11RANKS,
                                                         LL_ADC_REG_SEQ_SCAN_ENABLE_12RANKS,
                                                         LL_ADC_REG_SEQ_SCAN_ENABLE_13RANKS,
                                                         LL_ADC_REG_SEQ_SCAN_ENABLE_14RANKS,
                                                         LL_ADC_REG_SEQ_SCAN_ENABLE_15RANKS,
                                                         LL_ADC_REG_SEQ_SCAN_ENABLE_16RANKS,
                                                     }};

    struct AdcRegularGroupSequenceScanMode
        : public NumberedLLDriverEnumValue<AdcRegularGroupSequenceScanMode, kRegularGroupScanModeNumberMapping>
    {
        static const AdcRegularGroupSequenceScanMode kDisable;
        static const AdcRegularGroupSequenceScanMode k2Ranks;
        static const AdcRegularGroupSequenceScanMode k3Ranks;
        static const AdcRegularGroupSequenceScanMode k4Ranks;
        static const AdcRegularGroupSequenceScanMode k5Ranks;
        static const AdcRegularGroupSequenceScanMode k6Ranks;
        static const AdcRegularGroupSequenceScanMode k7Ranks;
        static const AdcRegularGroupSequenceScanMode k8Ranks;
        static const AdcRegularGroupSequenceScanMode k9Ranks;
        static const AdcRegularGroupSequenceScanMode k10Ranks;
        static const AdcRegularGroupSequenceScanMode k11Ranks;
        static const AdcRegularGroupSequenceScanMode k12Ranks;
        static const AdcRegularGroupSequenceScanMode k13Ranks;
        static const AdcRegularGroupSequenceScanMode k14Ranks;
        static const AdcRegularGroupSequenceScanMode k15Ranks;
        static const AdcRegularGroupSequenceScanMode k16Ranks;
    };

    inline constexpr AdcRegularGroupSequenceScanMode AdcRegularGroupSequenceScanMode::kDisable =
        AdcRegularGroupSequenceScanMode::from_number<1>();
    inline constexpr AdcRegularGroupSequenceScanMode AdcRegularGroupSequenceScanMode::k2Ranks =
        AdcRegularGroupSequenceScanMode::from_number<2>();
    inline constexpr AdcRegularGroupSequenceScanMode AdcRegularGroupSequenceScanMode::k3Ranks =
        AdcRegularGroupSequenceScanMode::from_number<3>();
    inline constexpr AdcRegularGroupSequenceScanMode AdcRegularGroupSequenceScanMode::k4Ranks =
        AdcRegularGroupSequenceScanMode::from_number<4>();
    inline constexpr AdcRegularGroupSequenceScanMode AdcRegularGroupSequenceScanMode::k5Ranks =
        AdcRegularGroupSequenceScanMode::from_number<5>();
    inline constexpr AdcRegularGroupSequenceScanMode AdcRegularGroupSequenceScanMode::k6Ranks =
        AdcRegularGroupSequenceScanMode::from_number<6>();
    inline constexpr AdcRegularGroupSequenceScanMode AdcRegularGroupSequenceScanMode::k7Ranks =
        AdcRegularGroupSequenceScanMode::from_number<7>();
    inline constexpr AdcRegularGroupSequenceScanMode AdcRegularGroupSequenceScanMode::k8Ranks =
        AdcRegularGroupSequenceScanMode::from_number<8>();
    inline constexpr AdcRegularGroupSequenceScanMode AdcRegularGroupSequenceScanMode::k9Ranks =
        AdcRegularGroupSequenceScanMode::from_number<9>();
    inline constexpr AdcRegularGroupSequenceScanMode AdcRegularGroupSequenceScanMode::k10Ranks =
        AdcRegularGroupSequenceScanMode::from_number<10>();
    inline constexpr AdcRegularGroupSequenceScanMode AdcRegularGroupSequenceScanMode::k11Ranks =
        AdcRegularGroupSequenceScanMode::from_number<11>();
    inline constexpr AdcRegularGroupSequenceScanMode AdcRegularGroupSequenceScanMode::k12Ranks =
        AdcRegularGroupSequenceScanMode::from_number<12>();
    inline constexpr AdcRegularGroupSequenceScanMode AdcRegularGroupSequenceScanMode::k13Ranks =
        AdcRegularGroupSequenceScanMode::from_number<13>();
    inline constexpr AdcRegularGroupSequenceScanMode AdcRegularGroupSequenceScanMode::k14Ranks =
        AdcRegularGroupSequenceScanMode::from_number<14>();
    inline constexpr AdcRegularGroupSequenceScanMode AdcRegularGroupSequenceScanMode::k15Ranks =
        AdcRegularGroupSequenceScanMode::from_number<15>();
    inline constexpr AdcRegularGroupSequenceScanMode AdcRegularGroupSequenceScanMode::k16Ranks =
        AdcRegularGroupSequenceScanMode::from_number<16>();

    inline constexpr auto kRegularGroupSequencerDiscontinuityNumberMapping =
        LLDriverEnumValueSequentialNumberMapping<9>{.start_number = 0,
                                                    .ll_id_table  = {
                                                        LL_ADC_REG_SEQ_DISCONT_DISABLE,
                                                        LL_ADC_REG_SEQ_DISCONT_1RANK,
                                                        LL_ADC_REG_SEQ_DISCONT_2RANKS,
                                                        LL_ADC_REG_SEQ_DISCONT_3RANKS,
                                                        LL_ADC_REG_SEQ_DISCONT_4RANKS,
                                                        LL_ADC_REG_SEQ_DISCONT_5RANKS,
                                                        LL_ADC_REG_SEQ_DISCONT_6RANKS,
                                                        LL_ADC_REG_SEQ_DISCONT_7RANKS,
                                                        LL_ADC_REG_SEQ_DISCONT_8RANKS,
                                                    }};

    struct AdcRegularGroupSequencerDiscontinuity
        : public NumberedLLDriverEnumValue<AdcRegularGroupSequencerDiscontinuity,
                                           kRegularGroupSequencerDiscontinuityNumberMapping>
    {
        static const AdcRegularGroupSequencerDiscontinuity kDisable;
        static const AdcRegularGroupSequencerDiscontinuity k1Rank;
        static const AdcRegularGroupSequencerDiscontinuity k2Ranks;
        static const AdcRegularGroupSequencerDiscontinuity k3Ranks;
        static const AdcRegularGroupSequencerDiscontinuity k4Ranks;
        static const AdcRegularGroupSequencerDiscontinuity k5Ranks;
        static const AdcRegularGroupSequencerDiscontinuity k6Ranks;
        static const AdcRegularGroupSequencerDiscontinuity k7Ranks;
        static const AdcRegularGroupSequencerDiscontinuity k8Ranks;
    };

    inline constexpr AdcRegularGroupSequencerDiscontinuity AdcRegularGroupSequencerDiscontinuity::kDisable =
        AdcRegularGroupSequencerDiscontinuity::from_number<0>();
    inline constexpr AdcRegularGroupSequencerDiscontinuity AdcRegularGroupSequencerDiscontinuity::k1Rank =
        AdcRegularGroupSequencerDiscontinuity::from_number<1>();
    inline constexpr AdcRegularGroupSequencerDiscontinuity AdcRegularGroupSequencerDiscontinuity::k2Ranks =
        AdcRegularGroupSequencerDiscontinuity::from_number<2>();
    inline constexpr AdcRegularGroupSequencerDiscontinuity AdcRegularGroupSequencerDiscontinuity::k3Ranks =
        AdcRegularGroupSequencerDiscontinuity::from_number<3>();
    inline constexpr AdcRegularGroupSequencerDiscontinuity AdcRegularGroupSequencerDiscontinuity::k4Ranks =
        AdcRegularGroupSequencerDiscontinuity::from_number<4>();
    inline constexpr AdcRegularGroupSequencerDiscontinuity AdcRegularGroupSequencerDiscontinuity::k5Ranks =
        AdcRegularGroupSequencerDiscontinuity::from_number<5>();
    inline constexpr AdcRegularGroupSequencerDiscontinuity AdcRegularGroupSequencerDiscontinuity::k6Ranks =
        AdcRegularGroupSequencerDiscontinuity::from_number<6>();
    inline constexpr AdcRegularGroupSequencerDiscontinuity AdcRegularGroupSequencerDiscontinuity::k7Ranks =
        AdcRegularGroupSequencerDiscontinuity::from_number<7>();
    inline constexpr AdcRegularGroupSequencerDiscontinuity AdcRegularGroupSequencerDiscontinuity::k8Ranks =
        AdcRegularGroupSequencerDiscontinuity::from_number<8>();

    struct AdcRegularGroupDmaTransfer : public LLDriverEnumValue<AdcRegularGroupDmaTransfer>
    {
        static const AdcRegularGroupDmaTransfer kNone;
        static const AdcRegularGroupDmaTransfer kUnlimited;
        static const AdcRegularGroupDmaTransfer kLimited;
    };

    inline constexpr AdcRegularGroupDmaTransfer AdcRegularGroupDmaTransfer::kNone =
        AdcRegularGroupDmaTransfer::from_ll<LL_ADC_REG_DMA_TRANSFER_NONE>();
    inline constexpr AdcRegularGroupDmaTransfer AdcRegularGroupDmaTransfer::kUnlimited =
        AdcRegularGroupDmaTransfer::from_ll<LL_ADC_REG_DMA_TRANSFER_UNLIMITED>();
    inline constexpr AdcRegularGroupDmaTransfer AdcRegularGroupDmaTransfer::kLimited =
        AdcRegularGroupDmaTransfer::from_ll<LL_ADC_REG_DMA_TRANSFER_LIMITED>();

    struct AdcRegularGroupOverrunBehavior : public LLDriverEnumValue<AdcRegularGroupOverrunBehavior>
    {
        static const AdcRegularGroupOverrunBehavior kPreserve;
        static const AdcRegularGroupOverrunBehavior kOverwrite;
    };

    inline constexpr AdcRegularGroupOverrunBehavior AdcRegularGroupOverrunBehavior::kPreserve =
        AdcRegularGroupOverrunBehavior::from_ll<LL_ADC_REG_OVR_DATA_PRESERVED>();
    inline constexpr AdcRegularGroupOverrunBehavior AdcRegularGroupOverrunBehavior::kOverwrite =
        AdcRegularGroupOverrunBehavior::from_ll<LL_ADC_REG_OVR_DATA_OVERWRITTEN>();

    struct AdcRegularGroupConversionMode : public LLDriverEnumValue<AdcRegularGroupConversionMode>
    {
        static const AdcRegularGroupConversionMode kSingleShot;
        static const AdcRegularGroupConversionMode kContinuous;
    };

    inline constexpr AdcRegularGroupConversionMode AdcRegularGroupConversionMode::kSingleShot =
        AdcRegularGroupConversionMode::from_ll<LL_ADC_REG_CONV_SINGLE>();
    inline constexpr AdcRegularGroupConversionMode AdcRegularGroupConversionMode::kContinuous =
        AdcRegularGroupConversionMode::from_ll<LL_ADC_REG_CONV_CONTINUOUS>();

    struct AdcRegularGroupOversamplingMode : public LLDriverEnumValue<AdcRegularGroupOversamplingMode>
    {
        static const AdcRegularGroupOversamplingMode kDiscontinuous;
        static const AdcRegularGroupOversamplingMode kContinuous;
    };

    inline constexpr AdcRegularGroupOversamplingMode AdcRegularGroupOversamplingMode::kDiscontinuous =
        AdcRegularGroupOversamplingMode::from_ll<LL_ADC_OVS_REG_DISCONT>();
    inline constexpr AdcRegularGroupOversamplingMode AdcRegularGroupOversamplingMode::kContinuous =
        AdcRegularGroupOversamplingMode::from_ll<LL_ADC_OVS_REG_CONT>();

    struct AdcRegularGroupSamplingMode : public LLDriverEnumValue<AdcRegularGroupSamplingMode>
    {
        static const AdcRegularGroupSamplingMode kNormal;
        static const AdcRegularGroupSamplingMode kBulb;
        static const AdcRegularGroupSamplingMode kTriggerControlled;
    };

    inline constexpr AdcRegularGroupSamplingMode AdcRegularGroupSamplingMode::kNormal =
        AdcRegularGroupSamplingMode::from_ll<LL_ADC_REG_SAMPLING_MODE_NORMAL>();
    inline constexpr AdcRegularGroupSamplingMode AdcRegularGroupSamplingMode::kBulb =
        AdcRegularGroupSamplingMode::from_ll<LL_ADC_REG_SAMPLING_MODE_BULB>();
    inline constexpr AdcRegularGroupSamplingMode AdcRegularGroupSamplingMode::kTriggerControlled =
        AdcRegularGroupSamplingMode::from_ll<LL_ADC_REG_SAMPLING_MODE_TRIGGER_CONTROLED>();

    inline constexpr auto kAdcOversamplingRatioNumberMapping =
        LLDriverEnumValueNumberMapping<8>{.mapping = {
                                              std::make_pair(LL_ADC_OVS_RATIO_2, 2),
                                              std::make_pair(LL_ADC_OVS_RATIO_4, 4),
                                              std::make_pair(LL_ADC_OVS_RATIO_8, 8),
                                              std::make_pair(LL_ADC_OVS_RATIO_16, 16),
                                              std::make_pair(LL_ADC_OVS_RATIO_32, 32),
                                              std::make_pair(LL_ADC_OVS_RATIO_64, 64),
                                              std::make_pair(LL_ADC_OVS_RATIO_128, 128),
                                              std::make_pair(LL_ADC_OVS_RATIO_256, 256),
                                          }};

    struct AdcOversamplingRatio
        : public NumberedLLDriverEnumValue<AdcOversamplingRatio, kAdcOversamplingRatioNumberMapping>
    {
        static const AdcOversamplingRatio k2x;
        static const AdcOversamplingRatio k4x;
        static const AdcOversamplingRatio k8x;
        static const AdcOversamplingRatio k16x;
        static const AdcOversamplingRatio k32x;
        static const AdcOversamplingRatio k64x;
        static const AdcOversamplingRatio k128x;
        static const AdcOversamplingRatio k256x;
    };

    inline constexpr AdcOversamplingRatio AdcOversamplingRatio::k2x   = AdcOversamplingRatio::from_number<2>();
    inline constexpr AdcOversamplingRatio AdcOversamplingRatio::k4x   = AdcOversamplingRatio::from_number<4>();
    inline constexpr AdcOversamplingRatio AdcOversamplingRatio::k8x   = AdcOversamplingRatio::from_number<8>();
    inline constexpr AdcOversamplingRatio AdcOversamplingRatio::k16x  = AdcOversamplingRatio::from_number<16>();
    inline constexpr AdcOversamplingRatio AdcOversamplingRatio::k32x  = AdcOversamplingRatio::from_number<32>();
    inline constexpr AdcOversamplingRatio AdcOversamplingRatio::k64x  = AdcOversamplingRatio::from_number<64>();
    inline constexpr AdcOversamplingRatio AdcOversamplingRatio::k128x = AdcOversamplingRatio::from_number<128>();
    inline constexpr AdcOversamplingRatio AdcOversamplingRatio::k256x = AdcOversamplingRatio::from_number<256>();

    inline constexpr auto kAdcOversamplingShiftNumberMapping =
        LLDriverEnumValueNumberMapping<9>{.mapping = {
                                              std::make_pair(LL_ADC_OVS_SHIFT_NONE, 0U),
                                              std::make_pair(LL_ADC_OVS_SHIFT_RIGHT_1, 1U),
                                              std::make_pair(LL_ADC_OVS_SHIFT_RIGHT_2, 2U),
                                              std::make_pair(LL_ADC_OVS_SHIFT_RIGHT_3, 3U),
                                              std::make_pair(LL_ADC_OVS_SHIFT_RIGHT_4, 4U),
                                              std::make_pair(LL_ADC_OVS_SHIFT_RIGHT_5, 5U),
                                              std::make_pair(LL_ADC_OVS_SHIFT_RIGHT_6, 6U),
                                              std::make_pair(LL_ADC_OVS_SHIFT_RIGHT_7, 7U),
                                              std::make_pair(LL_ADC_OVS_SHIFT_RIGHT_8, 8U),
                                          }};

    struct AdcOversamplingShift
        : public NumberedLLDriverEnumValue<AdcOversamplingShift, kAdcOversamplingShiftNumberMapping>
    {
        static const AdcOversamplingShift kNone;
        static const AdcOversamplingShift kDiv2;
        static const AdcOversamplingShift kDiv4;
        static const AdcOversamplingShift kDiv8;
        static const AdcOversamplingShift kDiv16;
        static const AdcOversamplingShift kDiv32;
        static const AdcOversamplingShift kDiv64;
        static const AdcOversamplingShift kDiv128;
        static const AdcOversamplingShift kDiv256;
    };

    inline constexpr AdcOversamplingShift AdcOversamplingShift::kNone   = AdcOversamplingShift::from_number<0>();
    inline constexpr AdcOversamplingShift AdcOversamplingShift::kDiv2   = AdcOversamplingShift::from_number<1>();
    inline constexpr AdcOversamplingShift AdcOversamplingShift::kDiv4   = AdcOversamplingShift::from_number<2>();
    inline constexpr AdcOversamplingShift AdcOversamplingShift::kDiv8   = AdcOversamplingShift::from_number<3>();
    inline constexpr AdcOversamplingShift AdcOversamplingShift::kDiv16  = AdcOversamplingShift::from_number<4>();
    inline constexpr AdcOversamplingShift AdcOversamplingShift::kDiv32  = AdcOversamplingShift::from_number<5>();
    inline constexpr AdcOversamplingShift AdcOversamplingShift::kDiv64  = AdcOversamplingShift::from_number<6>();
    inline constexpr AdcOversamplingShift AdcOversamplingShift::kDiv128 = AdcOversamplingShift::from_number<7>();
    inline constexpr AdcOversamplingShift AdcOversamplingShift::kDiv256 = AdcOversamplingShift::from_number<8>();

    struct AdcOversamplingScope : public LLDriverEnumValue<AdcOversamplingScope>
    {
        static const AdcOversamplingScope kDisable;
        static const AdcOversamplingScope kRegularContinued;
        static const AdcOversamplingScope kRegularResumed;
        static const AdcOversamplingScope kInject;
        static const AdcOversamplingScope kInjectRegularResumed;
    };

    inline constexpr AdcOversamplingScope AdcOversamplingScope::kDisable =
        AdcOversamplingScope::from_ll<LL_ADC_OVS_DISABLE>();
    inline constexpr AdcOversamplingScope AdcOversamplingScope::kRegularContinued =
        AdcOversamplingScope::from_ll<LL_ADC_OVS_GRP_REGULAR_CONTINUED>();
    inline constexpr AdcOversamplingScope AdcOversamplingScope::kRegularResumed =
        AdcOversamplingScope::from_ll<LL_ADC_OVS_GRP_REGULAR_RESUMED>();
    inline constexpr AdcOversamplingScope AdcOversamplingScope::kInject =
        AdcOversamplingScope::from_ll<LL_ADC_OVS_GRP_INJECTED>();
    inline constexpr AdcOversamplingScope AdcOversamplingScope::kInjectRegularResumed =
        AdcOversamplingScope::from_ll<LL_ADC_OVS_GRP_INJ_REG_RESUMED>();

    struct AdcChannelGroup : public LLDriverEnumValue<AdcChannelGroup>
    {
        static const AdcChannelGroup kRegular;
        static const AdcChannelGroup kInject;
    };

    inline constexpr AdcChannelGroup AdcChannelGroup::kRegular = AdcChannelGroup::from_ll<0>();
    inline constexpr AdcChannelGroup AdcChannelGroup::kInject  = AdcChannelGroup::from_ll<0>();

    struct AdcChannelSampleTime : public LLDriverEnumValue<AdcChannelSampleTime>
    {
        static const AdcChannelSampleTime k2Cycles5;
        static const AdcChannelSampleTime k6Cycles5;
        static const AdcChannelSampleTime k12Cycles5;
        static const AdcChannelSampleTime k24Cycles5;
        static const AdcChannelSampleTime k47Cycles5;
        static const AdcChannelSampleTime k92Cycles5;
        static const AdcChannelSampleTime k247Cycles5;
        static const AdcChannelSampleTime k640Cycles5;

        [[nodiscard]] constexpr float to_cyles() const
        {
            // NOLINTBEGIN(readability-magic-numbers)
            switch (to_ll())
            {
                case k2Cycles5.to_ll():
                    return 2.5F;
                case k6Cycles5.to_ll():
                    return 6.5F;
                case k12Cycles5.to_ll():
                    return 12.5F;
                case k24Cycles5.to_ll():
                    return 24.5F;
                case k47Cycles5.to_ll():
                    return 47.5F;
                case k92Cycles5.to_ll():
                    return 92.5F;
                case k247Cycles5.to_ll():
                    return 247.5F;
                case k640Cycles5.to_ll():
                    return 640.5F;
                default:
                    expect(false, "Invalid sample time");  // Invalid sample time
                    return 0.0F;
            }
            // NOLINTEND(readability-magic-numbers)
        }
    };

    inline constexpr AdcChannelSampleTime AdcChannelSampleTime::k2Cycles5 =
        AdcChannelSampleTime::from_ll<LL_ADC_SAMPLINGTIME_2CYCLES_5>();
    inline constexpr AdcChannelSampleTime AdcChannelSampleTime::k6Cycles5 =
        AdcChannelSampleTime::from_ll<LL_ADC_SAMPLINGTIME_6CYCLES_5>();
    inline constexpr AdcChannelSampleTime AdcChannelSampleTime::k12Cycles5 =
        AdcChannelSampleTime::from_ll<LL_ADC_SAMPLINGTIME_12CYCLES_5>();
    inline constexpr AdcChannelSampleTime AdcChannelSampleTime::k24Cycles5 =
        AdcChannelSampleTime::from_ll<LL_ADC_SAMPLINGTIME_24CYCLES_5>();
    inline constexpr AdcChannelSampleTime AdcChannelSampleTime::k47Cycles5 =
        AdcChannelSampleTime::from_ll<LL_ADC_SAMPLINGTIME_47CYCLES_5>();
    inline constexpr AdcChannelSampleTime AdcChannelSampleTime::k92Cycles5 =
        AdcChannelSampleTime::from_ll<LL_ADC_SAMPLINGTIME_92CYCLES_5>();
    inline constexpr AdcChannelSampleTime AdcChannelSampleTime::k247Cycles5 =
        AdcChannelSampleTime::from_ll<LL_ADC_SAMPLINGTIME_247CYCLES_5>();
    inline constexpr AdcChannelSampleTime AdcChannelSampleTime::k640Cycles5 =
        AdcChannelSampleTime::from_ll<LL_ADC_SAMPLINGTIME_640CYCLES_5>();

    struct AdcChannelInputMode : public LLDriverEnumValue<AdcChannelInputMode>
    {
        static const AdcChannelInputMode kSingleEnded;
        static const AdcChannelInputMode kDifferential;
    };

    inline constexpr AdcChannelInputMode AdcChannelInputMode::kSingleEnded =
        AdcChannelInputMode::from_ll<LL_ADC_SINGLE_ENDED>();
    inline constexpr AdcChannelInputMode AdcChannelInputMode::kDifferential =
        AdcChannelInputMode::from_ll<LL_ADC_DIFFERENTIAL_ENDED>();

    struct AdcControllerInterruptSource : public LLDriverEnumValue<AdcControllerInterruptSource>
    {
        static const AdcControllerInterruptSource kReady;
        static const AdcControllerInterruptSource kRegularEndOfConversion;
        static const AdcControllerInterruptSource kRegularEndOfSequence;
        static const AdcControllerInterruptSource kRegularEndOfSampling;
        static const AdcControllerInterruptSource kInjectEndOfConversion;
        static const AdcControllerInterruptSource kInjectEndOfSequence;
        static const AdcControllerInterruptSource kInjectContextQueueOverflow;
        static const AdcControllerInterruptSource kOverrun;
        static const AdcControllerInterruptSource kAnalogWatchdog1;
        static const AdcControllerInterruptSource kAnalogWatchdog2;
        static const AdcControllerInterruptSource kAnalogWatchdog3;
    };

    inline constexpr AdcControllerInterruptSource AdcControllerInterruptSource::kReady =
        AdcControllerInterruptSource::from_ll<0>();
    inline constexpr AdcControllerInterruptSource AdcControllerInterruptSource::kRegularEndOfConversion =
        AdcControllerInterruptSource::from_ll<0>();
    inline constexpr AdcControllerInterruptSource AdcControllerInterruptSource::kRegularEndOfSequence =
        AdcControllerInterruptSource::from_ll<0>();
    inline constexpr AdcControllerInterruptSource AdcControllerInterruptSource::kRegularEndOfSampling =
        AdcControllerInterruptSource::from_ll<0>();
    inline constexpr AdcControllerInterruptSource AdcControllerInterruptSource::kInjectEndOfConversion =
        AdcControllerInterruptSource::from_ll<0>();
    inline constexpr AdcControllerInterruptSource AdcControllerInterruptSource::kInjectEndOfSequence =
        AdcControllerInterruptSource::from_ll<0>();
    inline constexpr AdcControllerInterruptSource AdcControllerInterruptSource::kInjectContextQueueOverflow =
        AdcControllerInterruptSource::from_ll<0>();
    inline constexpr AdcControllerInterruptSource AdcControllerInterruptSource::kOverrun =
        AdcControllerInterruptSource::from_ll<0>();
    inline constexpr AdcControllerInterruptSource AdcControllerInterruptSource::kAnalogWatchdog1 =
        AdcControllerInterruptSource::from_ll<0>();
    inline constexpr AdcControllerInterruptSource AdcControllerInterruptSource::kAnalogWatchdog2 =
        AdcControllerInterruptSource::from_ll<0>();
    inline constexpr AdcControllerInterruptSource AdcControllerInterruptSource::kAnalogWatchdog3 =
        AdcControllerInterruptSource::from_ll<0>();

    struct AdcDmaRegister : public LLDriverEnumValue<AdcDmaRegister>
    {
        static const AdcDmaRegister kRegularData;
        static const AdcDmaRegister kRegularDataMulti;
    };

    inline constexpr AdcDmaRegister AdcDmaRegister::kRegularData =
        AdcDmaRegister::from_ll<LL_ADC_DMA_REG_REGULAR_DATA>();
    inline constexpr AdcDmaRegister AdcDmaRegister::kRegularDataMulti =
        AdcDmaRegister::from_ll<LL_ADC_DMA_REG_REGULAR_DATA_MULTI>();

    struct AdcDifferentialMode : public LLDriverEnumValue<AdcDifferentialMode>
    {
        static const AdcDifferentialMode kSingleEnded;
        static const AdcDifferentialMode kDifferential;
        static const AdcDifferentialMode kSingleDifferential;
    };

    inline constexpr AdcDifferentialMode AdcDifferentialMode::kSingleEnded =
        AdcDifferentialMode::from_ll<LL_ADC_SINGLE_ENDED>();
    inline constexpr AdcDifferentialMode AdcDifferentialMode::kDifferential =
        AdcDifferentialMode::from_ll<LL_ADC_DIFFERENTIAL_ENDED>();
    inline constexpr AdcDifferentialMode AdcDifferentialMode::kSingleDifferential =
        AdcDifferentialMode::from_ll<LL_ADC_BOTH_SINGLE_DIFF_ENDED>();

    struct AdcAnalogWatchdogChannelGroup : public LLDriverEnumValue<AdcAnalogWatchdogChannelGroup>
    {
        static const AdcAnalogWatchdogChannelGroup kDisable;
        static const AdcAnalogWatchdogChannelGroup kAllRegularChannels;
        static const AdcAnalogWatchdogChannelGroup kAllInjectedChannels;
        static const AdcAnalogWatchdogChannelGroup kAllRegularAndInjectedChannels;
        static const AdcAnalogWatchdogChannelGroup kChannel0Regular;
        static const AdcAnalogWatchdogChannelGroup kChannel0Injected;
        static const AdcAnalogWatchdogChannelGroup kChannel0RegularAndInjected;
        static const AdcAnalogWatchdogChannelGroup kChannel1Regular;
        static const AdcAnalogWatchdogChannelGroup kChannel1Injected;
        static const AdcAnalogWatchdogChannelGroup kChannel1RegularAndInjected;
        static const AdcAnalogWatchdogChannelGroup kChannel2Regular;
        static const AdcAnalogWatchdogChannelGroup kChannel2Injected;
        static const AdcAnalogWatchdogChannelGroup kChannel2RegularAndInjected;
        static const AdcAnalogWatchdogChannelGroup kChannel3Regular;
        static const AdcAnalogWatchdogChannelGroup kChannel3Injected;
        static const AdcAnalogWatchdogChannelGroup kChannel3RegularAndInjected;
        static const AdcAnalogWatchdogChannelGroup kChannel4Regular;
        static const AdcAnalogWatchdogChannelGroup kChannel4Injected;
        static const AdcAnalogWatchdogChannelGroup kChannel4RegularAndInjected;
        static const AdcAnalogWatchdogChannelGroup kChannel5Regular;
        static const AdcAnalogWatchdogChannelGroup kChannel5Injected;
        static const AdcAnalogWatchdogChannelGroup kChannel5RegularAndInjected;
        static const AdcAnalogWatchdogChannelGroup kChannel6Regular;
        static const AdcAnalogWatchdogChannelGroup kChannel6Injected;
        static const AdcAnalogWatchdogChannelGroup kChannel6RegularAndInjected;
        static const AdcAnalogWatchdogChannelGroup kChannel7Regular;
        static const AdcAnalogWatchdogChannelGroup kChannel7Injected;
        static const AdcAnalogWatchdogChannelGroup kChannel7RegularAndInjected;
        static const AdcAnalogWatchdogChannelGroup kChannel8Regular;
        static const AdcAnalogWatchdogChannelGroup kChannel8Injected;
        static const AdcAnalogWatchdogChannelGroup kChannel8RegularAndInjected;
        static const AdcAnalogWatchdogChannelGroup kChannel9Regular;
        static const AdcAnalogWatchdogChannelGroup kChannel9Injected;
        static const AdcAnalogWatchdogChannelGroup kChannel9RegularAndInjected;
        static const AdcAnalogWatchdogChannelGroup kChannel10Regular;
        static const AdcAnalogWatchdogChannelGroup kChannel10Injected;
        static const AdcAnalogWatchdogChannelGroup kChannel10RegularAndInjected;
        static const AdcAnalogWatchdogChannelGroup kChannel11Regular;
        static const AdcAnalogWatchdogChannelGroup kChannel11Injected;
        static const AdcAnalogWatchdogChannelGroup kChannel11RegularAndInjected;
        static const AdcAnalogWatchdogChannelGroup kChannel12Regular;
        static const AdcAnalogWatchdogChannelGroup kChannel12Injected;
        static const AdcAnalogWatchdogChannelGroup kChannel12RegularAndInjected;
        static const AdcAnalogWatchdogChannelGroup kChannel13Regular;
        static const AdcAnalogWatchdogChannelGroup kChannel13Injected;
        static const AdcAnalogWatchdogChannelGroup kChannel13RegularAndInjected;
        static const AdcAnalogWatchdogChannelGroup kChannel14Regular;
        static const AdcAnalogWatchdogChannelGroup kChannel14Injected;
        static const AdcAnalogWatchdogChannelGroup kChannel14RegularAndInjected;
        static const AdcAnalogWatchdogChannelGroup kChannel15Regular;
        static const AdcAnalogWatchdogChannelGroup kChannel15Injected;
        static const AdcAnalogWatchdogChannelGroup kChannel15RegularAndInjected;
        static const AdcAnalogWatchdogChannelGroup kChannel16Regular;
        static const AdcAnalogWatchdogChannelGroup kChannel16Injected;
        static const AdcAnalogWatchdogChannelGroup kChannel16RegularAndInjected;
        static const AdcAnalogWatchdogChannelGroup kChannel17Regular;
        static const AdcAnalogWatchdogChannelGroup kChannel17Injected;
        static const AdcAnalogWatchdogChannelGroup kChannel17RegularAndInjected;
        static const AdcAnalogWatchdogChannelGroup kChannel18Regular;
        static const AdcAnalogWatchdogChannelGroup kChannel18Injected;
        static const AdcAnalogWatchdogChannelGroup kChannel18RegularAndInjected;
        static const AdcAnalogWatchdogChannelGroup kChannelVrefintRegular;
        static const AdcAnalogWatchdogChannelGroup kChannelVrefintInjected;
        static const AdcAnalogWatchdogChannelGroup kChannelVrefintRegularAndInjected;
        static const AdcAnalogWatchdogChannelGroup kChannelTempsensorAdc1Regular;
        static const AdcAnalogWatchdogChannelGroup kChannelTempsensorAdc1Injected;
        static const AdcAnalogWatchdogChannelGroup kChannelTempsensorAdc1RegularAndInjected;
        static const AdcAnalogWatchdogChannelGroup kChannelTempsensorAdc5Regular;
        static const AdcAnalogWatchdogChannelGroup kChannelTempsensorAdc5Injected;
        static const AdcAnalogWatchdogChannelGroup kChannelTempsensorAdc5RegularAndInjected;
        static const AdcAnalogWatchdogChannelGroup kChannelVbatRegular;
        static const AdcAnalogWatchdogChannelGroup kChannelVbatInjected;
        static const AdcAnalogWatchdogChannelGroup kChannelVbatRegularAndInjected;
        static const AdcAnalogWatchdogChannelGroup kChannelVopamp1Regular;
        static const AdcAnalogWatchdogChannelGroup kChannelVopamp1Injected;
        static const AdcAnalogWatchdogChannelGroup kChannelVopamp1RegularAndInjected;
        static const AdcAnalogWatchdogChannelGroup kChannelVopamp2Regular;
        static const AdcAnalogWatchdogChannelGroup kChannelVopamp2Injected;
        static const AdcAnalogWatchdogChannelGroup kChannelVopamp2RegularAndInjected;
        static const AdcAnalogWatchdogChannelGroup kChannelVopamp3Adc2Regular;
        static const AdcAnalogWatchdogChannelGroup kChannelVopamp3Adc2Injected;
        static const AdcAnalogWatchdogChannelGroup kChannelVopamp3Adc2RegularAndInjected;
        static const AdcAnalogWatchdogChannelGroup kChannelVopamp3Adc3Regular;
        static const AdcAnalogWatchdogChannelGroup kChannelVopamp3Adc3Injected;
        static const AdcAnalogWatchdogChannelGroup kChannelVopamp3Adc3RegularAndInjected;
        static const AdcAnalogWatchdogChannelGroup kChannelVopamp4Regular;
        static const AdcAnalogWatchdogChannelGroup kChannelVopamp4Injected;
        static const AdcAnalogWatchdogChannelGroup kChannelVopamp4RegularAndInjected;
        static const AdcAnalogWatchdogChannelGroup kChannelVopamp5Regular;
        static const AdcAnalogWatchdogChannelGroup kChannelVopamp5Injected;
        static const AdcAnalogWatchdogChannelGroup kChannelVopamp5RegularAndInjected;
        static const AdcAnalogWatchdogChannelGroup kChannelVopamp6Regular;
        static const AdcAnalogWatchdogChannelGroup kChannelVopamp6Injected;
        static const AdcAnalogWatchdogChannelGroup kChannelVopamp6RegularAndInjected;

        [[nodiscard]] static AdcAnalogWatchdogChannelGroup from_channel(const AdcChannelId channel,
                                                                        const bool         regular,
                                                                        const bool         injected)
        {
#define CHANNEL_CASE(channel_name)                   \
    case AdcChannelId::##channel_name.to_ll():       \
        if (regular && injected)                     \
        {                                            \
            return channel_name##RegularAndInjected; \
        }                                            \
        else if (regular)                            \
        {                                            \
            return channel_name##Regular;            \
        }                                            \
        else if (injected)                           \
        {                                            \
            return channel_name##Injected;           \
        }                                            \
        break;

            switch (channel.to_ll())
            {
                CHANNEL_CASE(kChannel0);
                CHANNEL_CASE(kChannel1);
                CHANNEL_CASE(kChannel2);
                CHANNEL_CASE(kChannel3);
                CHANNEL_CASE(kChannel4);
                CHANNEL_CASE(kChannel5);
                CHANNEL_CASE(kChannel6);
                CHANNEL_CASE(kChannel7);
                CHANNEL_CASE(kChannel8);
                CHANNEL_CASE(kChannel9);
                CHANNEL_CASE(kChannel10);
                CHANNEL_CASE(kChannel11);
                CHANNEL_CASE(kChannel12);
                CHANNEL_CASE(kChannel13);
                CHANNEL_CASE(kChannel14);
                CHANNEL_CASE(kChannel15);
                CHANNEL_CASE(kChannel16);
                CHANNEL_CASE(kChannel17);
                CHANNEL_CASE(kChannel18);
                CHANNEL_CASE(kVrefint);
                CHANNEL_CASE(kTempsensorAdc1);
                CHANNEL_CASE(kTempsensorAdc5);
                CHANNEL_CASE(kVbat);
                CHANNEL_CASE(kVopamp1);
                CHANNEL_CASE(kVopamp2);
                CHANNEL_CASE(kVopamp3Adc2);
                CHANNEL_CASE(kVopamp3Adc3);
                CHANNEL_CASE(kVopamp4);
                CHANNEL_CASE(kVopamp5);
                CHANNEL_CASE(kVopamp6);
                default:
                    expect(false, "Invalid channel for watchdog group");  // Invalid channel for watchdog group
                    return kDisable;
            }

#undef CHANNEL_CASE
        }
    };

    inline constexpr AdcAnalogWatchdogChannelGroup AdcAnalogWatchdogChannelGroup::kDisable =
        AdcAnalogWatchdogChannelGroup::from_ll<LL_ADC_AWD_DISABLE>();
    inline constexpr AdcAnalogWatchdogChannelGroup AdcAnalogWatchdogChannelGroup::kAllRegularChannels =
        AdcAnalogWatchdogChannelGroup::from_ll<LL_ADC_AWD_ALL_CHANNELS_REG>();
    inline constexpr AdcAnalogWatchdogChannelGroup AdcAnalogWatchdogChannelGroup::kAllInjectedChannels =
        AdcAnalogWatchdogChannelGroup::from_ll<LL_ADC_AWD_ALL_CHANNELS_INJ>();
    inline constexpr AdcAnalogWatchdogChannelGroup AdcAnalogWatchdogChannelGroup::kAllRegularAndInjectedChannels =
        AdcAnalogWatchdogChannelGroup::from_ll<LL_ADC_AWD_ALL_CHANNELS_REG_INJ>();
    inline constexpr AdcAnalogWatchdogChannelGroup AdcAnalogWatchdogChannelGroup::kChannel0Regular =
        AdcAnalogWatchdogChannelGroup::from_ll<LL_ADC_AWD_CHANNEL_0_REG>();
    inline constexpr AdcAnalogWatchdogChannelGroup AdcAnalogWatchdogChannelGroup::kChannel0Injected =
        AdcAnalogWatchdogChannelGroup::from_ll<LL_ADC_AWD_CHANNEL_0_INJ>();
    inline constexpr AdcAnalogWatchdogChannelGroup AdcAnalogWatchdogChannelGroup::kChannel0RegularAndInjected =
        AdcAnalogWatchdogChannelGroup::from_ll<LL_ADC_AWD_CHANNEL_0_REG_INJ>();
    inline constexpr AdcAnalogWatchdogChannelGroup AdcAnalogWatchdogChannelGroup::kChannel1Regular =
        AdcAnalogWatchdogChannelGroup::from_ll<LL_ADC_AWD_CHANNEL_1_REG>();
    inline constexpr AdcAnalogWatchdogChannelGroup AdcAnalogWatchdogChannelGroup::kChannel1Injected =
        AdcAnalogWatchdogChannelGroup::from_ll<LL_ADC_AWD_CHANNEL_1_INJ>();
    inline constexpr AdcAnalogWatchdogChannelGroup AdcAnalogWatchdogChannelGroup::kChannel1RegularAndInjected =
        AdcAnalogWatchdogChannelGroup::from_ll<LL_ADC_AWD_CHANNEL_1_REG_INJ>();
    inline constexpr AdcAnalogWatchdogChannelGroup AdcAnalogWatchdogChannelGroup::kChannel2Regular =
        AdcAnalogWatchdogChannelGroup::from_ll<LL_ADC_AWD_CHANNEL_2_REG>();
    inline constexpr AdcAnalogWatchdogChannelGroup AdcAnalogWatchdogChannelGroup::kChannel2Injected =
        AdcAnalogWatchdogChannelGroup::from_ll<LL_ADC_AWD_CHANNEL_2_INJ>();
    inline constexpr AdcAnalogWatchdogChannelGroup AdcAnalogWatchdogChannelGroup::kChannel2RegularAndInjected =
        AdcAnalogWatchdogChannelGroup::from_ll<LL_ADC_AWD_CHANNEL_2_REG_INJ>();
    inline constexpr AdcAnalogWatchdogChannelGroup AdcAnalogWatchdogChannelGroup::kChannel3Regular =
        AdcAnalogWatchdogChannelGroup::from_ll<LL_ADC_AWD_CHANNEL_3_REG>();
    inline constexpr AdcAnalogWatchdogChannelGroup AdcAnalogWatchdogChannelGroup::kChannel3Injected =
        AdcAnalogWatchdogChannelGroup::from_ll<LL_ADC_AWD_CHANNEL_3_INJ>();
    inline constexpr AdcAnalogWatchdogChannelGroup AdcAnalogWatchdogChannelGroup::kChannel3RegularAndInjected =
        AdcAnalogWatchdogChannelGroup::from_ll<LL_ADC_AWD_CHANNEL_3_REG_INJ>();
    inline constexpr AdcAnalogWatchdogChannelGroup AdcAnalogWatchdogChannelGroup::kChannel4Regular =
        AdcAnalogWatchdogChannelGroup::from_ll<LL_ADC_AWD_CHANNEL_4_REG>();
    inline constexpr AdcAnalogWatchdogChannelGroup AdcAnalogWatchdogChannelGroup::kChannel4Injected =
        AdcAnalogWatchdogChannelGroup::from_ll<LL_ADC_AWD_CHANNEL_4_INJ>();
    inline constexpr AdcAnalogWatchdogChannelGroup AdcAnalogWatchdogChannelGroup::kChannel4RegularAndInjected =
        AdcAnalogWatchdogChannelGroup::from_ll<LL_ADC_AWD_CHANNEL_4_REG_INJ>();
    inline constexpr AdcAnalogWatchdogChannelGroup AdcAnalogWatchdogChannelGroup::kChannel5Regular =
        AdcAnalogWatchdogChannelGroup::from_ll<LL_ADC_AWD_CHANNEL_5_REG>();
    inline constexpr AdcAnalogWatchdogChannelGroup AdcAnalogWatchdogChannelGroup::kChannel5Injected =
        AdcAnalogWatchdogChannelGroup::from_ll<LL_ADC_AWD_CHANNEL_5_INJ>();
    inline constexpr AdcAnalogWatchdogChannelGroup AdcAnalogWatchdogChannelGroup::kChannel5RegularAndInjected =
        AdcAnalogWatchdogChannelGroup::from_ll<LL_ADC_AWD_CHANNEL_5_REG_INJ>();
    inline constexpr AdcAnalogWatchdogChannelGroup AdcAnalogWatchdogChannelGroup::kChannel6Regular =
        AdcAnalogWatchdogChannelGroup::from_ll<LL_ADC_AWD_CHANNEL_6_REG>();
    inline constexpr AdcAnalogWatchdogChannelGroup AdcAnalogWatchdogChannelGroup::kChannel6Injected =
        AdcAnalogWatchdogChannelGroup::from_ll<LL_ADC_AWD_CHANNEL_6_INJ>();
    inline constexpr AdcAnalogWatchdogChannelGroup AdcAnalogWatchdogChannelGroup::kChannel6RegularAndInjected =
        AdcAnalogWatchdogChannelGroup::from_ll<LL_ADC_AWD_CHANNEL_6_REG_INJ>();
    inline constexpr AdcAnalogWatchdogChannelGroup AdcAnalogWatchdogChannelGroup::kChannel7Regular =
        AdcAnalogWatchdogChannelGroup::from_ll<LL_ADC_AWD_CHANNEL_7_REG>();
    inline constexpr AdcAnalogWatchdogChannelGroup AdcAnalogWatchdogChannelGroup::kChannel7Injected =
        AdcAnalogWatchdogChannelGroup::from_ll<LL_ADC_AWD_CHANNEL_7_INJ>();
    inline constexpr AdcAnalogWatchdogChannelGroup AdcAnalogWatchdogChannelGroup::kChannel7RegularAndInjected =
        AdcAnalogWatchdogChannelGroup::from_ll<LL_ADC_AWD_CHANNEL_7_REG_INJ>();
    inline constexpr AdcAnalogWatchdogChannelGroup AdcAnalogWatchdogChannelGroup::kChannel8Regular =
        AdcAnalogWatchdogChannelGroup::from_ll<LL_ADC_AWD_CHANNEL_8_REG>();
    inline constexpr AdcAnalogWatchdogChannelGroup AdcAnalogWatchdogChannelGroup::kChannel8Injected =
        AdcAnalogWatchdogChannelGroup::from_ll<LL_ADC_AWD_CHANNEL_8_INJ>();
    inline constexpr AdcAnalogWatchdogChannelGroup AdcAnalogWatchdogChannelGroup::kChannel8RegularAndInjected =
        AdcAnalogWatchdogChannelGroup::from_ll<LL_ADC_AWD_CHANNEL_8_REG_INJ>();
    inline constexpr AdcAnalogWatchdogChannelGroup AdcAnalogWatchdogChannelGroup::kChannel9Regular =
        AdcAnalogWatchdogChannelGroup::from_ll<LL_ADC_AWD_CHANNEL_9_REG>();
    inline constexpr AdcAnalogWatchdogChannelGroup AdcAnalogWatchdogChannelGroup::kChannel9Injected =
        AdcAnalogWatchdogChannelGroup::from_ll<LL_ADC_AWD_CHANNEL_9_INJ>();
    inline constexpr AdcAnalogWatchdogChannelGroup AdcAnalogWatchdogChannelGroup::kChannel9RegularAndInjected =
        AdcAnalogWatchdogChannelGroup::from_ll<LL_ADC_AWD_CHANNEL_9_REG_INJ>();
    inline constexpr AdcAnalogWatchdogChannelGroup AdcAnalogWatchdogChannelGroup::kChannel10Regular =
        AdcAnalogWatchdogChannelGroup::from_ll<LL_ADC_AWD_CHANNEL_10_REG>();
    inline constexpr AdcAnalogWatchdogChannelGroup AdcAnalogWatchdogChannelGroup::kChannel10Injected =
        AdcAnalogWatchdogChannelGroup::from_ll<LL_ADC_AWD_CHANNEL_10_INJ>();
    inline constexpr AdcAnalogWatchdogChannelGroup AdcAnalogWatchdogChannelGroup::kChannel10RegularAndInjected =
        AdcAnalogWatchdogChannelGroup::from_ll<LL_ADC_AWD_CHANNEL_10_REG_INJ>();
    inline constexpr AdcAnalogWatchdogChannelGroup AdcAnalogWatchdogChannelGroup::kChannel11Regular =
        AdcAnalogWatchdogChannelGroup::from_ll<LL_ADC_AWD_CHANNEL_11_REG>();
    inline constexpr AdcAnalogWatchdogChannelGroup AdcAnalogWatchdogChannelGroup::kChannel11Injected =
        AdcAnalogWatchdogChannelGroup::from_ll<LL_ADC_AWD_CHANNEL_11_INJ>();
    inline constexpr AdcAnalogWatchdogChannelGroup AdcAnalogWatchdogChannelGroup::kChannel11RegularAndInjected =
        AdcAnalogWatchdogChannelGroup::from_ll<LL_ADC_AWD_CHANNEL_11_REG_INJ>();
    inline constexpr AdcAnalogWatchdogChannelGroup AdcAnalogWatchdogChannelGroup::kChannel12Regular =
        AdcAnalogWatchdogChannelGroup::from_ll<LL_ADC_AWD_CHANNEL_12_REG>();
    inline constexpr AdcAnalogWatchdogChannelGroup AdcAnalogWatchdogChannelGroup::kChannel12Injected =
        AdcAnalogWatchdogChannelGroup::from_ll<LL_ADC_AWD_CHANNEL_12_INJ>();
    inline constexpr AdcAnalogWatchdogChannelGroup AdcAnalogWatchdogChannelGroup::kChannel12RegularAndInjected =
        AdcAnalogWatchdogChannelGroup::from_ll<LL_ADC_AWD_CHANNEL_12_REG_INJ>();
    inline constexpr AdcAnalogWatchdogChannelGroup AdcAnalogWatchdogChannelGroup::kChannel13Regular =
        AdcAnalogWatchdogChannelGroup::from_ll<LL_ADC_AWD_CHANNEL_13_REG>();
    inline constexpr AdcAnalogWatchdogChannelGroup AdcAnalogWatchdogChannelGroup::kChannel13Injected =
        AdcAnalogWatchdogChannelGroup::from_ll<LL_ADC_AWD_CHANNEL_13_INJ>();
    inline constexpr AdcAnalogWatchdogChannelGroup AdcAnalogWatchdogChannelGroup::kChannel13RegularAndInjected =
        AdcAnalogWatchdogChannelGroup::from_ll<LL_ADC_AWD_CHANNEL_13_REG_INJ>();
    inline constexpr AdcAnalogWatchdogChannelGroup AdcAnalogWatchdogChannelGroup::kChannel14Regular =
        AdcAnalogWatchdogChannelGroup::from_ll<LL_ADC_AWD_CHANNEL_14_REG>();
    inline constexpr AdcAnalogWatchdogChannelGroup AdcAnalogWatchdogChannelGroup::kChannel14Injected =
        AdcAnalogWatchdogChannelGroup::from_ll<LL_ADC_AWD_CHANNEL_14_INJ>();
    inline constexpr AdcAnalogWatchdogChannelGroup AdcAnalogWatchdogChannelGroup::kChannel14RegularAndInjected =
        AdcAnalogWatchdogChannelGroup::from_ll<LL_ADC_AWD_CHANNEL_14_REG_INJ>();
    inline constexpr AdcAnalogWatchdogChannelGroup AdcAnalogWatchdogChannelGroup::kChannel15Regular =
        AdcAnalogWatchdogChannelGroup::from_ll<LL_ADC_AWD_CHANNEL_15_REG>();
    inline constexpr AdcAnalogWatchdogChannelGroup AdcAnalogWatchdogChannelGroup::kChannel15Injected =
        AdcAnalogWatchdogChannelGroup::from_ll<LL_ADC_AWD_CHANNEL_15_INJ>();
    inline constexpr AdcAnalogWatchdogChannelGroup AdcAnalogWatchdogChannelGroup::kChannel15RegularAndInjected =
        AdcAnalogWatchdogChannelGroup::from_ll<LL_ADC_AWD_CHANNEL_15_REG_INJ>();
    inline constexpr AdcAnalogWatchdogChannelGroup AdcAnalogWatchdogChannelGroup::kChannel16Regular =
        AdcAnalogWatchdogChannelGroup::from_ll<LL_ADC_AWD_CHANNEL_16_REG>();
    inline constexpr AdcAnalogWatchdogChannelGroup AdcAnalogWatchdogChannelGroup::kChannel16Injected =
        AdcAnalogWatchdogChannelGroup::from_ll<LL_ADC_AWD_CHANNEL_16_INJ>();
    inline constexpr AdcAnalogWatchdogChannelGroup AdcAnalogWatchdogChannelGroup::kChannel16RegularAndInjected =
        AdcAnalogWatchdogChannelGroup::from_ll<LL_ADC_AWD_CHANNEL_16_REG_INJ>();
    inline constexpr AdcAnalogWatchdogChannelGroup AdcAnalogWatchdogChannelGroup::kChannel17Regular =
        AdcAnalogWatchdogChannelGroup::from_ll<LL_ADC_AWD_CHANNEL_17_REG>();
    inline constexpr AdcAnalogWatchdogChannelGroup AdcAnalogWatchdogChannelGroup::kChannel17Injected =
        AdcAnalogWatchdogChannelGroup::from_ll<LL_ADC_AWD_CHANNEL_17_INJ>();
    inline constexpr AdcAnalogWatchdogChannelGroup AdcAnalogWatchdogChannelGroup::kChannel17RegularAndInjected =
        AdcAnalogWatchdogChannelGroup::from_ll<LL_ADC_AWD_CHANNEL_17_REG_INJ>();
    inline constexpr AdcAnalogWatchdogChannelGroup AdcAnalogWatchdogChannelGroup::kChannel18Regular =
        AdcAnalogWatchdogChannelGroup::from_ll<LL_ADC_AWD_CHANNEL_18_REG>();
    inline constexpr AdcAnalogWatchdogChannelGroup AdcAnalogWatchdogChannelGroup::kChannel18Injected =
        AdcAnalogWatchdogChannelGroup::from_ll<LL_ADC_AWD_CHANNEL_18_INJ>();
    inline constexpr AdcAnalogWatchdogChannelGroup AdcAnalogWatchdogChannelGroup::kChannel18RegularAndInjected =
        AdcAnalogWatchdogChannelGroup::from_ll<LL_ADC_AWD_CHANNEL_18_REG_INJ>();
    inline constexpr AdcAnalogWatchdogChannelGroup AdcAnalogWatchdogChannelGroup::kChannelVrefintRegular =
        AdcAnalogWatchdogChannelGroup::from_ll<LL_ADC_AWD_CH_VREFINT_REG>();
    inline constexpr AdcAnalogWatchdogChannelGroup AdcAnalogWatchdogChannelGroup::kChannelVrefintInjected =
        AdcAnalogWatchdogChannelGroup::from_ll<LL_ADC_AWD_CH_VREFINT_INJ>();
    inline constexpr AdcAnalogWatchdogChannelGroup AdcAnalogWatchdogChannelGroup::kChannelVrefintRegularAndInjected =
        AdcAnalogWatchdogChannelGroup::from_ll<LL_ADC_AWD_CH_VREFINT_REG_INJ>();
    inline constexpr AdcAnalogWatchdogChannelGroup AdcAnalogWatchdogChannelGroup::kChannelTempsensorAdc1Regular =
        AdcAnalogWatchdogChannelGroup::from_ll<LL_ADC_AWD_CH_TEMPSENSOR_ADC1_REG>();
    inline constexpr AdcAnalogWatchdogChannelGroup AdcAnalogWatchdogChannelGroup::kChannelTempsensorAdc1Injected =
        AdcAnalogWatchdogChannelGroup::from_ll<LL_ADC_AWD_CH_TEMPSENSOR_ADC1_INJ>();
    inline constexpr AdcAnalogWatchdogChannelGroup
        AdcAnalogWatchdogChannelGroup::kChannelTempsensorAdc1RegularAndInjected =
            AdcAnalogWatchdogChannelGroup::from_ll<LL_ADC_AWD_CH_TEMPSENSOR_ADC1_REG_INJ>();
    inline constexpr AdcAnalogWatchdogChannelGroup AdcAnalogWatchdogChannelGroup::kChannelTempsensorAdc5Regular =
        AdcAnalogWatchdogChannelGroup::from_ll<LL_ADC_AWD_CH_TEMPSENSOR_ADC5_REG>();
    inline constexpr AdcAnalogWatchdogChannelGroup AdcAnalogWatchdogChannelGroup::kChannelTempsensorAdc5Injected =
        AdcAnalogWatchdogChannelGroup::from_ll<LL_ADC_AWD_CH_TEMPSENSOR_ADC5_INJ>();
    inline constexpr AdcAnalogWatchdogChannelGroup
        AdcAnalogWatchdogChannelGroup::kChannelTempsensorAdc5RegularAndInjected =
            AdcAnalogWatchdogChannelGroup::from_ll<LL_ADC_AWD_CH_TEMPSENSOR_ADC5_REG_INJ>();
    inline constexpr AdcAnalogWatchdogChannelGroup AdcAnalogWatchdogChannelGroup::kChannelVbatRegular =
        AdcAnalogWatchdogChannelGroup::from_ll<LL_ADC_AWD_CH_VBAT_REG>();
    inline constexpr AdcAnalogWatchdogChannelGroup AdcAnalogWatchdogChannelGroup::kChannelVbatInjected =
        AdcAnalogWatchdogChannelGroup::from_ll<LL_ADC_AWD_CH_VBAT_INJ>();
    inline constexpr AdcAnalogWatchdogChannelGroup AdcAnalogWatchdogChannelGroup::kChannelVbatRegularAndInjected =
        AdcAnalogWatchdogChannelGroup::from_ll<LL_ADC_AWD_CH_VBAT_REG_INJ>();
    inline constexpr AdcAnalogWatchdogChannelGroup AdcAnalogWatchdogChannelGroup::kChannelVopamp1Regular =
        AdcAnalogWatchdogChannelGroup::from_ll<LL_ADC_AWD_CH_VOPAMP1_REG>();
    inline constexpr AdcAnalogWatchdogChannelGroup AdcAnalogWatchdogChannelGroup::kChannelVopamp1Injected =
        AdcAnalogWatchdogChannelGroup::from_ll<LL_ADC_AWD_CH_VOPAMP1_INJ>();
    inline constexpr AdcAnalogWatchdogChannelGroup AdcAnalogWatchdogChannelGroup::kChannelVopamp1RegularAndInjected =
        AdcAnalogWatchdogChannelGroup::from_ll<LL_ADC_AWD_CH_VOPAMP1_REG_INJ>();
    inline constexpr AdcAnalogWatchdogChannelGroup AdcAnalogWatchdogChannelGroup::kChannelVopamp2Regular =
        AdcAnalogWatchdogChannelGroup::from_ll<LL_ADC_AWD_CH_VOPAMP2_REG>();
    inline constexpr AdcAnalogWatchdogChannelGroup AdcAnalogWatchdogChannelGroup::kChannelVopamp2Injected =
        AdcAnalogWatchdogChannelGroup::from_ll<LL_ADC_AWD_CH_VOPAMP2_INJ>();
    inline constexpr AdcAnalogWatchdogChannelGroup AdcAnalogWatchdogChannelGroup::kChannelVopamp2RegularAndInjected =
        AdcAnalogWatchdogChannelGroup::from_ll<LL_ADC_AWD_CH_VOPAMP2_REG_INJ>();
    inline constexpr AdcAnalogWatchdogChannelGroup AdcAnalogWatchdogChannelGroup::kChannelVopamp3Adc2Regular =
        AdcAnalogWatchdogChannelGroup::from_ll<LL_ADC_AWD_CH_VOPAMP3_ADC2_REG>();
    inline constexpr AdcAnalogWatchdogChannelGroup AdcAnalogWatchdogChannelGroup::kChannelVopamp3Adc2Injected =
        AdcAnalogWatchdogChannelGroup::from_ll<LL_ADC_AWD_CH_VOPAMP3_ADC2_INJ>();
    inline constexpr AdcAnalogWatchdogChannelGroup AdcAnalogWatchdogChannelGroup::kChannelVoamp3Adc2RegularAndInjected =
        AdcAnalogWatchdogChannelGroup::from_ll<LL_ADC_AWD_CH_VOPAMP3_ADC2_REG_INJ>();
    inline constexpr AdcAnalogWatchdogChannelGroup AdcAnalogWatchdogChannelGroup::kChannelVoamp3Adc3Regular =
        AdcAnalogWatchdogChannelGroup::from_ll<LL_ADC_AWD_CH_VOPAMP3_ADC3_REG>();
    inline constexpr AdcAnalogWatchdogChannelGroup AdcAnalogWatchdogChannelGroup::kChannelVoamp3Adc3Injected =
        AdcAnalogWatchdogChannelGroup::from_ll<LL_ADC_AWD_CH_VOPAMP3_ADC3_INJ>();
    inline constexpr AdcAnalogWatchdogChannelGroup AdcAnalogWatchdogChannelGroup::kChannelVoamp3Adc3RegularAndInjected =
        AdcAnalogWatchdogChannelGroup::from_ll<LL_ADC_AWD_CH_VOPAMP3_ADC3_REG_INJ>();
    inline constexpr AdcAnalogWatchdogChannelGroup AdcAnalogWatchdogChannelGroup::kChannelVoamp4Regular =
        AdcAnalogWatchdogChannelGroup::from_ll<LL_ADC_AWD_CH_VOPAMP4_REG>();
    inline constexpr AdcAnalogWatchdogChannelGroup AdcAnalogWatchdogChannelGroup::kChannelVoamp4Injected =
        AdcAnalogWatchdogChannelGroup::from_ll<LL_ADC_AWD_CH_VOPAMP4_INJ>();
    inline constexpr AdcAnalogWatchdogChannelGroup AdcAnalogWatchdogChannelGroup::kChannelVoamp4RegularAndInjected =
        AdcAnalogWatchdogChannelGroup::from_ll<LL_ADC_AWD_CH_VOPAMP4_REG_INJ>();
    inline constexpr AdcAnalogWatchdogChannelGroup AdcAnalogWatchdogChannelGroup::kChannelVoamp5Regular =
        AdcAnalogWatchdogChannelGroup::from_ll<LL_ADC_AWD_CH_VOPAMP5_REG>();
    inline constexpr AdcAnalogWatchdogChannelGroup AdcAnalogWatchdogChannelGroup::kChannelVoamp5Injected =
        AdcAnalogWatchdogChannelGroup::from_ll<LL_ADC_AWD_CH_VOPAMP5_INJ>();
    inline constexpr AdcAnalogWatchdogChannelGroup AdcAnalogWatchdogChannelGroup::kChannelVoamp5RegularAndInjected =
        AdcAnalogWatchdogChannelGroup::from_ll<LL_ADC_AWD_CH_VOPAMP5_REG_INJ>();
    inline constexpr AdcAnalogWatchdogChannelGroup AdcAnalogWatchdogChannelGroup::kChannelVoamp6Regular =
        AdcAnalogWatchdogChannelGroup::from_ll<LL_ADC_AWD_CH_VOPAMP6_REG>();
    inline constexpr AdcAnalogWatchdogChannelGroup AdcAnalogWatchdogChannelGroup::kChannelVoamp6Injected =
        AdcAnalogWatchdogChannelGroup::from_ll<LL_ADC_AWD_CH_VOPAMP6_INJ>();
    inline constexpr AdcAnalogWatchdogChannelGroup AdcAnalogWatchdogChannelGroup::kChannelVoamp6RegularAndInjected =
        AdcAnalogWatchdogChannelGroup::from_ll<LL_ADC_AWD_CH_VOPAMP6_REG_INJ>();

    inline constexpr auto kAdcAnalogWatchdogFilteringMapping =
        LLDriverEnumValueSequentialNumberMapping<8>{.start_number = 1,
                                                    .ll_id_table  = {LL_ADC_AWD_FILTERING_NONE,
                                                                     LL_ADC_AWD_FILTERING_2SAMPLES,
                                                                     LL_ADC_AWD_FILTERING_3SAMPLES,
                                                                     LL_ADC_AWD_FILTERING_4SAMPLES,
                                                                     LL_ADC_AWD_FILTERING_5SAMPLES,
                                                                     LL_ADC_AWD_FILTERING_6SAMPLES,
                                                                     LL_ADC_AWD_FILTERING_7SAMPLES,
                                                                     LL_ADC_AWD_FILTERING_8SAMPLES}};

    struct AdcAnalogWatchdogFiltering
        : NumberedLLDriverEnumValue<AdcAnalogWatchdogFiltering, kAdcAnalogWatchdogFilteringMapping>
    {
        static const AdcAnalogWatchdogFiltering kNone;
        static const AdcAnalogWatchdogFiltering k2Samples;
        static const AdcAnalogWatchdogFiltering k3Samples;
        static const AdcAnalogWatchdogFiltering k4Samples;
        static const AdcAnalogWatchdogFiltering k5Samples;
        static const AdcAnalogWatchdogFiltering k6Samples;
        static const AdcAnalogWatchdogFiltering k7Samples;
        static const AdcAnalogWatchdogFiltering k8Samples;
    };

    inline constexpr AdcAnalogWatchdogFiltering AdcAnalogWatchdogFiltering::kNone =
        AdcAnalogWatchdogFiltering::from_number<1>();
    inline constexpr AdcAnalogWatchdogFiltering AdcAnalogWatchdogFiltering::k2Samples =
        AdcAnalogWatchdogFiltering::from_number<2>();
    inline constexpr AdcAnalogWatchdogFiltering AdcAnalogWatchdogFiltering::k3Samples =
        AdcAnalogWatchdogFiltering::from_number<3>();
    inline constexpr AdcAnalogWatchdogFiltering AdcAnalogWatchdogFiltering::k4Samples =
        AdcAnalogWatchdogFiltering::from_number<4>();
    inline constexpr AdcAnalogWatchdogFiltering AdcAnalogWatchdogFiltering::k5Samples =
        AdcAnalogWatchdogFiltering::from_number<5>();
    inline constexpr AdcAnalogWatchdogFiltering AdcAnalogWatchdogFiltering::k6Samples =
        AdcAnalogWatchdogFiltering::from_number<6>();
    inline constexpr AdcAnalogWatchdogFiltering AdcAnalogWatchdogFiltering::k7Samples =
        AdcAnalogWatchdogFiltering::from_number<7>();
    inline constexpr AdcAnalogWatchdogFiltering AdcAnalogWatchdogFiltering::k8Samples =
        AdcAnalogWatchdogFiltering::from_number<8>();

    struct AdcControllerInterruptSource : public EnumValue<AdcControllerInterruptSource, uint8_t>
    {
        static const AdcControllerInterruptSource kReady;
        static const AdcControllerInterruptSource kRegularEndOfConversion;
        static const AdcControllerInterruptSource kRegularEndOfSequence;
        static const AdcControllerInterruptSource kRegularEndOfSampling;
        static const AdcControllerInterruptSource kInjectEndOfConversion;
        static const AdcControllerInterruptSource kInjectEndOfSequence;
        static const AdcControllerInterruptSource kInjectContextQueueOverflow;
        static const AdcControllerInterruptSource kOverrun;
        static const AdcControllerInterruptSource kAnalogWatchdog1;
        static const AdcControllerInterruptSource kAnalogWatchdog2;
        static const AdcControllerInterruptSource kAnalogWatchdog3;

        [[nodiscard]] static constexpr std::array<AdcControllerInterruptSource, 11> values()
        {
            return {kReady,
                    kRegularEndOfConversion,
                    kRegularEndOfSequence,
                    kRegularEndOfSampling,
                    kInjectEndOfConversion,
                    kInjectEndOfSequence,
                    kInjectContextQueueOverflow,
                    kOverrun,
                    kAnalogWatchdog1,
                    kAnalogWatchdog2,
                    kAnalogWatchdog3};
        }
    };

    inline constexpr AdcControllerInterruptSource AdcControllerInterruptSource::kReady =
        AdcControllerInterruptSource::from_number<0>();
    inline constexpr AdcControllerInterruptSource AdcControllerInterruptSource::kRegularEndOfConversion =
        AdcControllerInterruptSource::from_number<1>();
    inline constexpr AdcControllerInterruptSource AdcControllerInterruptSource::kRegularEndOfSequence =
        AdcControllerInterruptSource::from_number<2>();
    inline constexpr AdcControllerInterruptSource AdcControllerInterruptSource::kRegularEndOfSampling =
        AdcControllerInterruptSource::from_number<3>();
    inline constexpr AdcControllerInterruptSource AdcControllerInterruptSource::kInjectEndOfConversion =
        AdcControllerInterruptSource::from_number<4>();
    inline constexpr AdcControllerInterruptSource AdcControllerInterruptSource::kInjectEndOfSequence =
        AdcControllerInterruptSource::from_number<5>();
    inline constexpr AdcControllerInterruptSource AdcControllerInterruptSource::kInjectContextQueueOverflow =
        AdcControllerInterruptSource::from_number<6>();
    inline constexpr AdcControllerInterruptSource AdcControllerInterruptSource::kOverrun =
        AdcControllerInterruptSource::from_number<7>();
    inline constexpr AdcControllerInterruptSource AdcControllerInterruptSource::kAnalogWatchdog1 =
        AdcControllerInterruptSource::from_number<8>();
    inline constexpr AdcControllerInterruptSource AdcControllerInterruptSource::kAnalogWatchdog2 =
        AdcControllerInterruptSource::from_number<9>();
    inline constexpr AdcControllerInterruptSource AdcControllerInterruptSource::kAnalogWatchdog3 =
        AdcControllerInterruptSource::from_number<10>();

    // ============================================================================
    // HARDWARE TRAITS
    // ============================================================================
    template <AdcControllerSpec tkControllerSpec>
    struct AdcControllerTraits;

    template <>
    struct AdcControllerTraits<kAdc1ControllerSpec>
    {
        static constexpr AdcControllerSpec       skControllerSpec = kAdc1ControllerSpec;
        static constexpr AdcControllerId         skControllerId   = skControllerSpec.controller_id;
        static inline ADC_TypeDef *const         skInstance       = ADC1;
        static constexpr IRQn_Type               skIRQn           = ADC1_2_IRQn;
        static constexpr DmaMuxRequestId         skDmaMuxRequest  = DmaMuxRequestId::kAdc1;
        static constexpr AdcCommonControllerSpec skCommonSpec     = kAdc12CommonControllerSpec;
    };
    template <>
    struct AdcControllerTraits<kAdc2ControllerSpec>
    {
        static constexpr AdcControllerSpec       skControllerSpec = kAdc2ControllerSpec;
        static constexpr AdcControllerId         skControllerId   = skControllerSpec.controller_id;
        static inline ADC_TypeDef *const         skInstance       = ADC2;
        static constexpr IRQn_Type               skIRQn           = ADC1_2_IRQn;
        static constexpr DmaMuxRequestId         skDmaMuxRequest  = DmaMuxRequestId::kAdc2;
        static constexpr AdcCommonControllerSpec skCommonSpec     = kAdc12CommonControllerSpec;
    };

    template <>
    struct AdcControllerTraits<kAdc3ControllerSpec>
    {
        static constexpr AdcControllerSpec       skControllerSpec = kAdc3ControllerSpec;
        static constexpr AdcControllerId         skControllerId   = skControllerSpec.controller_id;
        static inline ADC_TypeDef *const         skInstance       = ADC3;
        static constexpr IRQn_Type               skIRQn           = ADC3_IRQn;
        static constexpr DmaMuxRequestId         skDmaMuxRequest  = DmaMuxRequestId::kAdc3;
        static constexpr AdcCommonControllerSpec skCommonSpec     = kAdc345CommonControllerSpec;
    };

    template <>
    struct AdcControllerTraits<kAdc4ControllerSpec>
    {
        static constexpr AdcControllerSpec       skControllerSpec = kAdc4ControllerSpec;
        static constexpr AdcControllerId         skControllerId   = skControllerSpec.controller_id;
        static inline ADC_TypeDef *const         skInstance       = ADC4;
        static constexpr IRQn_Type               skIRQn           = ADC4_IRQn;
        static constexpr DmaMuxRequestId         skDmaMuxRequest  = DmaMuxRequestId::kAdc4;
        static constexpr AdcCommonControllerSpec skCommonSpec     = kAdc345CommonControllerSpec;
    };

    template <>
    struct AdcControllerTraits<kAdc5ControllerSpec>
    {
        static constexpr AdcControllerSpec       skControllerSpec = kAdc5ControllerSpec;
        static constexpr AdcControllerId         skControllerId   = skControllerSpec.controller_id;
        static inline ADC_TypeDef *const         skInstance       = ADC5;
        static constexpr IRQn_Type               skIRQn           = ADC5_IRQn;
        static constexpr DmaMuxRequestId         skDmaMuxRequest  = DmaMuxRequestId::kAdc5;
        static constexpr AdcCommonControllerSpec skCommonSpec     = kAdc345CommonControllerSpec;
    };

    // ============================================================================
    // INTERFACE
    // ============================================================================

    struct AdcTraits
    {
        using AdcValue                            = uint16_t;
        static constexpr size_t skNumChannels     = 19;
        static constexpr size_t skNumInjChannels  = 4;
        static constexpr size_t skNumRegChannels  = 16;
        static constexpr size_t skNumOffsetBlocks = 4;

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

        static constexpr DurationSecondsF calculate_channel_sample_time(
            const uint32_t                            clock_freq_hz,
            const AdcChannelSampleTime                sample_time,
            const std::optional<AdcOversamplingRatio> oversampling_ratio)
        {
            const uint32_t oversampling_factor =
                oversampling_ratio.has_value() ? oversampling_ratio.value().to_number() : 1;
            const float duration_s =
                (sample_time.to_cycles() * static_cast<float>(oversampling_factor)) / static_cast<float>(clock_freq_hz);
            return DurationSecondsF(duration_s);
        }
    };

    template <AdcControllerSpec tkControllerSpec>
    struct AdcControllerInterface : public AdcTraits
    {
        using ControllerTraitsT = AdcControllerTraits<tkControllerSpec>;

        // ----------------------------------------------------------------------------
        // CORE
        // ----------------------------------------------------------------------------
        VALLE_LL_WRAPPER void enable() const
        {
            LL_ADC_Enable(ControllerTraitsT::skInstance);
        }

        [[nodiscard]] VALLE_LL_WRAPPER bool is_enabled() const
        {
            return LL_ADC_IsEnabled(ControllerTraitsT::skInstance) == 1UL;
        }

        VALLE_LL_WRAPPER void disable() const
        {
            LL_ADC_Disable(ControllerTraitsT::skInstance);
        }

        [[nodiscard]] VALLE_LL_WRAPPER bool is_disable_ongoing() const
        {
            return LL_ADC_IsDisableOngoing(ControllerTraitsT::skInstance) != 0;
        }

        VALLE_LL_WRAPPER void enable_deep_power_down() const
        {
            LL_ADC_EnableDeepPowerDown(ControllerTraitsT::skInstance);
        }

        VALLE_LL_WRAPPER void disable_deep_power_down() const
        {
            LL_ADC_DisableDeepPowerDown(ControllerTraitsT::skInstance);
        }

        [[nodiscard]] VALLE_LL_WRAPPER bool deep_power_down_enabled() const
        {
            return LL_ADC_IsDeepPowerDownEnabled(ControllerTraitsT::skInstance) == 1UL;
        }

        VALLE_LL_WRAPPER void enable_internal_regulator() const
        {
            LL_ADC_EnableInternalRegulator(ControllerTraitsT::skInstance);
        }

        VALLE_LL_WRAPPER void disable_internal_regulator() const
        {
            LL_ADC_DisableInternalRegulator(ControllerTraitsT::skInstance);
        }

        [[nodiscard]] VALLE_LL_WRAPPER bool internal_regulator_enabled() const
        {
            return LL_ADC_IsInternalRegulatorEnabled(ControllerTraitsT::skInstance) == 1UL;
        }

        VALLE_LL_WRAPPER void start_calibration() const
        {
            LL_ADC_StartCalibration(ControllerTraitsT::skInstance, LL_ADC_SINGLE_ENDED);
        }

        [[nodiscard]] VALLE_LL_WRAPPER bool is_calibration_ongoing() const
        {
            return LL_ADC_IsCalibrationOnGoing(ControllerTraitsT::skInstance) != 0;
        }

        [[nodiscard]] VALLE_LL_WRAPPER uint32_t get_dma_reg_addr(const AdcDmaRegister dma_register) const
        {
            return LL_ADC_DMA_GetRegAddr(ControllerTraitsT::skInstance, dma_register.to_ll());
        }
        // -----------------------------------------------------------------------------
        // CONFIGURATION
        // -----------------------------------------------------------------------------
        VALLE_LL_WRAPPER void set_resolution(const AdcResolution resolution) const
        {
            LL_ADC_SetResolution(ControllerTraitsT::skInstance, resolution.to_ll());
        }

        [[nodiscard]] VALLE_LL_WRAPPER AdcResolution get_resolution() const
        {
            return AdcResolution::from_ll(LL_ADC_GetResolution(ControllerTraitsT::skInstance));
        }

        VALLE_LL_WRAPPER void set_data_alignment(const AdcDataAlignment data_alignment) const
        {
            LL_ADC_SetDataAlignment(ControllerTraitsT::skInstance, data_alignment.to_ll());
        }

        [[nodiscard]] VALLE_LL_WRAPPER AdcDataAlignment get_data_alignment() const
        {
            return AdcDataAlignment::from_ll(LL_ADC_GetDataAlignment(ControllerTraitsT::skInstance));
        }

        VALLE_LL_WRAPPER void set_low_power_mode(const AdcLowPowerMode low_power_mode) const
        {
            LL_ADC_SetLowPowerMode(ControllerTraitsT::skInstance, low_power_mode.to_ll());
        }

        [[nodiscard]] VALLE_LL_WRAPPER AdcLowPowerMode get_low_power_mode() const
        {
            return AdcLowPowerMode::from_ll(LL_ADC_GetLowPowerMode(ControllerTraitsT::skInstance));
        }

        VALLE_LL_WRAPPER void set_calibration_factor(const AdcDifferentialMode single_diff,
                                                     const uint32_t            calibration_factor) const
        {
            LL_ADC_SetCalibrationFactor(ControllerTraitsT::skInstance, single_diff.to_ll(), calibration_factor);
        }

        [[nodiscard]] VALLE_LL_WRAPPER uint32_t get_calibration_factor(const AdcDifferentialMode single_diff) const
        {
            return LL_ADC_GetCalibrationFactor(ControllerTraitsT::skInstance, single_diff.to_ll());
        }

        VALLE_LL_WRAPPER void set_gain_compensation(const uint16_t gain_compensation) const
        {
            LL_ADC_SetGainCompensation(ControllerTraitsT::skInstance, gain_compensation.to_ll());
        }

        [[nodiscard]] VALLE_LL_WRAPPER uint32_t get_gain_compensation() const
        {
            return LL_ADC_GetGainCompensation(ControllerTraitsT::skInstance);
        }

        VALLE_LL_WRAPPER void set_common_sample_time(const AdcCommonSamplingTime sampling_time) const
        {
            LL_ADC_SetSamplingTimeCommonConfig(ControllerTraitsT::skInstance, sampling_time.to_ll());
        }

        [[nodiscard]] VALLE_LL_WRAPPER AdcCommonSamplingTime get_sampling_time_common_config() const
        {
            return AdcCommonSamplingTime::from_ll(LL_ADC_GetSamplingTimeCommonConfig(ControllerTraitsT::skInstance));
        }

        VALLE_LL_WRAPPER void set_oversampling_scope(const AdcOversamplingScope scope) const
        {
            LL_ADC_SetOverSamplingScope(ControllerTraitsT::skInstance, scope.to_ll());
        }

        [[nodiscard]] VALLE_LL_WRAPPER AdcOversamplingScope get_oversampling_scope() const
        {
            return AdcOversamplingScope::from_ll(LL_ADC_GetOverSamplingScope(ControllerTraitsT::skInstance));
        }

        [[nodiscard]] VALLE_LL_WRAPPER bool inject_group_oversampling_enabled() const
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

        [[nodiscard]] VALLE_LL_WRAPPER bool regular_group_oversampling_enabled() const
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

        VALLE_LL_WRAPPER void set_oversampling_ratio_shift(const AdcOversamplingRatio ratio,
                                                           const AdcOversamplingShift shift) const
        {
            LL_ADC_ConfigOverSamplingRatioShift(ControllerTraitsT::skInstance, ratio.to_ll(), shift.to_ll());
        }

        [[nodiscard]] VALLE_LL_WRAPPER AdcOversamplingRatio get_oversampling_ratio() const
        {
            return AdcOversamplingRatio::from_ll(LL_ADC_GetOverSamplingRatio(ControllerTraitsT::skInstance));
        }

        [[nodiscard]] VALLE_LL_WRAPPER AdcOversamplingShift get_oversampling_shift() const
        {
            return AdcOversamplingShift::from_ll(LL_ADC_GetOverSamplingShift(ControllerTraitsT::skInstance));
        }

        // ----------------------------------------------------------------------------
        // CONVERSION CONTROL
        // ----------------------------------------------------------------------------
        /**
         * @brief ARM THE TRIGGER (The most important part)
         * If Trigger is Hardware (HRTIM): ADC goes into "Waiting for Trigger" state.
         * If Trigger is Software: ADC converts immediately.
         */
        VALLE_LL_WRAPPER void start_inject() const
        {
            LL_ADC_INJ_StartConversion(ControllerTraitsT::skInstance);
        }

        VALLE_LL_WRAPPER void stop_inject() const
        {
            LL_ADC_INJ_StopConversion(ControllerTraitsT::skInstance);
        }

        /**
         * @brief ARM THE TRIGGER (The most important part)
         * If Trigger is Hardware (HRTIM): ADC goes into "Waiting for Trigger" state.
         * If Trigger is Software: ADC converts immediately.
         */
        VALLE_LL_WRAPPER void start_regular() const
        {
            LL_ADC_REG_StartConversion(ControllerTraitsT::skInstance);
        }

        VALLE_LL_WRAPPER void stop_regular() const
        {
            LL_ADC_REG_StopConversion(ControllerTraitsT::skInstance);
        }

        // ----------------------------------------------------------------------------
        // RESOLUTION INFO
        // ----------------------------------------------------------------------------

        [[nodiscard]] VALLE_LL_WRAPPER uint32_t get_resolution_range() const
        {
            const uint8_t res_bits = get_resolution().to_number();
            return (1UL << res_bits) - 1UL;
        }

        [[nodiscard]] VALLE_LL_WRAPPER uint32_t get_effective_resolution_range() const
        {
            // Max value of a single ADC conversion
            const uint32_t max_single_sample = get_resolution_range();

            // Max value when 'ovs_ratio' samples are accumulated
            const uint32_t max_accumulated = max_single_sample * get_oversampling_ratio().to_number();

            // Hardware shifts the accumulated sum right
            const uint32_t effective_max = max_accumulated >> get_oversampling_shift().to_number();

            // The ADC Data Register is 16 bits.
            // If the math results in > 65535, the hardware truncates the top bits!
            return std::min(effective_max, static_cast<uint32_t>(std::numeric_limits<uint16_t>::max()));
        }

        [[nodiscard]] VALLE_LL_WRAPPER uint32_t get_inject_group_effective_resolution_range() const
        {
            if (inject_group_oversampling_enabled())
            {
                return get_effective_resolution_range();
            }

            return get_resolution_range();
        }

        [[nodiscard]] VALLE_LL_WRAPPER uint32_t get_regular_group_effective_resolution_range() const
        {
            if (regular_group_oversampling_enabled())
            {
                return get_effective_resolution_range();
            }

            return get_resolution_range();
        }

        [[nodiscard]] VALLE_LL_WRAPPER AdcInjectGroupTriggerSource inject_group_get_trigger_source() const
        {
            return AdcInjectGroupTriggerSource::from_ll(LL_ADC_INJ_GetTriggerSource(ControllerTraitsT::skInstance));
        }

        [[nodiscard]] VALLE_LL_WRAPPER bool inject_group_is_trigger_source_sw_start() const
        {
            return LL_ADC_INJ_IsTriggerSourceSWStart(ControllerTraitsT::skInstance) != 0;
        }

        [[nodiscard]] VALLE_LL_WRAPPER AdcInjectGroupTriggerEdge inject_group_get_trigger_edge() const
        {
            return AdcInjectGroupTriggerEdge::from_ll(LL_ADC_INJ_GetTriggerEdge(ControllerTraitsT::skInstance));
        }

        [[nodiscard]] VALLE_LL_WRAPPER uint32_t inject_group_get_sequencer_discontinuity_mode() const
        {
            return LL_ADC_INJ_GetSequencerDiscont(ControllerTraitsT::skInstance);
        }

        VALLE_LL_WRAPPER void inject_group_config_queue_context(const uint32_t channel,
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
        VALLE_LL_WRAPPER void inject_group_set_queue_mode(const AdcInjectGroupQueueMode queue_mode) const
        {
            LL_ADC_INJ_SetQueueMode(ControllerTraitsT::skInstance, queue_mode.to_ll());
        }

        [[nodiscard]] VALLE_LL_WRAPPER AdcInjectGroupQueueMode inject_group_get_queue_mode() const
        {
            return AdcInjectGroupQueueMode::from_ll(LL_ADC_INJ_GetQueueMode(ControllerTraitsT::skInstance));
        }

        VALLE_LL_WRAPPER void inject_group_set_trigger_mode(const AdcInjectGroupTriggerMode trigger_mode) const
        {
            LL_ADC_INJ_SetTrigAuto(ControllerTraitsT::skInstance, trigger_mode.to_ll());
        }

        [[nodiscard]] VALLE_LL_WRAPPER AdcInjectGroupTriggerMode inject_group_get_trigger_mode() const
        {
            return AdcInjectGroupTriggerMode::from_ll(LL_ADC_INJ_GetTrigAuto(ControllerTraitsT::skInstance));
        }

        VALLE_LL_WRAPPER void inject_group_set_sequencer_discontinuity_mode(
            const AdcInjectGroupSequencerDiscontinuityMode discontinuity_mode) const
        {
            LL_ADC_INJ_SetSequencerDiscont(ControllerTraitsT::skInstance, discontinuity_mode.to_ll());
        }

        VALLE_LL_WRAPPER void inject_group_set_trigger_source(const AdcInjectGroupTriggerSource trigger_source) const
        {
            LL_ADC_INJ_SetTriggerSource(ControllerTraitsT::skInstance, trigger_source.to_ll());
        }

        VALLE_LL_WRAPPER void inject_group_set_trigger_edge(const AdcInjectGroupTriggerEdge trigger_edge) const
        {
            LL_ADC_INJ_SetTriggerEdge(ControllerTraitsT::skInstance, trigger_edge.to_ll());
        }

        static VALLE_LL_WRAPPER void inject_group_config_trigger(const AdcInjectGroupTriggerSource trigger_source,
                                                                 const AdcInjectGroupTriggerEdge   trigger_edge)
        {
            set_trigger_source(trigger_source);
            if (trigger_source != AdcInjectGroupTriggerSource::kSoftware)
            {
                set_trigger_edge(trigger_edge);
            }
        }

        // Sequence Config
        VALLE_LL_WRAPPER void inject_group_set_sequencer_length(const uint32_t inj_count) const
        {
            LL_ADC_INJ_SetSequencerLength(ControllerTraitsT::skInstance,
                                          AdcInjectGroupSequenceScanMode::from_number(inj_count).to_ll());
        }

        [[nodiscard]] VALLE_LL_WRAPPER uint32_t inject_group_get_sequencer_length() const
        {
            return AdcInjectGroupSequenceScanMode::from_ll(LL_ADC_INJ_GetSequencerLength(ControllerTraitsT::skInstance))
                .to_number();
        }

        template <AdcChannelId tkChannelId>
        VALLE_LL_WRAPPER void inject_group_set_sequencer_ranks(const AdcInjectChannelRank rank) const
        {
            LL_ADC_INJ_SetSequencerRanks(ControllerTraitsT::skInstance, rank.to_ll(), tkChannelId.to_ll());
        }

        [[nodiscard]] VALLE_LL_WRAPPER AdcChannelId
        inject_group_get_sequencer_ranks(const AdcInjectChannelRank rank) const
        {
            return AdcChannelId::from_ll(LL_ADC_INJ_GetSequencerRanks(ControllerTraitsT::skInstance, rank.to_ll()));
        }

        [[nodiscard]] VALLE_LL_WRAPPER uint32_t inject_group_read(const AdcInjectChannelRank rank) const
        {
            return LL_ADC_INJ_ReadConversionData32(ControllerTraitsT::skInstance, rank.to_ll());
        }

        [[nodiscard]] VALLE_LL_WRAPPER bool inject_group_is_conversion_ongoing() const
        {
            return LL_ADC_INJ_IsConversionOngoing(ControllerTraitsT::skInstance) != 0;
        }

        [[nodiscard]] VALLE_LL_WRAPPER bool inject_group_is_stop_conversion_ongoing() const
        {
            return LL_ADC_INJ_IsStopConversionOngoing(ControllerTraitsT::skInstance) != 0;
        }

        // -----------------------------------------------------------------------------
        // Regular Group Control
        // -----------------------------------------------------------------------------
        VALLE_LL_WRAPPER void regular_group_set_oversampling_mode(
            const AdcRegularGroupOversamplingMode oversampling_mode) const
        {
            LL_ADC_SetOverSamplingDiscont(ControllerTraitsT::skInstance, oversampling_mode.to_ll());
        }

        [[nodiscard]] VALLE_LL_WRAPPER AdcRegularGroupOversamplingMode get_oversampling_discont() const
        {
            return AdcRegularGroupOversamplingMode::from_ll(
                LL_ADC_GetOverSamplingDiscont(ControllerTraitsT::skInstance));
        }

        VALLE_LL_WRAPPER void regular_group_set_trigger_source(const AdcRegularGroupTriggerSource trigger_source) const
        {
            LL_ADC_REG_SetTriggerSource(ControllerTraitsT::skInstance, trigger_source.to_ll());
        }

        [[nodiscard]] VALLE_LL_WRAPPER AdcRegularGroupTriggerSource regular_group_get_trigger_source() const
        {
            return AdcRegularGroupTriggerSource::from_ll(LL_ADC_REG_GetTriggerSource(ControllerTraitsT::skInstance));
        }

        VALLE_LL_WRAPPER void regular_group_set_trigger_edge(const AdcRegularGroupTriggerEdge trigger_edge) const
        {
            LL_ADC_REG_SetTriggerEdge(ControllerTraitsT::skInstance, trigger_edge.to_ll());
        }

        [[nodiscard]] VALLE_LL_WRAPPER AdcRegularGroupTriggerEdge regular_group_get_trigger_edge() const
        {
            return AdcRegularGroupTriggerEdge::from_ll(LL_ADC_REG_GetTriggerEdge(ControllerTraitsT::skInstance));
        }

        VALLE_LL_WRAPPER void regular_group_config_trigger(const AdcRegularGroupTriggerSource trigger_source,
                                                           const AdcRegularGroupTriggerEdge   trigger_edge) const
        {
            regular_group_set_trigger_source(trigger_source);
            if (trigger_source != AdcRegularGroupTriggerSource::kSoftware)
            {
                regular_group_set_trigger_edge(trigger_edge);
            }
        }

        VALLE_LL_WRAPPER void regular_group_set_overrun_behavior(
            const AdcRegularGroupOverrunBehavior overrun_behavior) const
        {
            LL_ADC_REG_SetOverrun(ControllerTraitsT::skInstance, overrun_behavior.to_ll());
        }

        [[nodiscard]] VALLE_LL_WRAPPER AdcRegularGroupOverrunBehavior regular_group_get_overrun_behavior() const
        {
            return AdcRegularGroupOverrunBehavior::from_ll(LL_ADC_REG_GetOverrun(ControllerTraitsT::skInstance));
        }

        VALLE_LL_WRAPPER void regular_group_set_conversion_mode(
            const AdcRegularGroupConversionMode conversion_mode) const
        {
            LL_ADC_REG_SetContinuousMode(ControllerTraitsT::skInstance, conversion_mode.to_ll());
        }

        [[nodiscard]] VALLE_LL_WRAPPER AdcRegularGroupConversionMode regular_group_get_continuous_mode() const
        {
            return AdcRegularGroupConversionMode::from_ll(LL_ADC_REG_GetContinuousMode(ControllerTraitsT::skInstance));
        }

        VALLE_LL_WRAPPER void regular_group_set_sequencer_discontinuity_mode(
            const AdcRegularGroupSequencerDiscontinuity seq_discont) const
        {
            LL_ADC_REG_SetSequencerDiscont(ControllerTraitsT::skInstance, seq_discont.to_ll());
        }

        [[nodiscard]] VALLE_LL_WRAPPER AdcRegularGroupSequencerDiscontinuity
        regular_group_get_sequencer_discontinuity_mode() const
        {
            return AdcRegularGroupSequencerDiscontinuity::from_ll(
                LL_ADC_REG_GetSequencerDiscont(ControllerTraitsT::skInstance));
        }

        VALLE_LL_WRAPPER void regular_group_set_sampling_mode(const AdcRegularGroupSamplingMode sampling_mode) const
        {
            LL_ADC_REG_SetSamplingMode(ControllerTraitsT::skInstance, sampling_mode.to_ll());
        }

        [[nodiscard]] VALLE_LL_WRAPPER AdcRegularGroupSamplingMode regular_group_get_sampling_mode() const
        {
            return AdcRegularGroupSamplingMode::from_ll(LL_ADC_REG_GetSamplingMode(ControllerTraitsT::skInstance));
        }

        [[nodiscard]] VALLE_LL_WRAPPER bool regular_group_is_trigger_source_sw_start() const
        {
            return LL_ADC_REG_IsTriggerSourceSWStart(ControllerTraitsT::skInstance) != 0;
        }

        // Sequence Config
        VALLE_LL_WRAPPER void regular_group_set_sequencer_length(const uint32_t reg_count) const
        {
            LL_ADC_REG_SetSequencerLength(ControllerTraitsT::skInstance,
                                          AdcRegularGroupCountToSequenceScanMode::from_number(reg_count).to_ll());
        }

        [[nodiscard]] VALLE_LL_WRAPPER uint32_t regular_group_get_sequencer_length() const
        {
            return AdcRegularGroupCountToSequenceScanMode::from_ll(
                       LL_ADC_REG_GetSequencerLength(ControllerTraitsT::skInstance))
                .to_number();
        }

        template <AdcChannelId tkChannelId>
        VALLE_LL_WRAPPER void regular_group_set_sequencer_ranks(const AdcRegularChannelRank rank) const
        {
            LL_ADC_REG_SetSequencerRanks(ControllerTraitsT::skInstance, rank.to_ll(), tkChannelId.to_ll());
        }

        [[nodiscard]] VALLE_LL_WRAPPER AdcChannelId
        regular_group_get_sequencer_ranks(const AdcRegularChannelRank rank) const
        {
            return AdcChannelId::from_ll(LL_ADC_REG_GetSequencerRanks(ControllerTraitsT::skInstance, rank.to_ll()));
        }

        VALLE_LL_WRAPPER void regular_group_set_dma_transfer(const AdcRegularGroupDmaTransfer dma_transfer) const
        {
            LL_ADC_REG_SetDmaTransfer(ControllerTraitsT::skInstance, dma_transfer.to_ll());
        }

        [[nodiscard]] VALLE_LL_WRAPPER AdcRegularGroupDmaTransfer regular_group_get_dma_transfer() const
        {
            return AdcRegularGroupDmaTransfer::from_ll(LL_ADC_REG_GetDMATransfer(ControllerTraitsT::skInstance));
        }

        [[nodiscard]] VALLE_LL_WRAPPER uint32_t regular_group_read(const AdcRegularChannelRank rank) const
        {
            return LL_ADC_REG_ReadConversionData32(ControllerTraitsT::skInstance, rank.to_ll());
        }

        [[nodiscard]] VALLE_LL_WRAPPER bool regular_group_is_conversion_ongoing() const
        {
            return LL_ADC_REG_IsConversionOngoing(ControllerTraitsT::skInstance) != 0;
        }

        [[nodiscard]] VALLE_LL_WRAPPER bool regular_group_is_stop_conversion_ongoing() const
        {
            return LL_ADC_REG_IsStopConversionOngoing(ControllerTraitsT::skInstance) != 0;
        }

        VALLE_LL_WRAPPER void regular_group_start_sampling_phase() const
        {
            LL_ADC_REG_StartSamplingPhase(ControllerTraitsT::skInstance);
        }

        VALLE_LL_WRAPPER void regular_group_stop_sampling_phase() const
        {
            LL_ADC_REG_StopSamplingPhase(ControllerTraitsT::skInstance);
        }

        // -----------------------------------------------------------------------------
        // Interrupts and Flags
        // -----------------------------------------------------------------------------
        [[nodiscard]] VALLE_LL_WRAPPER bool is_ready_interrupt_flag_active() const
        {
            return LL_ADC_IsActiveFlag_ADRDY(ControllerTraitsT::skInstance) != 0;
        }

        VALLE_LL_WRAPPER void clear_ready_interrupt_flag() const
        {
            LL_ADC_ClearFlag_ADRDY(ControllerTraitsT::skInstance);
        }

        VALLE_LL_WRAPPER void enable_ready_interrupt() const
        {
            LL_ADC_EnableIT_ADRDY(ControllerTraitsT::skInstance);
        }

        VALLE_LL_WRAPPER void disable_ready_interrupt() const
        {
            LL_ADC_DisableIT_ADRDY(ControllerTraitsT::skInstance);
        }

        [[nodiscard]] VALLE_LL_WRAPPER bool is_ready_interrupt_enabled() const
        {
            return LL_ADC_IsEnabledIT_ADRDY(ControllerTraitsT::skInstance) != 0;
        }

        [[nodiscard]] VALLE_LL_WRAPPER bool is_regular_group_end_of_conversion_interrupt_flag_active() const
        {
            return LL_ADC_IsActiveFlag_EOC(ControllerTraitsT::skInstance) != 0;
        }

        VALLE_LL_WRAPPER void clear_regular_group_end_of_conversion_interrupt_flag() const
        {
            LL_ADC_ClearFlag_EOC(ControllerTraitsT::skInstance);
        }

        VALLE_LL_WRAPPER void enable_regular_group_end_of_conversion_interrupt() const
        {
            LL_ADC_EnableIT_EOC(ControllerTraitsT::skInstance);
        }

        VALLE_LL_WRAPPER void disable_regular_group_end_of_conversion_interrupt() const
        {
            LL_ADC_DisableIT_EOC(ControllerTraitsT::skInstance);
        }

        [[nodiscard]] VALLE_LL_WRAPPER bool is_regular_group_end_of_conversion_interrupt_enabled() const
        {
            return LL_ADC_IsEnabledIT_EOC(ControllerTraitsT::skInstance) != 0;
        }

        [[nodiscard]] VALLE_LL_WRAPPER bool is_regular_group_end_of_sequence_interrupt_flag_active() const
        {
            return LL_ADC_IsActiveFlag_EOS(ControllerTraitsT::skInstance) != 0;
        }

        VALLE_LL_WRAPPER void clear_regular_group_end_of_sequence_interrupt_flag() const
        {
            LL_ADC_ClearFlag_EOS(ControllerTraitsT::skInstance);
        }

        VALLE_LL_WRAPPER void enable_regular_group_end_of_sequence_interrupt() const
        {
            LL_ADC_EnableIT_EOS(ControllerTraitsT::skInstance);
        }

        VALLE_LL_WRAPPER void disable_regular_group_end_of_sequence_interrupt() const
        {
            LL_ADC_DisableIT_EOS(ControllerTraitsT::skInstance);
        }

        [[nodiscard]] VALLE_LL_WRAPPER bool is_regular_group_end_of_sequence_interrupt_enabled() const
        {
            return LL_ADC_IsEnabledIT_EOS(ControllerTraitsT::skInstance) != 0;
        }

        [[nodiscard]] VALLE_LL_WRAPPER bool is_overrun_interrupt_flag_active() const
        {
            return LL_ADC_IsActiveFlag_OVR(ControllerTraitsT::skInstance) != 0;
        }

        VALLE_LL_WRAPPER void clear_overrun_interrupt_flag() const
        {
            LL_ADC_ClearFlag_OVR(ControllerTraitsT::skInstance);
        }

        VALLE_LL_WRAPPER void enable_overrun_interrupt() const
        {
            LL_ADC_EnableIT_OVR(ControllerTraitsT::skInstance);
        }

        VALLE_LL_WRAPPER void disable_overrun_interrupt() const
        {
            LL_ADC_DisableIT_OVR(ControllerTraitsT::skInstance);
        }

        [[nodiscard]] VALLE_LL_WRAPPER bool is_overrun_interrupt_enabled() const
        {
            return LL_ADC_IsEnabledIT_OVR(ControllerTraitsT::skInstance) != 0;
        }

        [[nodiscard]] VALLE_LL_WRAPPER bool is_regular_group_end_of_sampling_interrupt_flag_active() const
        {
            return LL_ADC_IsActiveFlag_EOSMP(ControllerTraitsT::skInstance) != 0;
        }

        VALLE_LL_WRAPPER void clear_regular_group_end_of_sampling_interrupt_flag() const
        {
            LL_ADC_ClearFlag_EOSMP(ControllerTraitsT::skInstance);
        }

        VALLE_LL_WRAPPER void enable_regular_group_end_of_sampling_interrupt() const
        {
            LL_ADC_EnableIT_EOSMP(ControllerTraitsT::skInstance);
        }

        VALLE_LL_WRAPPER void disable_regular_group_end_of_sampling_interrupt() const
        {
            LL_ADC_DisableIT_EOSMP(ControllerTraitsT::skInstance);
        }

        [[nodiscard]] VALLE_LL_WRAPPER bool is_regular_group_end_of_sampling_interrupt_enabled() const
        {
            return LL_ADC_IsEnabledIT_EOSMP(ControllerTraitsT::skInstance) != 0;
        }

        [[nodiscard]] VALLE_LL_WRAPPER bool is_inject_group_end_of_conversion_interrupt_flag_active() const
        {
            return LL_ADC_IsActiveFlag_JEOC(ControllerTraitsT::skInstance) != 0;
        }

        VALLE_LL_WRAPPER void clear_inject_group_end_of_conversion_interrupt_flag() const
        {
            LL_ADC_ClearFlag_JEOC(ControllerTraitsT::skInstance);
        }

        VALLE_LL_WRAPPER void enable_inject_group_end_of_conversion_interrupt() const
        {
            LL_ADC_EnableIT_JEOC(ControllerTraitsT::skInstance);
        }

        VALLE_LL_WRAPPER void disable_inject_group_end_of_conversion_interrupt() const
        {
            LL_ADC_DisableIT_JEOC(ControllerTraitsT::skInstance);
        }

        [[nodiscard]] VALLE_LL_WRAPPER bool is_inject_group_end_of_conversion_interrupt_enabled() const
        {
            return LL_ADC_IsEnabledIT_JEOC(ControllerTraitsT::skInstance) != 0;
        }

        [[nodiscard]] VALLE_LL_WRAPPER bool is_inject_group_end_of_sequence_interrupt_flag_active() const
        {
            return LL_ADC_IsActiveFlag_JEOS(ControllerTraitsT::skInstance) != 0;
        }

        VALLE_LL_WRAPPER void clear_inject_group_end_of_sequence_interrupt_flag() const
        {
            LL_ADC_ClearFlag_JEOS(ControllerTraitsT::skInstance);
        }

        VALLE_LL_WRAPPER void enable_inject_group_end_of_sequence_interrupt() const
        {
            LL_ADC_EnableIT_JEOS(ControllerTraitsT::skInstance);
        }

        VALLE_LL_WRAPPER void disable_inject_group_end_of_sequence_interrupt() const
        {
            LL_ADC_DisableIT_JEOS(ControllerTraitsT::skInstance);
        }

        [[nodiscard]] VALLE_LL_WRAPPER bool is_inject_group_end_of_sequence_interrupt_enabled() const
        {
            return LL_ADC_IsEnabledIT_JEOS(ControllerTraitsT::skInstance) != 0;
        }

        [[nodiscard]] VALLE_LL_WRAPPER bool is_inject_group_queue_overflow_interrupt_flag_active() const
        {
            return LL_ADC_IsActiveFlag_JQOVF(ControllerTraitsT::skInstance) != 0;
        }

        VALLE_LL_WRAPPER void clear_inject_group_queue_overflow_interrupt_flag() const
        {
            LL_ADC_ClearFlag_JQOVF(ControllerTraitsT::skInstance);
        }

        VALLE_LL_WRAPPER void enable_inject_group_queue_overflow_interrupt() const
        {
            LL_ADC_EnableIT_JQOVF(ControllerTraitsT::skInstance);
        }

        VALLE_LL_WRAPPER void disable_inject_group_queue_overflow_interrupt() const
        {
            LL_ADC_DisableIT_JQOVF(ControllerTraitsT::skInstance);
        }

        [[nodiscard]] VALLE_LL_WRAPPER bool is_inject_group_queue_overflow_interrupt_enabled() const
        {
            return LL_ADC_IsEnabledIT_JQOVF(ControllerTraitsT::skInstance) != 0;
        }

        [[nodiscard]] VALLE_LL_WRAPPER bool is_analog_watchdog_1_interrupt_flag_active() const
        {
            return LL_ADC_IsActiveFlag_AWD1(ControllerTraitsT::skInstance) != 0;
        }

        VALLE_LL_WRAPPER void clear_analog_watchdog_1_interrupt_flag() const
        {
            LL_ADC_ClearFlag_AWD1(ControllerTraitsT::skInstance);
        }

        VALLE_LL_WRAPPER void enable_analog_watchdog_1_interrupt() const
        {
            LL_ADC_EnableIT_AWD1(ControllerTraitsT::skInstance);
        }

        VALLE_LL_WRAPPER void disable_analog_watchdog_1_interrupt() const
        {
            LL_ADC_DisableIT_AWD1(ControllerTraitsT::skInstance);
        }

        [[nodiscard]] VALLE_LL_WRAPPER bool is_analog_watchdog_1_interrupt_enabled() const
        {
            return LL_ADC_IsEnabledIT_AWD1(ControllerTraitsT::skInstance) != 0;
        }

        [[nodiscard]] VALLE_LL_WRAPPER bool is_analog_watchdog_2_interrupt_flag_active() const
        {
            return LL_ADC_IsActiveFlag_AWD2(ControllerTraitsT::skInstance) != 0;
        }

        VALLE_LL_WRAPPER void clear_analog_watchdog_2_interrupt_flag() const
        {
            LL_ADC_ClearFlag_AWD2(ControllerTraitsT::skInstance);
        }

        VALLE_LL_WRAPPER void enable_analog_watchdog_2_interrupt() const
        {
            LL_ADC_EnableIT_AWD2(ControllerTraitsT::skInstance);
        }

        VALLE_LL_WRAPPER void disable_analog_watchdog_2_interrupt() const
        {
            LL_ADC_DisableIT_AWD2(ControllerTraitsT::skInstance);
        }

        [[nodiscard]] VALLE_LL_WRAPPER bool is_analog_watchdog_2_interrupt_enabled() const
        {
            return LL_ADC_IsEnabledIT_AWD2(ControllerTraitsT::skInstance) != 0;
        }

        [[nodiscard]] VALLE_LL_WRAPPER bool is_analog_watchdog_3_interrupt_flag_active() const
        {
            return LL_ADC_IsActiveFlag_AWD3(ControllerTraitsT::skInstance) != 0;
        }

        VALLE_LL_WRAPPER void clear_analog_watchdog_3_interrupt_flag() const
        {
            LL_ADC_ClearFlag_AWD3(ControllerTraitsT::skInstance);
        }

        VALLE_LL_WRAPPER void enable_analog_watchdog_3_interrupt() const
        {
            LL_ADC_EnableIT_AWD3(ControllerTraitsT::skInstance);
        }

        VALLE_LL_WRAPPER void disable_analog_watchdog_3_interrupt() const
        {
            LL_ADC_DisableIT_AWD3(ControllerTraitsT::skInstance);
        }

        [[nodiscard]] VALLE_LL_WRAPPER bool is_analog_watchdog_3_interrupt_enabled() const
        {
            return LL_ADC_IsEnabledIT_AWD3(ControllerTraitsT::skInstance) != 0;
        }

        void enable_interrupt(const AdcControllerInterruptSource interrupt_source) const
        {
            switch (interrupt_source.to_number())
            {
                case AdcControllerInterruptSource::kReady.to_number():
                    enable_ready_interrupt();
                    break;
                case AdcControllerInterruptSource::kRegularEndOfConversion.to_number():
                    enable_regular_group_end_of_conversion_interrupt();
                    break;
                case AdcControllerInterruptSource::kRegularEndOfSequence.to_number():
                    enable_regular_group_end_of_sequence_interrupt();
                    break;
                case AdcControllerInterruptSource::kRegularEndOfSampling.to_number():
                    enable_regular_group_end_of_sampling_interrupt();
                    break;
                case AdcControllerInterruptSource::kInjectEndOfConversion.to_number():
                    enable_inject_group_end_of_conversion_interrupt();
                    break;
                case AdcControllerInterruptSource::kInjectEndOfSequence.to_number():
                    enable_inject_group_end_of_sequence_interrupt();
                    break;
                case AdcControllerInterruptSource::kInjectContextQueueOverflow.to_number():
                    enable_inject_group_queue_overflow_interrupt();
                    break;
                case AdcControllerInterruptSource::kOverrun.to_number():
                    enable_overrun_interrupt();
                    break;
                case AdcControllerInterruptSource::kAnalogWatchdog1.to_number():
                    enable_analog_watchdog_1_interrupt();
                    break;
                case AdcControllerInterruptSource::kAnalogWatchdog2.to_number():
                    enable_analog_watchdog_2_interrupt();
                    break;
                case AdcControllerInterruptSource::kAnalogWatchdog3.to_number():
                    enable_analog_watchdog_3_interrupt();
                    break;
                default:
                    expect(false, "Invalid ADC interrupt type");
            }
        }

        void disable_interrupt(const AdcControllerInterruptSource interrupt_source) const
        {
            switch (interrupt_source.to_number())
            {
                case AdcControllerInterruptSource::kReady.to_number():
                    disable_ready_interrupt();
                    break;
                case AdcControllerInterruptSource::kRegularEndOfConversion.to_number():
                    disable_regular_group_end_of_conversion_interrupt();
                    break;
                case AdcControllerInterruptSource::kRegularEndOfSequence.to_number():
                    disable_regular_group_end_of_sequence_interrupt();
                    break;
                case AdcControllerInterruptSource::kRegularEndOfSampling.to_number():
                    disable_regular_group_end_of_sampling_interrupt();
                    break;
                case AdcControllerInterruptSource::kInjectEndOfConversion.to_number():
                    disable_inject_group_end_of_conversion_interrupt();
                    break;
                case AdcControllerInterruptSource::kInjectEndOfSequence.to_number():
                    disable_inject_group_end_of_sequence_interrupt();
                    break;
                case AdcControllerInterruptSource::kInjectContextQueueOverflow.to_number():
                    disable_inject_group_queue_overflow_interrupt();
                    break;
                case AdcControllerInterruptSource::kOverrun.to_number():
                    disable_overrun_interrupt();
                    break;
                case AdcControllerInterruptSource::kAnalogWatchdog1.to_number():
                    disable_analog_watchdog_1_interrupt();
                    break;
                case AdcControllerInterruptSource::kAnalogWatchdog2.to_number():
                    disable_analog_watchdog_2_interrupt();
                    break;
                case AdcControllerInterruptSource::kAnalogWatchdog3.to_number():
                    disable_analog_watchdog_3_interrupt();
                    break;
                default:
                    expect(false, "Invalid ADC interrupt type");
            }
        }

        void clear_interrupt_flag(const AdcControllerInterruptSource interrupt_source) const
        {
            switch (interrupt_source.to_number())
            {
                case AdcControllerInterruptSource::kReady.to_number():
                    clear_ready_interrupt_flag();
                    break;
                case AdcControllerInterruptSource::kRegularEndOfConversion.to_number():
                    clear_regular_group_end_of_conversion_interrupt_flag();
                    break;
                case AdcControllerInterruptSource::kRegularEndOfSequence.to_number():
                    clear_regular_group_end_of_sequence_interrupt_flag();
                    break;
                case AdcControllerInterruptSource::kRegularEndOfSampling.to_number():
                    clear_regular_group_end_of_sampling_interrupt_flag();
                    break;
                case AdcControllerInterruptSource::kInjectEndOfConversion.to_number():
                    clear_inject_group_end_of_conversion_interrupt_flag();
                    break;
                case AdcControllerInterruptSource::kInjectEndOfSequence.to_number():
                    clear_inject_group_end_of_sequence_interrupt_flag();
                    break;
                case AdcControllerInterruptSource::kInjectContextQueueOverflow.to_number():
                    clear_inject_group_queue_overflow_interrupt_flag();
                    break;
                case AdcControllerInterruptSource::kOverrun.to_number():
                    clear_overrun_interrupt_flag();
                    break;
                case AdcControllerInterruptSource::kAnalogWatchdog1.to_number():
                    clear_analog_watchdog_1_interrupt_flag();
                    break;
                case AdcControllerInterruptSource::kAnalogWatchdog2.to_number():
                    clear_analog_watchdog_2_interrupt_flag();
                    break;
                case AdcControllerInterruptSource::kAnalogWatchdog3.to_number():
                    clear_analog_watchdog_3_interrupt_flag();
                    break;
                default:
                    expect(false, "Invalid ADC interrupt type");
            }
        }

        [[nodiscard]] bool is_interrupt_flag_active(const AdcControllerInterruptSource interrupt_source) const
        {
            switch (interrupt_source.to_number())
            {
                case AdcControllerInterruptSource::kReady.to_number():
                    return is_ready_interrupt_flag_active();
                    break;
                case AdcControllerInterruptSource::kRegularEndOfConversion.to_number():
                    return is_regular_group_end_of_conversion_interrupt_flag_active();
                    break;
                case AdcControllerInterruptSource::kRegularEndOfSequence.to_number():
                    return is_regular_group_end_of_sequence_interrupt_flag_active();
                    break;
                case AdcControllerInterruptSource::kRegularEndOfSampling.to_number():
                    return is_regular_group_end_of_sampling_interrupt_flag_active();
                    break;
                case AdcControllerInterruptSource::kInjectEndOfConversion.to_number():
                    return is_inject_group_end_of_conversion_interrupt_flag_active();
                    break;
                case AdcControllerInterruptSource::kInjectEndOfSequence.to_number():
                    return is_inject_group_end_of_sequence_interrupt_flag_active();
                    break;
                case AdcControllerInterruptSource::kInjectContextQueueOverflow.to_number():
                    return is_inject_group_queue_overflow_interrupt_flag_active();
                    break;
                case AdcControllerInterruptSource::kOverrun.to_number():
                    return is_overrun_interrupt_flag_active();
                    break;
                case AdcControllerInterruptSource::kAnalogWatchdog1.to_number():
                    return is_analog_watchdog_1_interrupt_flag_active();
                    break;
                case AdcControllerInterruptSource::kAnalogWatchdog2.to_number():
                    return is_analog_watchdog_2_interrupt_flag_active();
                    break;
                case AdcControllerInterruptSource::kAnalogWatchdog3.to_number():
                    return is_analog_watchdog_3_interrupt_flag_active();
                    break;
                default:
                    expect(false, "Invalid ADC interrupt type");
            }
        }
    };

    template <AdcChannelSpec tkChannelSpec>
    struct AdcChannelInterface
    {
        static constexpr AdcChannelSpec  skChannelSpec  = tkChannelSpec;
        static constexpr AdcControllerId skControllerId = tkChannelSpec.controller_id;
        using ControllerTraitsT                         = AdcControllerTraits<tkChannelSpec.controller_spec()>;

        VALLE_LL_WRAPPER void set_sampling_time(const AdcChannelSampleTime sample_time) const
        {
            LL_ADC_SetChannelSamplingTime(ControllerTraitsT::skInstance, skChannelId.to_ll(), sample_time.to_ll());
        }

        [[nodiscard]] VALLE_LL_WRAPPER AdcChannelSampleTime get_sampling_time() const
        {
            return AdcChannelSampleTime::from_ll(
                LL_ADC_GetChannelSamplingTime(ControllerTraitsT::skInstance, skChannelId.to_ll()));
        }

        VALLE_LL_WRAPPER void set_input_mode(const AdcChannelInputMode input_mode) const
        {
            LL_ADC_SetChannelSingleDiff(ControllerTraitsT::skInstance, skChannelId.to_ll(), input_mode.to_ll());
        }

        [[nodiscard]] VALLE_LL_WRAPPER AdcChannelInputMode get_input_mode() const
        {
            return AdcChannelInputMode::from_ll(
                LL_ADC_GetChannelSingleDiff(ControllerTraitsT::skInstance, skChannelId.to_ll()));
        }
    };

    // ==============================================================================
    // INJECT CHANNEL INTERFACE
    // =============================================================================

    template <AdcInjectChannelRankSpec tkRankSpec>
    struct AdcInjectChannelInterface
    {
        static constexpr AdcInjectChannelRankSpec skRankSpec     = tkRankSpec;
        static constexpr AdcControllerId          skControllerId = skRankSpec.controller_id;
        static constexpr AdcInjectChannelRank     skRank         = skRankSpec.rank;
        using ControllerTraitsT                                  = AdcControllerTraits<skRankSpec.controller_spec()>;
    };

    // ============================================================================
    // REGULAR CHANNEL INTERFACE
    // ============================================================================

    template <AdcRegularChannelRankSpec tkRankSpec>
    struct AdcRegularChannelInterface
    {
        static constexpr AdcRegularChannelRankSpec skRankSpec     = tkRankSpec;
        static constexpr AdcControllerId           skControllerId = skRankSpec.controller_id;
        static constexpr AdcRegularChannelRank     skRank         = skRankSpec.rank;
        using ControllerTraitsT                                   = AdcControllerTraits<skRankSpec.controller_spec()>;
    };

    // ==============================================================================
    // OFFSET BLOCK INTERFACE
    // ==============================================================================

    template <AdcOffsetBlockSpec tkOffsetBlockSpec>
    struct AdcOffsetBlockInterface
    {
        static constexpr AdcOffsetBlockSpec skOffsetBlockSpec = tkOffsetBlockSpec;
        static constexpr AdcOffsetBlockId   skOffsetBlockId   = tkOffsetBlockSpec.block_id;
        using ControllerTraitsT = AdcControllerTraits<tkOffsetBlockSpec.controller_spec()>;

        VALLE_LL_WRAPPER void set_offset(const AdcChannelId channel_id, const uint32_t offset_value) const
        {
            LL_ADC_SetOffset(ControllerTraitsT::skInstance, skOffsetBlockId.to_ll(), channel_id.to_ll(), offset_value);
        }

        [[nodiscard]] VALLE_LL_WRAPPER AdcChannelId get_channel() const
        {
            return AdcChannelId::from_ll(
                LL_ADC_GetOffsetChannel(ControllerTraitsT::skInstance, skOffsetBlockId.to_ll()));
        }

        [[nodiscard]] VALLE_LL_WRAPPER uint32_t get_level() const
        {
            return LL_ADC_GetOffsetLevel(ControllerTraitsT::skInstance, skOffsetBlockId.to_ll());
        }

        VALLE_LL_WRAPPER void enable()
        {
            LL_ADC_SetOffsetState(ControllerTraitsT::skInstance, skOffsetBlockId.to_ll(), LL_ADC_OFFSET_ENABLE);
        }

        VALLE_LL_WRAPPER void disable()
        {
            LL_ADC_SetOffsetState(ControllerTraitsT::skInstance, skOffsetBlockId.to_ll(), LL_ADC_OFFSET_DISABLE);
        }

        [[nodiscard]] VALLE_LL_WRAPPER bool get_enabled() const
        {
            return LL_ADC_GetOffsetState(ControllerTraitsT::skInstance, skOffsetBlockId.to_ll()) ==
                   LL_ADC_OFFSET_ENABLE;
        }

        VALLE_LL_WRAPPER void set_sign_positive() const
        {
            LL_ADC_SetOffsetSign(ControllerTraitsT::skInstance, skOffsetBlockId.to_ll(), LL_ADC_OFFSET_SIGN_POSITIVE);
        }

        VALLE_LL_WRAPPER void set_sign_negative() const
        {
            LL_ADC_SetOffsetSign(ControllerTraitsT::skInstance, skOffsetBlockId.to_ll(), LL_ADC_OFFSET_SIGN_NEGATIVE);
        }

        [[nodiscard]] VALLE_LL_WRAPPER bool get_offset_sign_positive() const
        {
            return LL_ADC_GetOffsetSign(ControllerTraitsT::skInstance, skOffsetBlockId.to_ll()) ==
                   LL_ADC_OFFSET_SIGN_POSITIVE;
        }

        [[nodiscard]] VALLE_LL_WRAPPER bool get_sign_negative() const
        {
            return LL_ADC_GetOffsetSign(ControllerTraitsT::skInstance, skOffsetBlockId.to_ll()) ==
                   LL_ADC_OFFSET_SIGN_NEGATIVE;
        }

        VALLE_LL_WRAPPER void enable_saturation() const
        {
            LL_ADC_SetOffsetSaturation(
                ControllerTraitsT::skInstance, skOffsetBlockId.to_ll(), LL_ADC_OFFSET_SATURATION_ENABLE);
        }

        VALLE_LL_WRAPPER void disable_saturation() const
        {
            LL_ADC_SetOffsetSaturation(
                ControllerTraitsT::skInstance, skOffsetBlockId.to_ll(), LL_ADC_OFFSET_SATURATION_DISABLE);
        }

        [[nodiscard]] VALLE_LL_WRAPPER bool get_saturation_enabled() const
        {
            return LL_ADC_GetOffsetSaturation(ControllerTraitsT::skInstance, skOffsetBlockId.to_ll()) ==
                   LL_ADC_OFFSET_SATURATION_ENABLE;
        }
    };

    // ==============================================================================
    // ANALOG WATCHDOG INTERFACE
    // ==============================================================================
    template <AdcAnalogWatchdogSpec tkAnalogWatchdogSpec>
    struct AdcAnalogWatchdogInterface
    {
        static constexpr AdcAnalogWatchdogSpec skAnalogWatchdogSpec = tkAnalogWatchdogSpec;
        static constexpr AdcAnalogWatchdogId   skWatchdogId         = tkAnalogWatchdogSpec.watchdog_id;
        using ControllerTraitsT = AdcControllerTraits<tkAnalogWatchdogSpec.controller_spec()>;

        VALLE_LL_WRAPPER void set_monitor_channels(const AdcAnalogWatchdogChannelGroup awd_channel_group) const
        {
            LL_ADC_SetAnalogWDMonitChannels(
                ControllerTraitsT::skInstance, skWatchdogId.to_ll(), awd_channel_group.to_ll());
        }

        [[nodiscard]] VALLE_LL_WRAPPER AdcAnalogWatchdogChannelGroup get_monitor_channels() const
        {
            return AdcAnalogWatchdogChannelGroup::from_ll(
                LL_ADC_GetAnalogWDMonitChannels(ControllerTraitsT::skInstance, skWatchdogId.to_ll()));
        }

        VALLE_LL_WRAPPER void config_thresholds(const uint32_t awd_threshold_high_value,
                                                const uint32_t awd_threshold_low_value) const
        {
            LL_ADC_ConfigAnalogWDThresholds(
                ControllerTraitsT::skInstance, skWatchdogId.to_ll(), awd_threshold_high_value, awd_threshold_low_value);
        }

        VALLE_LL_WRAPPER void set_low_threshold(const uint32_t awd_thresholds_low) const
        {
            LL_ADC_SetAnalogWDThresholds(
                ControllerTraitsT::skInstance, skWatchdogId.to_ll(), LL_ADC_AWD_THRESHOLD_LOW, awd_thresholds_low);
        }

        [[nodiscard]] VALLE_LL_WRAPPER uint32_t get_low_threshold() const
        {
            return LL_ADC_GetAnalogWDThresholds(
                ControllerTraitsT::skInstance, skWatchdogId.to_ll(), LL_ADC_AWD_THRESHOLD_LOW);
        }

        VALLE_LL_WRAPPER void set_high_threshold(const uint32_t awd_thresholds_high) const
        {
            LL_ADC_SetAnalogWDThresholds(
                ControllerTraitsT::skInstance, skWatchdogId.to_ll(), LL_ADC_AWD_THRESHOLD_HIGH, awd_thresholds_high);
        }

        [[nodiscard]] VALLE_LL_WRAPPER uint32_t get_high_threshold() const
        {
            return LL_ADC_GetAnalogWDThresholds(
                ControllerTraitsT::skInstance, skWatchdogId.to_ll(), LL_ADC_AWD_THRESHOLD_HIGH);
        }

        VALLE_LL_WRAPPER void set_filtering(const AdcAnalogWatchdogFiltering filtering) const
        {
            LL_ADC_SetAWDFilteringConfiguration(ControllerTraitsT::skInstance, skWatchdogId.to_ll(), filtering.to_ll());
        }

        [[nodiscard]] VALLE_LL_WRAPPER AdcAnalogWatchdogFiltering get_filtering() const
        {
            return AdcAnalogWatchdogFiltering::from_ll(
                LL_ADC_GetAWDFilteringConfiguration(ControllerTraitsT::skInstance, skWatchdogId.to_ll()));
        }
    };

}  // namespace valle::platform

VALLE_OPTIMIZE_POP