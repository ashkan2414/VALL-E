#pragma once

#include "stm32g4xx_ll_bus.h"
#include "stm32g4xx_ll_hrtim.h"
#include "valle/platform/hardware/hrtim/id.hpp"
#include "valle/utils/tmp.hpp"

namespace valle::platform
{
    // ============================================================================
    // ENUMERATIONS
    // ============================================================================
    struct HrtimFaultSource : public LLDriverEnumValue<HrtimFaultSource>
    {
        static const HrtimFaultSource kDigitalInput;
        static const HrtimFaultSource kInternal;
        static const HrtimFaultSource kExternalEventInput;
    };

    inline constexpr HrtimFaultSource HrtimFaultSource::kDigitalInput = HrtimFaultSource::from_ll<LL_HRTIM_FLT_SRC_DIGITALINPUT>();
    inline constexpr HrtimFaultSource HrtimFaultSource::kInternal = HrtimFaultSource::from_ll<LL_HRTIM_FLT_SRC_INTERNAL>();
    inline constexpr HrtimFaultSource HrtimFaultSource::kExternalEventInput = HrtimFaultSource::from_ll<LL_HRTIM_FLT_SRC_EEVINPUT>();


    struct HrtimFaultPolarity : public LLDriverEnumValue<HrtimFaultPolarity>
    {
        static const HrtimFaultPolarity kLow;
        static const HrtimFaultPolarity kHigh;
    };

    inline constexpr HrtimFaultPolarity HrtimFaultPolarity::kLow = HrtimFaultPolarity::from_ll<LL_HRTIM_FLT_POLARITY_LOW>();
    inline constexpr HrtimFaultPolarity HrtimFaultPolarity::kHigh = HrtimFaultPolarity::from_ll<LL_HRTIM_FLT_POLARITY_HIGH>();


    struct HrtimFaultFilter : public LLDriverEnumValue<HrtimFaultFilter>
    {
        static const HrtimFaultFilter kNone;
        static const HrtimFaultFilter kFilter1;
        static const HrtimFaultFilter kFilter2;
        static const HrtimFaultFilter kFilter3;
        static const HrtimFaultFilter kFilter4;
        static const HrtimFaultFilter kFilter5;
        static const HrtimFaultFilter kFilter6;
        static const HrtimFaultFilter kFilter7;
        static const HrtimFaultFilter kFilter8;
        static const HrtimFaultFilter kFilter9;
        static const HrtimFaultFilter kFilter10;
        static const HrtimFaultFilter kFilter11;
        static const HrtimFaultFilter kFilter12;
        static const HrtimFaultFilter kFilter13;
        static const HrtimFaultFilter kFilter14;
        static const HrtimFaultFilter kFilter15;
    };

    inline constexpr HrtimFaultFilter HrtimFaultFilter::kNone = HrtimFaultFilter::from_ll<LL_HRTIM_FLT_FILTER_NONE>();
    inline constexpr HrtimFaultFilter HrtimFaultFilter::kFilter1 = HrtimFaultFilter::from_ll<LL_HRTIM_FLT_FILTER_1>();
    inline constexpr HrtimFaultFilter HrtimFaultFilter::kFilter2 = HrtimFaultFilter::from_ll<LL_HRTIM_FLT_FILTER_2>();
    inline constexpr HrtimFaultFilter HrtimFaultFilter::kFilter3 = HrtimFaultFilter::from_ll<LL_HRTIM_FLT_FILTER_3>();
    inline constexpr HrtimFaultFilter HrtimFaultFilter::kFilter4 = HrtimFaultFilter::from_ll<LL_HRTIM_FLT_FILTER_4>();
    inline constexpr HrtimFaultFilter HrtimFaultFilter::kFilter5 = HrtimFaultFilter::from_ll<LL_HRTIM_FLT_FILTER_5>();
    inline constexpr HrtimFaultFilter HrtimFaultFilter::kFilter6 = HrtimFaultFilter::from_ll<LL_HRTIM_FLT_FILTER_6>();
    inline constexpr HrtimFaultFilter HrtimFaultFilter::kFilter7 = HrtimFaultFilter::from_ll<LL_HRTIM_FLT_FILTER_7>();
    inline constexpr HrtimFaultFilter HrtimFaultFilter::kFilter8 = HrtimFaultFilter::from_ll<LL_HRTIM_FLT_FILTER_8>();
    inline constexpr HrtimFaultFilter HrtimFaultFilter::kFilter9 = HrtimFaultFilter::from_ll<LL_HRTIM_FLT_FILTER_9>();
    inline constexpr HrtimFaultFilter HrtimFaultFilter::kFilter10 = HrtimFaultFilter::from_ll<LL_HRTIM_FLT_FILTER_10>();
    inline constexpr HrtimFaultFilter HrtimFaultFilter::kFilter11 = HrtimFaultFilter::from_ll<LL_HRTIM_FLT_FILTER_11>();
    inline constexpr HrtimFaultFilter HrtimFaultFilter::kFilter12 = HrtimFaultFilter::from_ll<LL_HRTIM_FLT_FILTER_12>();
    inline constexpr HrtimFaultFilter HrtimFaultFilter::kFilter13 = HrtimFaultFilter::from_ll<LL_HRTIM_FLT_FILTER_13>();
    inline constexpr HrtimFaultFilter HrtimFaultFilter::kFilter14 = HrtimFaultFilter::from_ll<LL_HRTIM_FLT_FILTER_14>();
    inline constexpr HrtimFaultFilter HrtimFaultFilter::kFilter15 = HrtimFaultFilter::from_ll<LL_HRTIM_FLT_FILTER_15>();


    struct HrtimFaultPrescaler : public LLDriverEnumValue<HrtimFaultPrescaler>
    {
        static const HrtimFaultPrescaler kDiv1;
        static const HrtimFaultPrescaler kDiv2;
        static const HrtimFaultPrescaler kDiv4;
        static const HrtimFaultPrescaler kDiv8;
    };

    inline constexpr HrtimFaultPrescaler HrtimFaultPrescaler::kDiv1 = HrtimFaultPrescaler::from_ll<LL_HRTIM_FLT_PRESCALER_DIV1>();
    inline constexpr HrtimFaultPrescaler HrtimFaultPrescaler::kDiv2 = HrtimFaultPrescaler::from_ll<LL_HRTIM_FLT_PRESCALER_DIV2>();
    inline constexpr HrtimFaultPrescaler HrtimFaultPrescaler::kDiv4 = HrtimFaultPrescaler::from_ll<LL_HRTIM_FLT_PRESCALER_DIV4>();
    inline constexpr HrtimFaultPrescaler HrtimFaultPrescaler::kDiv8 = HrtimFaultPrescaler::from_ll<LL_HRTIM_FLT_PRESCALER_DIV8>();


    struct HrtimFaultBlankingSource : public LLDriverEnumValue<HrtimFaultBlankingSource>
    {
        static const HrtimFaultBlankingSource kRSTAligned;
        static const HrtimFaultBlankingSource kMoving;
    };

    inline constexpr HrtimFaultBlankingSource HrtimFaultBlankingSource::kRSTAligned = HrtimFaultBlankingSource::from_ll<LL_HRTIM_FLT_BLANKING_RSTALIGNED>();
    inline constexpr HrtimFaultBlankingSource HrtimFaultBlankingSource::kMoving = HrtimFaultBlankingSource::from_ll<LL_HRTIM_FLT_BLANKING_MOVING>();


    struct HrtimFaultResetMode : public LLDriverEnumValue<HrtimFaultResetMode>
    {
        static const HrtimFaultResetMode kUnconditional;
        static const HrtimFaultResetMode kConditional;
    };

    inline constexpr HrtimFaultResetMode HrtimFaultResetMode::kUnconditional = HrtimFaultResetMode::from_ll<LL_HRTIM_FLT_COUNTERRST_UNCONDITIONAL>();
    inline constexpr HrtimFaultResetMode HrtimFaultResetMode::kConditional = HrtimFaultResetMode::from_ll<LL_HRTIM_FLT_COUNTERRST_CONDITIONAL>();


    struct HrtimExternalEventSource : public LLDriverEnumValue<HrtimExternalEventSource>
    {
        static const HrtimExternalEventSource kSource1;
        static const HrtimExternalEventSource kSource2;
        static const HrtimExternalEventSource kSource3;
        static const HrtimExternalEventSource kSource4;
    };

    inline constexpr HrtimExternalEventSource HrtimExternalEventSource::kSource1 = HrtimExternalEventSource::from_ll<0x00000000U>();
    inline constexpr HrtimExternalEventSource HrtimExternalEventSource::kSource2 = HrtimExternalEventSource::from_ll<Hrtim_EECR1_EE1SRC_0>();
    inline constexpr HrtimExternalEventSource HrtimExternalEventSource::kSource3 = HrtimExternalEventSource::from_ll<Hrtim_EECR1_EE1SRC_1>();
    inline constexpr HrtimExternalEventSource HrtimExternalEventSource::kSource4 = HrtimExternalEventSource::from_ll<Hrtim_EECR1_EE1SRC_1 | Hrtim_EECR1_EE1SRC_0>();


    struct HrtimExternalEventPolarity : public LLDriverEnumValue<HrtimExternalEventPolarity>
    {
        static const HrtimExternalEventPolarity kHigh;
        static const HrtimExternalEventPolarity kLow;
    };

    inline constexpr HrtimExternalEventPolarity HrtimExternalEventPolarity::kHigh = HrtimExternalEventPolarity::from_ll<LL_HRTIM_EE_POLARITY_HIGH>();
    inline constexpr HrtimExternalEventPolarity HrtimExternalEventPolarity::kLow = HrtimExternalEventPolarity::from_ll<LL_HRTIM_EE_POLARITY_LOW>();


    struct HrtimExternalEventSensitivity : public LLDriverEnumValue<HrtimExternalEventSensitivity>
    {
        static const HrtimExternalEventSensitivity kLevel;
        static const HrtimExternalEventSensitivity kRisingEdge;
        static const HrtimExternalEventSensitivity kFallingEdge;
        static const HrtimExternalEventSensitivity kBothEdges;
    };

    inline constexpr HrtimExternalEventSensitivity HrtimExternalEventSensitivity::kLevel = HrtimExternalEventSensitivity::from_ll<LL_HRTIM_EE_SENSITIVITY_LEVEL>();
    inline constexpr HrtimExternalEventSensitivity HrtimExternalEventSensitivity::kRisingEdge = HrtimExternalEventSensitivity::from_ll<LL_HRTIM_EE_SENSITIVITY_RISINGEDGE>();
    inline constexpr HrtimExternalEventSensitivity HrtimExternalEventSensitivity::kFallingEdge = HrtimExternalEventSensitivity::from_ll<LL_HRTIM_EE_SENSITIVITY_FALLINGEDGE>();
    inline constexpr HrtimExternalEventSensitivity HrtimExternalEventSensitivity::kBothEdges = HrtimExternalEventSensitivity::from_ll<LL_HRTIM_EE_SENSITIVITY_BOTHEDGES>();


    struct HrtimExternalEventFastMode : public LLDriverEnumValue<HrtimExternalEventFastMode>
    {
        static const HrtimExternalEventFastMode kDisable;
        static const HrtimExternalEventFastMode kEnable;
    };

    inline constexpr HrtimExternalEventFastMode HrtimExternalEventFastMode::kDisable = HrtimExternalEventFastMode::from_ll<LL_HRTIM_EE_FASTMODE_DISABLE>();
    inline constexpr HrtimExternalEventFastMode HrtimExternalEventFastMode::kEnable = HrtimExternalEventFastMode::from_ll<LL_HRTIM_EE_FASTMODE_ENABLE>();


    struct HrtimExternalEventFilter : public LLDriverEnumValue<HrtimExternalEventFilter>
    {
        static const HrtimExternalEventFilter kNone;
        static const HrtimExternalEventFilter kFilter1;
        static const HrtimExternalEventFilter kFilter2;
        static const HrtimExternalEventFilter kFilter3;
        static const HrtimExternalEventFilter kFilter4;
        static const HrtimExternalEventFilter kFilter5;
        static const HrtimExternalEventFilter kFilter6;
        static const HrtimExternalEventFilter kFilter7;
        static const HrtimExternalEventFilter kFilter8;
        static const HrtimExternalEventFilter kFilter9;
        static const HrtimExternalEventFilter kFilter10;
        static const HrtimExternalEventFilter kFilter11;
        static const HrtimExternalEventFilter kFilter12;
        static const HrtimExternalEventFilter kFilter13;
        static const HrtimExternalEventFilter kFilter14;
        static const HrtimExternalEventFilter kFilter15;
    };

    inline constexpr HrtimExternalEventFilter HrtimExternalEventFilter::kNone = HrtimExternalEventFilter::from_ll<LL_HRTIM_EE_FILTER_NONE>();
    inline constexpr HrtimExternalEventFilter HrtimExternalEventFilter::kFilter1 = HrtimExternalEventFilter::from_ll<LL_HRTIM_EE_FILTER_1>();
    inline constexpr HrtimExternalEventFilter HrtimExternalEventFilter::kFilter2 = HrtimExternalEventFilter::from_ll<LL_HRTIM_EE_FILTER_2>();
    inline constexpr HrtimExternalEventFilter HrtimExternalEventFilter::kFilter3 = HrtimExternalEventFilter::from_ll<LL_HRTIM_EE_FILTER_3>();
    inline constexpr HrtimExternalEventFilter HrtimExternalEventFilter::kFilter4 = HrtimExternalEventFilter::from_ll<LL_HRTIM_EE_FILTER_4>();
    inline constexpr HrtimExternalEventFilter HrtimExternalEventFilter::kFilter5 = HrtimExternalEventFilter::from_ll<LL_HRTIM_EE_FILTER_5>();
    inline constexpr HrtimExternalEventFilter HrtimExternalEventFilter::kFilter6 = HrtimExternalEventFilter::from_ll<LL_HRTIM_EE_FILTER_6>();
    inline constexpr HrtimExternalEventFilter HrtimExternalEventFilter::kFilter7 = HrtimExternalEventFilter::from_ll<LL_HRTIM_EE_FILTER_7>();
    inline constexpr HrtimExternalEventFilter HrtimExternalEventFilter::kFilter8 = HrtimExternalEventFilter::from_ll<LL_HRTIM_EE_FILTER_8>();
    inline constexpr HrtimExternalEventFilter HrtimExternalEventFilter::kFilter9 = HrtimExternalEventFilter::from_ll<LL_HRTIM_EE_FILTER_9>();
    inline constexpr HrtimExternalEventFilter HrtimExternalEventFilter::kFilter10 = HrtimExternalEventFilter::from_ll<LL_HRTIM_EE_FILTER_10>();
    inline constexpr HrtimExternalEventFilter HrtimExternalEventFilter::kFilter11 = HrtimExternalEventFilter::from_ll<LL_HRTIM_EE_FILTER_11>();
    inline constexpr HrtimExternalEventFilter HrtimExternalEventFilter::kFilter12 = HrtimExternalEventFilter::from_ll<LL_HRTIM_EE_FILTER_12>();
    inline constexpr HrtimExternalEventFilter HrtimExternalEventFilter::kFilter13 = HrtimExternalEventFilter::from_ll<LL_HRTIM_EE_FILTER_13>();
    inline constexpr HrtimExternalEventFilter HrtimExternalEventFilter::kFilter14 = HrtimExternalEventFilter::from_ll<LL_HRTIM_EE_FILTER_14>();
    inline constexpr HrtimExternalEventFilter HrtimExternalEventFilter::kFilter15 = HrtimExternalEventFilter::from_ll<LL_HRTIM_EE_FILTER_15>();


    struct HrtimExternalEventPrescaler : public LLDriverEnumValue<HrtimExternalEventPrescaler>
    {
        static const HrtimExternalEventPrescaler kDiv1;
        static const HrtimExternalEventPrescaler kDiv2;
        static const HrtimExternalEventPrescaler kDiv4;
        static const HrtimExternalEventPrescaler kDiv8;
    };

    inline constexpr HrtimExternalEventPrescaler HrtimExternalEventPrescaler::kDiv1 = HrtimExternalEventPrescaler::from_ll<LL_HRTIM_EE_PRESCALER_DIV1>();
    inline constexpr HrtimExternalEventPrescaler HrtimExternalEventPrescaler::kDiv2 = HrtimExternalEventPrescaler::from_ll<LL_HRTIM_EE_PRESCALER_DIV2>();
    inline constexpr HrtimExternalEventPrescaler HrtimExternalEventPrescaler::kDiv4 = HrtimExternalEventPrescaler::from_ll<LL_HRTIM_EE_PRESCALER_DIV4>();
    inline constexpr HrtimExternalEventPrescaler HrtimExternalEventPrescaler::kDiv8 = HrtimExternalEventPrescaler::from_ll<LL_HRTIM_EE_PRESCALER_DIV8>();


    struct HrtimTimerPrescaler : public LLDriverEnumValue<HrtimTimerPrescaler>
    {
        static const HrtimTimerPrescaler kMul32;
        static const HrtimTimerPrescaler kMul16;
        static const HrtimTimerPrescaler kMul8;
        static const HrtimTimerPrescaler kMul4;
        static const HrtimTimerPrescaler kMul2;
        static const HrtimTimerPrescaler kDiv1;
        static const HrtimTimerPrescaler kDiv2;
        static const HrtimTimerPrescaler kDiv4;
    };

    inline constexpr HrtimTimerPrescaler HrtimTimerPrescaler::kMul32 = HrtimTimerPrescaler::from_ll<LL_HRTIM_PRESCALERRATIO_MUL32>();
    inline constexpr HrtimTimerPrescaler HrtimTimerPrescaler::kMul16 = HrtimTimerPrescaler::from_ll<LL_HRTIM_PRESCALERRATIO_MUL16>();
    inline constexpr HrtimTimerPrescaler HrtimTimerPrescaler::kMul8 = HrtimTimerPrescaler::from_ll<LL_HRTIM_PRESCALERRATIO_MUL8>();
    inline constexpr HrtimTimerPrescaler HrtimTimerPrescaler::kMul4 = HrtimTimerPrescaler::from_ll<LL_HRTIM_PRESCALERRATIO_MUL4>();
    inline constexpr HrtimTimerPrescaler HrtimTimerPrescaler::kMul2 = HrtimTimerPrescaler::from_ll<LL_HRTIM_PRESCALERRATIO_MUL2>();
    inline constexpr HrtimTimerPrescaler HrtimTimerPrescaler::kDiv1 = HrtimTimerPrescaler::from_ll<LL_HRTIM_PRESCALERRATIO_DIV1>();
    inline constexpr HrtimTimerPrescaler HrtimTimerPrescaler::kDiv2 = HrtimTimerPrescaler::from_ll<LL_HRTIM_PRESCALERRATIO_DIV2>();
    inline constexpr HrtimTimerPrescaler HrtimTimerPrescaler::kDiv4 = HrtimTimerPrescaler::from_ll<LL_HRTIM_PRESCALERRATIO_DIV4>();


    struct HrtimTimerDeadtimePrescaler : public LLDriverEnumValue<HrtimTimerDeadtimePrescaler>
    {
        static const HrtimTimerDeadtimePrescaler kMul8;
        static const HrtimTimerDeadtimePrescaler kMul4;
        static const HrtimTimerDeadtimePrescaler kMul2;
        static const HrtimTimerDeadtimePrescaler kDiv1;
        static const HrtimTimerDeadtimePrescaler kDiv2;
        static const HrtimTimerDeadtimePrescaler kDiv4;
        static const HrtimTimerDeadtimePrescaler kDiv8;
        static const HrtimTimerDeadtimePrescaler kDiv16;
    };

    inline constexpr HrtimTimerDeadtimePrescaler HrtimTimerDeadtimePrescaler::kMul8 = HrtimTimerDeadtimePrescaler::from_ll<LL_HRTIM_DT_PRESCALER_MUL8>();
    inline constexpr HrtimTimerDeadtimePrescaler HrtimTimerDeadtimePrescaler::kMul4 = HrtimTimerDeadtimePrescaler::from_ll<LL_HRTIM_DT_PRESCALER_MUL4>();
    inline constexpr HrtimTimerDeadtimePrescaler HrtimTimerDeadtimePrescaler::kMul2 = HrtimTimerDeadtimePrescaler::from_ll<LL_HRTIM_DT_PRESCALER_MUL2>();
    inline constexpr HrtimTimerDeadtimePrescaler HrtimTimerDeadtimePrescaler::kDiv1 = HrtimTimerDeadtimePrescaler::from_ll<LL_HRTIM_DT_PRESCALER_DIV1>();
    inline constexpr HrtimTimerDeadtimePrescaler HrtimTimerDeadtimePrescaler::kDiv2 = HrtimTimerDeadtimePrescaler::from_ll<LL_HRTIM_DT_PRESCALER_DIV2>();
    inline constexpr HrtimTimerDeadtimePrescaler HrtimTimerDeadtimePrescaler::kDiv4 = HrtimTimerDeadtimePrescaler::from_ll<LL_HRTIM_DT_PRESCALER_DIV4>();
    inline constexpr HrtimTimerDeadtimePrescaler HrtimTimerDeadtimePrescaler::kDiv8 = HrtimTimerDeadtimePrescaler::from_ll<LL_HRTIM_DT_PRESCALER_DIV8>();
    inline constexpr HrtimTimerDeadtimePrescaler HrtimTimerDeadtimePrescaler::kDiv16 = HrtimTimerDeadtimePrescaler::from_ll<LL_HRTIM_DT_PRESCALER_DIV16>();


    struct HrtimTimerCounterMode : public LLDriverEnumValue<HrtimTimerCounterMode>
    {
        static const HrtimTimerCounterMode kContinuous;
        static const HrtimTimerCounterMode kSingleShot;
        static const HrtimTimerCounterMode kBurst;
    };

    inline constexpr HrtimTimerCounterMode HrtimTimerCounterMode::kContinuous = HrtimTimerCounterMode::from_ll<LL_HRTIM_MODE_CONTINUOUS>();
    inline constexpr HrtimTimerCounterMode HrtimTimerCounterMode::kSingleShot = HrtimTimerCounterMode::from_ll<LL_HRTIM_MODE_SINGLESHOT>();
    inline constexpr HrtimTimerCounterMode HrtimTimerCounterMode::kBurst = HrtimTimerCounterMode::from_ll<LL_HRTIM_MODE_RETRIGGERABLE>();


    struct HrtimTimerCountingMode : public LLDriverEnumValue<HrtimTimerCountingMode>
    {
        static const HrtimTimerCountingMode kUp;
        static const HrtimTimerCountingMode kUpDown;
    };

    inline constexpr HrtimTimerCountingMode HrtimTimerCountingMode::kUp = HrtimTimerCountingMode::from_ll<LL_HRTIM_COUNTING_MODE_UP>();
    inline constexpr HrtimTimerCountingMode HrtimTimerCountingMode::kUpDown = HrtimTimerCountingMode::from_ll<LL_HRTIM_COUNTING_MODE_UP_DOWN>();


    struct HrtimTimerRolloverMode : public LLDriverEnumValue<HrtimTimerRolloverMode>
    {
        static const HrtimTimerRolloverMode kPeriodReset;
        static const HrtimTimerRolloverMode kReset;
        static const HrtimTimerRolloverMode kPeriod;
    };

    inline constexpr HrtimTimerRolloverMode HrtimTimerRolloverMode::kPeriodReset = HrtimTimerRolloverMode::from_ll<LL_HRTIM_ROLLOVER_MODE_BOTH>();
    inline constexpr HrtimTimerRolloverMode HrtimTimerRolloverMode::kReset = HrtimTimerRolloverMode::from_ll<LL_HRTIM_ROLLOVER_MODE_RST>();
    inline constexpr HrtimTimerRolloverMode HrtimTimerRolloverMode::kPeriod = HrtimTimerRolloverMode::from_ll<LL_HRTIM_ROLLOVER_MODE_PER>();


    struct HrtimTimerCompareUnit : public EnumValue<HrtimTimerCompareUnit, uint32_t>
    {
        static const HrtimTimerCompareUnit kCompare1;
        static const HrtimTimerCompareUnit kCompare2;
        static const HrtimTimerCompareUnit kCompare3;
        static const HrtimTimerCompareUnit kCompare4;
    };

    inline constexpr HrtimTimerCompareUnit HrtimTimerCompareUnit::kCompare1 = HrtimTimerCompareUnit::from_number<1>();
    inline constexpr HrtimTimerCompareUnit HrtimTimerCompareUnit::kCompare2 = HrtimTimerCompareUnit::from_number<2>();
    inline constexpr HrtimTimerCompareUnit HrtimTimerCompareUnit::kCompare3 = HrtimTimerCompareUnit::from_number<3>();
    inline constexpr HrtimTimerCompareUnit HrtimTimerCompareUnit::kCompare4 = HrtimTimerCompareUnit::from_number<4>();


    struct HrtimTimerOutputSetSource : public LLDriverEnumValue<HrtimTimerOutputSetSource>
    {
        static const HrtimTimerOutputSetSource kNone;
        static const HrtimTimerOutputSetSource kResync;
        static const HrtimTimerOutputSetSource kTimerPeriod;
        static const HrtimTimerOutputSetSource kTimerCompare1;
        static const HrtimTimerOutputSetSource kTimerCompare2;
        static const HrtimTimerOutputSetSource kTimerCompare3;
        static const HrtimTimerOutputSetSource kTimerCompare4;
        static const HrtimTimerOutputSetSource kMasterPeriod;
        static const HrtimTimerOutputSetSource kMasterCompare1;
        static const HrtimTimerOutputSetSource kMasterCompare2;
        static const HrtimTimerOutputSetSource kMasterCompare3;
        static const HrtimTimerOutputSetSource kMasterCompare4;
        static const HrtimTimerOutputSetSource kTimerAEvent1TimerBCompare1;
        static const HrtimTimerOutputSetSource kTimerAEvent2TimerBCompare2;
        static const HrtimTimerOutputSetSource kTimerAEvent3TimerCCompare2;
        static const HrtimTimerOutputSetSource kTimerAEvent4TimerCCompare3;
        static const HrtimTimerOutputSetSource kTimerAEvent5TimerDCompare1;
        static const HrtimTimerOutputSetSource kTimerAEvent6TimerDCompare2;
        static const HrtimTimerOutputSetSource kTimerAEvent7TimerECompare3;
        static const HrtimTimerOutputSetSource kTimerAEvent8TimerECompare4;
        static const HrtimTimerOutputSetSource kTimerAEvent9TimerFCompare4;
        static const HrtimTimerOutputSetSource kTimerBEvent1TimerACompare1;
        static const HrtimTimerOutputSetSource kTimerBEvent2TimerACompare2;
        static const HrtimTimerOutputSetSource kTimerBEvent3TimerCCompare3;
        static const HrtimTimerOutputSetSource kTimerBEvent4TimerCCompare4;
        static const HrtimTimerOutputSetSource kTimerBEvent5TimerDCompare3;
        static const HrtimTimerOutputSetSource kTimerBEvent6TimerDCompare4;
        static const HrtimTimerOutputSetSource kTimerBEvent7TimerECompare1;
        static const HrtimTimerOutputSetSource kTimerBEvent8TimerECompare2;
        static const HrtimTimerOutputSetSource kTimerBEvent9TimerFCompare3;
        static const HrtimTimerOutputSetSource kTimerCEvent1TimerACompare2;
        static const HrtimTimerOutputSetSource kTimerCEvent2TimerACompare3;
        static const HrtimTimerOutputSetSource kTimerCEvent3TimerBCompare2;
        static const HrtimTimerOutputSetSource kTimerCEvent4TimerBCompare3;
        static const HrtimTimerOutputSetSource kTimerCEvent5TimerDCompare2;
        static const HrtimTimerOutputSetSource kTimerCEvent6TimerDCompare4;
        static const HrtimTimerOutputSetSource kTimerCEvent7TimerECompare3;
        static const HrtimTimerOutputSetSource kTimerCEvent8TimerECompare4;
        static const HrtimTimerOutputSetSource kTimerCEvent9TimerFCompare2;
        static const HrtimTimerOutputSetSource kTimerDEvent1TimerACompare1;
        static const HrtimTimerOutputSetSource kTimerDEvent2TimerACompare4;
        static const HrtimTimerOutputSetSource kTimerDEvent3TimerBCompare2;
        static const HrtimTimerOutputSetSource kTimerDEvent4TimerBCompare4;
        static const HrtimTimerOutputSetSource kTimerDEvent5TimerCCompare4;
        static const HrtimTimerOutputSetSource kTimerDEvent6TimerECompare1;
        static const HrtimTimerOutputSetSource kTimerDEvent7TimerECompare4;
        static const HrtimTimerOutputSetSource kTimerDEvent8TimerFCompare1;
        static const HrtimTimerOutputSetSource kTimerDEvent9TimerFCompare3;
        static const HrtimTimerOutputSetSource kTimerEEvent1TimerACompare4;
        static const HrtimTimerOutputSetSource kTimerEEvent2TimerBCompare3;
        static const HrtimTimerOutputSetSource kTimerEEvent3TimerBCompare4;
        static const HrtimTimerOutputSetSource kTimerEEvent4TimerCCompare1;
        static const HrtimTimerOutputSetSource kTimerEEvent5TimerCCompare2;
        static const HrtimTimerOutputSetSource kTimerEEvent6TimerDCompare1;
        static const HrtimTimerOutputSetSource kTimerEEvent7TimerDCompare2;
        static const HrtimTimerOutputSetSource kTimerEEvent8TimerFCompare3;
        static const HrtimTimerOutputSetSource kTimerEEvent9TimerFCompare4;
        static const HrtimTimerOutputSetSource kTimerFEvent1TimerACompare3;
        static const HrtimTimerOutputSetSource kTimerFEvent2TimerBCompare1;
        static const HrtimTimerOutputSetSource kTimerFEvent3TimerBCompare4;
        static const HrtimTimerOutputSetSource kTimerFEvent4TimerCCompare1;
        static const HrtimTimerOutputSetSource kTimerFEvent5TimerCCompare4;
        static const HrtimTimerOutputSetSource kTimerFEvent6TimerDCompare3;
        static const HrtimTimerOutputSetSource kTimerFEvent7TimerDCompare4;
        static const HrtimTimerOutputSetSource kTimerFEvent8TimerECompare2;
        static const HrtimTimerOutputSetSource kTimerFEvent9TimerECompare3;
        static const HrtimTimerOutputSetSource kExternalEvent1;
        static const HrtimTimerOutputSetSource kExternalEvent2;
        static const HrtimTimerOutputSetSource kExternalEvent3;
        static const HrtimTimerOutputSetSource kExternalEvent4;
        static const HrtimTimerOutputSetSource kExternalEvent5;
        static const HrtimTimerOutputSetSource kExternalEvent6;
        static const HrtimTimerOutputSetSource kExternalEvent7;
        static const HrtimTimerOutputSetSource kExternalEvent8;
        static const HrtimTimerOutputSetSource kExternalEvent9;
        static const HrtimTimerOutputSetSource kExternalEvent10;
        static const HrtimTimerOutputSetSource kUpdate;
    };

    inline constexpr HrtimTimerOutputSetSource HrtimTimerOutputSetSource::kNone = HrtimTimerOutputSetSource::from_ll<LL_HRTIM_OUTPUTSET_NONE>();
    inline constexpr HrtimTimerOutputSetSource HrtimTimerOutputSetSource::kResync = HrtimTimerOutputSetSource::from_ll<LL_HRTIM_OUTPUTSET_RESYNC>();
    inline constexpr HrtimTimerOutputSetSource HrtimTimerOutputSetSource::kTimerPeriod = HrtimTimerOutputSetSource::from_ll<LL_HRTIM_OUTPUTSET_TIMPER>();
    inline constexpr HrtimTimerOutputSetSource HrtimTimerOutputSetSource::kTimerCompare1 = HrtimTimerOutputSetSource::from_ll<LL_HRTIM_OUTPUTSET_TIMCMP1>();
    inline constexpr HrtimTimerOutputSetSource HrtimTimerOutputSetSource::kTimerCompare2 = HrtimTimerOutputSetSource::from_ll<LL_HRTIM_OUTPUTSET_TIMCMP2>();
    inline constexpr HrtimTimerOutputSetSource HrtimTimerOutputSetSource::kTimerCompare3 = HrtimTimerOutputSetSource::from_ll<LL_HRTIM_OUTPUTSET_TIMCMP3>();
    inline constexpr HrtimTimerOutputSetSource HrtimTimerOutputSetSource::kTimerCompare4 = HrtimTimerOutputSetSource::from_ll<LL_HRTIM_OUTPUTSET_TIMCMP4>();
    inline constexpr HrtimTimerOutputSetSource HrtimTimerOutputSetSource::kMasterPeriod = HrtimTimerOutputSetSource::from_ll<LL_HRTIM_OUTPUTSET_MASTERPER>();
    inline constexpr HrtimTimerOutputSetSource HrtimTimerOutputSetSource::kMasterCompare1 = HrtimTimerOutputSetSource::from_ll<LL_HRTIM_OUTPUTSET_MASTERCMP1>();
    inline constexpr HrtimTimerOutputSetSource HrtimTimerOutputSetSource::kMasterCompare2 = HrtimTimerOutputSetSource::from_ll<LL_HRTIM_OUTPUTSET_MASTERCMP2>();
    inline constexpr HrtimTimerOutputSetSource HrtimTimerOutputSetSource::kMasterCompare3 = HrtimTimerOutputSetSource::from_ll<LL_HRTIM_OUTPUTSET_MASTERCMP3>();
    inline constexpr HrtimTimerOutputSetSource HrtimTimerOutputSetSource::kMasterCompare4 = HrtimTimerOutputSetSource::from_ll<LL_HRTIM_OUTPUTSET_MASTERCMP4>();
    inline constexpr HrtimTimerOutputSetSource HrtimTimerOutputSetSource::kTimerAEvent1TimerBCompare1 = HrtimTimerOutputSetSource::from_ll<LL_HRTIM_OUTPUTSET_TIMAEV1_TIMBCMP1>();
    inline constexpr HrtimTimerOutputSetSource HrtimTimerOutputSetSource::kTimerAEvent2TimerBCompare2 = HrtimTimerOutputSetSource::from_ll<LL_HRTIM_OUTPUTSET_TIMAEV2_TIMBCMP2>();
    inline constexpr HrtimTimerOutputSetSource HrtimTimerOutputSetSource::kTimerAEvent3TimerCCompare2 = HrtimTimerOutputSetSource::from_ll<LL_HRTIM_OUTPUTSET_TIMAEV3_TIMCCMP2>();
    inline constexpr HrtimTimerOutputSetSource HrtimTimerOutputSetSource::kTimerAEvent4TimerCCompare3 = HrtimTimerOutputSetSource::from_ll<LL_HRTIM_OUTPUTSET_TIMAEV4_TIMCCMP3>();
    inline constexpr HrtimTimerOutputSetSource HrtimTimerOutputSetSource::kTimerAEvent5TimerDCompare1 = HrtimTimerOutputSetSource::from_ll<LL_HRTIM_OUTPUTSET_TIMAEV5_TIMDCMP1>();
    inline constexpr HrtimTimerOutputSetSource HrtimTimerOutputSetSource::kTimerAEvent6TimerDCompare2 = HrtimTimerOutputSetSource::from_ll<LL_HRTIM_OUTPUTSET_TIMAEV6_TIMDCMP2>();
    inline constexpr HrtimTimerOutputSetSource HrtimTimerOutputSetSource::kTimerAEvent7TimerECompare3 = HrtimTimerOutputSetSource::from_ll<LL_HRTIM_OUTPUTSET_TIMAEV7_TIMECMP3>();
    inline constexpr HrtimTimerOutputSetSource HrtimTimerOutputSetSource::kTimerAEvent8TimerECompare4 = HrtimTimerOutputSetSource::from_ll<LL_HRTIM_OUTPUTSET_TIMAEV8_TIMECMP4>();
    inline constexpr HrtimTimerOutputSetSource HrtimTimerOutputSetSource::kTimerAEvent9TimerFCompare4 = HrtimTimerOutputSetSource::from_ll<LL_HRTIM_OUTPUTSET_TIMAEV9_TIMFCMP4>();
    inline constexpr HrtimTimerOutputSetSource HrtimTimerOutputSetSource::kTimerBEvent1TimerACompare1 = HrtimTimerOutputSetSource::from_ll<LL_HRTIM_OUTPUTSET_TIMBEV1_TIMACMP1>();
    inline constexpr HrtimTimerOutputSetSource HrtimTimerOutputSetSource::kTimerBEvent2TimerACompare2 = HrtimTimerOutputSetSource::from_ll<LL_HRTIM_OUTPUTSET_TIMBEV2_TIMACMP2>();
    inline constexpr HrtimTimerOutputSetSource HrtimTimerOutputSetSource::kTimerBEvent3TimerCCompare3 = HrtimTimerOutputSetSource::from_ll<LL_HRTIM_OUTPUTSET_TIMBEV3_TIMCCMP3>();
    inline constexpr HrtimTimerOutputSetSource HrtimTimerOutputSetSource::kTimerBEvent4TimerCCompare4 = HrtimTimerOutputSetSource::from_ll<LL_HRTIM_OUTPUTSET_TIMBEV4_TIMCCMP4>();
    inline constexpr HrtimTimerOutputSetSource HrtimTimerOutputSetSource::kTimerBEvent5TimerDCompare3 = HrtimTimerOutputSetSource::from_ll<LL_HRTIM_OUTPUTSET_TIMBEV5_TIMDCMP3>();
    inline constexpr HrtimTimerOutputSetSource HrtimTimerOutputSetSource::kTimerBEvent6TimerDCompare4 = HrtimTimerOutputSetSource::from_ll<LL_HRTIM_OUTPUTSET_TIMBEV6_TIMDCMP4>();
    inline constexpr HrtimTimerOutputSetSource HrtimTimerOutputSetSource::kTimerBEvent7TimerECompare1 = HrtimTimerOutputSetSource::from_ll<LL_HRTIM_OUTPUTSET_TIMBEV7_TIMECMP1>();
    inline constexpr HrtimTimerOutputSetSource HrtimTimerOutputSetSource::kTimerBEvent8TimerECompare2 = HrtimTimerOutputSetSource::from_ll<LL_HRTIM_OUTPUTSET_TIMBEV8_TIMECMP2>();
    inline constexpr HrtimTimerOutputSetSource HrtimTimerOutputSetSource::kTimerBEvent9TimerFCompare3 = HrtimTimerOutputSetSource::from_ll<LL_HRTIM_OUTPUTSET_TIMBEV9_TIMFCMP3>();
    inline constexpr HrtimTimerOutputSetSource HrtimTimerOutputSetSource::kTimerCEvent1TimerACompare2 = HrtimTimerOutputSetSource::from_ll<LL_HRTIM_OUTPUTSET_TIMCEV1_TIMACMP2>();
    inline constexpr HrtimTimerOutputSetSource HrtimTimerOutputSetSource::kTimerCEvent2TimerACompare3 = HrtimTimerOutputSetSource::from_ll<LL_HRTIM_OUTPUTSET_TIMCEV2_TIMACMP3>();
    inline constexpr HrtimTimerOutputSetSource HrtimTimerOutputSetSource::kTimerCEvent3TimerBCompare2 = HrtimTimerOutputSetSource::from_ll<LL_HRTIM_OUTPUTSET_TIMCEV3_TIMBCMP2>();
    inline constexpr HrtimTimerOutputSetSource HrtimTimerOutputSetSource::kTimerCEvent4TimerBCompare3 = HrtimTimerOutputSetSource::from_ll<LL_HRTIM_OUTPUTSET_TIMCEV4_TIMBCMP3>();
    inline constexpr HrtimTimerOutputSetSource HrtimTimerOutputSetSource::kTimerCEvent5TimerDCompare2 = HrtimTimerOutputSetSource::from_ll<LL_HRTIM_OUTPUTSET_TIMCEV5_TIMDCMP2>();
    inline constexpr HrtimTimerOutputSetSource HrtimTimerOutputSetSource::kTimerCEvent6TimerDCompare4 = HrtimTimerOutputSetSource::from_ll<LL_HRTIM_OUTPUTSET_TIMCEV6_TIMDCMP4>();
    inline constexpr HrtimTimerOutputSetSource HrtimTimerOutputSetSource::kTimerCEvent7TimerECompare3 = HrtimTimerOutputSetSource::from_ll<LL_HRTIM_OUTPUTSET_TIMCEV7_TIMECMP3>();
    inline constexpr HrtimTimerOutputSetSource HrtimTimerOutputSetSource::kTimerCEvent8TimerECompare4 = HrtimTimerOutputSetSource::from_ll<LL_HRTIM_OUTPUTSET_TIMCEV8_TIMECMP4>();
    inline constexpr HrtimTimerOutputSetSource HrtimTimerOutputSetSource::kTimerCEvent9TimerFCompare2 = HrtimTimerOutputSetSource::from_ll<LL_HRTIM_OUTPUTSET_TIMCEV9_TIMFCMP2>();
    inline constexpr HrtimTimerOutputSetSource HrtimTimerOutputSetSource::kTimerDEvent1TimerACompare1 = HrtimTimerOutputSetSource::from_ll<LL_HRTIM_OUTPUTSET_TIMDEV1_TIMACMP1>();
    inline constexpr HrtimTimerOutputSetSource HrtimTimerOutputSetSource::kTimerDEvent2TimerACompare4 = HrtimTimerOutputSetSource::from_ll<LL_HRTIM_OUTPUTSET_TIMDEV2_TIMACMP4>();
    inline constexpr HrtimTimerOutputSetSource HrtimTimerOutputSetSource::kTimerDEvent3TimerBCompare2 = HrtimTimerOutputSetSource::from_ll<LL_HRTIM_OUTPUTSET_TIMDEV3_TIMBCMP2>();
    inline constexpr HrtimTimerOutputSetSource HrtimTimerOutputSetSource::kTimerDEvent4TimerBCompare4 = HrtimTimerOutputSetSource::from_ll<LL_HRTIM_OUTPUTSET_TIMDEV4_TIMBCMP4>();
    inline constexpr HrtimTimerOutputSetSource HrtimTimerOutputSetSource::kTimerDEvent5TimerCCompare4 = HrtimTimerOutputSetSource::from_ll<LL_HRTIM_OUTPUTSET_TIMDEV5_TIMCCMP4>();
    inline constexpr HrtimTimerOutputSetSource HrtimTimerOutputSetSource::kTimerDEvent6TimerECompare1 = HrtimTimerOutputSetSource::from_ll<LL_HRTIM_OUTPUTSET_TIMDEV6_TIMECMP1>();
    inline constexpr HrtimTimerOutputSetSource HrtimTimerOutputSetSource::kTimerDEvent7TimerECompare4 = HrtimTimerOutputSetSource::from_ll<LL_HRTIM_OUTPUTSET_TIMDEV7_TIMECMP4>();
    inline constexpr HrtimTimerOutputSetSource HrtimTimerOutputSetSource::kTimerDEvent8TimerFCompare1 = HrtimTimerOutputSetSource::from_ll<LL_HRTIM_OUTPUTSET_TIMDEV8_TIMFCMP1>();
    inline constexpr HrtimTimerOutputSetSource HrtimTimerOutputSetSource::kTimerDEvent9TimerFCompare3 = HrtimTimerOutputSetSource::from_ll<LL_HRTIM_OUTPUTSET_TIMDEV9_TIMFCMP3>();
    inline constexpr HrtimTimerOutputSetSource HrtimTimerOutputSetSource::kTimerEEvent1TimerACompare4 = HrtimTimerOutputSetSource::from_ll<LL_HRTIM_OUTPUTSET_TIMEEV1_TIMACMP4>();
    inline constexpr HrtimTimerOutputSetSource HrtimTimerOutputSetSource::kTimerEEvent2TimerBCompare3 = HrtimTimerOutputSetSource::from_ll<LL_HRTIM_OUTPUTSET_TIMEEV2_TIMBCMP3>();
    inline constexpr HrtimTimerOutputSetSource HrtimTimerOutputSetSource::kTimerEEvent3TimerBCompare4 = HrtimTimerOutputSetSource::from_ll<LL_HRTIM_OUTPUTSET_TIMEEV3_TIMBCMP4>();
    inline constexpr HrtimTimerOutputSetSource HrtimTimerOutputSetSource::kTimerEEvent4TimerCCompare1 = HrtimTimerOutputSetSource::from_ll<LL_HRTIM_OUTPUTSET_TIMEEV4_TIMCCMP1>();
    inline constexpr HrtimTimerOutputSetSource HrtimTimerOutputSetSource::kTimerEEvent5TimerCCompare2 = HrtimTimerOutputSetSource::from_ll<LL_HRTIM_OUTPUTSET_TIMEEV5_TIMCCMP2>();
    inline constexpr HrtimTimerOutputSetSource HrtimTimerOutputSetSource::kTimerEEvent6TimerDCompare1 = HrtimTimerOutputSetSource::from_ll<LL_HRTIM_OUTPUTSET_TIMEEV6_TIMDCMP1>();
    inline constexpr HrtimTimerOutputSetSource HrtimTimerOutputSetSource::kTimerEEvent7TimerDCompare2 = HrtimTimerOutputSetSource::from_ll<LL_HRTIM_OUTPUTSET_TIMEEV7_TIMDCMP2>();
    inline constexpr HrtimTimerOutputSetSource HrtimTimerOutputSetSource::kTimerEEvent8TimerFCompare3 = HrtimTimerOutputSetSource::from_ll<LL_HRTIM_OUTPUTSET_TIMEEV8_TIMFCMP3>();
    inline constexpr HrtimTimerOutputSetSource HrtimTimerOutputSetSource::kTimerEEvent9TimerFCompare4 = HrtimTimerOutputSetSource::from_ll<LL_HRTIM_OUTPUTSET_TIMEEV9_TIMFCMP4>();
    inline constexpr HrtimTimerOutputSetSource HrtimTimerOutputSetSource::kTimerFEvent1TimerACompare3 = HrtimTimerOutputSetSource::from_ll<LL_HRTIM_OUTPUTSET_TIMFEV1_TIMACMP3>();
    inline constexpr HrtimTimerOutputSetSource HrtimTimerOutputSetSource::kTimerFEvent2TimerBCompare1 = HrtimTimerOutputSetSource::from_ll<LL_HRTIM_OUTPUTSET_TIMFEV2_TIMBCMP1>();
    inline constexpr HrtimTimerOutputSetSource HrtimTimerOutputSetSource::kTimerFEvent3TimerBCompare4 = HrtimTimerOutputSetSource::from_ll<LL_HRTIM_OUTPUTSET_TIMFEV3_TIMBCMP4>();
    inline constexpr HrtimTimerOutputSetSource HrtimTimerOutputSetSource::kTimerFEvent4TimerCCompare1 = HrtimTimerOutputSetSource::from_ll<LL_HRTIM_OUTPUTSET_TIMFEV4_TIMCCMP1>();
    inline constexpr HrtimTimerOutputSetSource HrtimTimerOutputSetSource::kTimerFEvent5TimerCCompare4 = HrtimTimerOutputSetSource::from_ll<LL_HRTIM_OUTPUTSET_TIMFEV5_TIMCCMP4>();
    inline constexpr HrtimTimerOutputSetSource HrtimTimerOutputSetSource::kTimerFEvent6TimerDCompare3 = HrtimTimerOutputSetSource::from_ll<LL_HRTIM_OUTPUTSET_TIMFEV6_TIMDCMP3>();
    inline constexpr HrtimTimerOutputSetSource HrtimTimerOutputSetSource::kTimerFEvent7TimerDCompare4 = HrtimTimerOutputSetSource::from_ll<LL_HRTIM_OUTPUTSET_TIMFEV7_TIMDCMP4>();
    inline constexpr HrtimTimerOutputSetSource HrtimTimerOutputSetSource::kTimerFEvent8TimerECompare2 = HrtimTimerOutputSetSource::from_ll<LL_HRTIM_OUTPUTSET_TIMFEV8_TIMECMP2>();
    inline constexpr HrtimTimerOutputSetSource HrtimTimerOutputSetSource::kTimerFEvent9TimerECompare3 = HrtimTimerOutputSetSource::from_ll<LL_HRTIM_OUTPUTSET_TIMFEV9_TIMECMP3>();
    inline constexpr HrtimTimerOutputSetSource HrtimTimerOutputSetSource::kExternalEvent1 = HrtimTimerOutputSetSource::from_ll<LL_HRTIM_OUTPUTSET_EEV_1>();
    inline constexpr HrtimTimerOutputSetSource HrtimTimerOutputSetSource::kExternalEvent2 = HrtimTimerOutputSetSource::from_ll<LL_HRTIM_OUTPUTSET_EEV_2>();
    inline constexpr HrtimTimerOutputSetSource HrtimTimerOutputSetSource::kExternalEvent3 = HrtimTimerOutputSetSource::from_ll<LL_HRTIM_OUTPUTSET_EEV_3>();
    inline constexpr HrtimTimerOutputSetSource HrtimTimerOutputSetSource::kExternalEvent4 = HrtimTimerOutputSetSource::from_ll<LL_HRTIM_OUTPUTSET_EEV_4>();
    inline constexpr HrtimTimerOutputSetSource HrtimTimerOutputSetSource::kExternalEvent5 = HrtimTimerOutputSetSource::from_ll<LL_HRTIM_OUTPUTSET_EEV_5>();
    inline constexpr HrtimTimerOutputSetSource HrtimTimerOutputSetSource::kExternalEvent6 = HrtimTimerOutputSetSource::from_ll<LL_HRTIM_OUTPUTSET_EEV_6>();
    inline constexpr HrtimTimerOutputSetSource HrtimTimerOutputSetSource::kExternalEvent7 = HrtimTimerOutputSetSource::from_ll<LL_HRTIM_OUTPUTSET_EEV_7>();
    inline constexpr HrtimTimerOutputSetSource HrtimTimerOutputSetSource::kExternalEvent8 = HrtimTimerOutputSetSource::from_ll<LL_HRTIM_OUTPUTSET_EEV_8>();
    inline constexpr HrtimTimerOutputSetSource HrtimTimerOutputSetSource::kExternalEvent9 = HrtimTimerOutputSetSource::from_ll<LL_HRTIM_OUTPUTSET_EEV_9>();
    inline constexpr HrtimTimerOutputSetSource HrtimTimerOutputSetSource::kExternalEvent10 = HrtimTimerOutputSetSource::from_ll<LL_HRTIM_OUTPUTSET_EEV_10>();
    inline constexpr HrtimTimerOutputSetSource HrtimTimerOutputSetSource::kUpdate = HrtimTimerOutputSetSource::from_ll<LL_HRTIM_OUTPUTSET_UPDATE>();


    struct HrtimTimerOutputResetSource : public LLDriverEnumValue<HrtimTimerOutputResetSource>
    {
        static const HrtimTimerOutputResetSource kNone;
        static const HrtimTimerOutputResetSource kResync;
        static const HrtimTimerOutputResetSource kTimerPeriod;
        static const HrtimTimerOutputResetSource kTimerCompare1;
        static const HrtimTimerOutputResetSource kTimerCompare2;
        static const HrtimTimerOutputResetSource kTimerCompare3;
        static const HrtimTimerOutputResetSource kTimerCompare4;
        static const HrtimTimerOutputResetSource kMasterPeriod;
        static const HrtimTimerOutputResetSource kMasterCompare1;
        static const HrtimTimerOutputResetSource kMasterCompare2;
        static const HrtimTimerOutputResetSource kMasterCompare3;
        static const HrtimTimerOutputResetSource kMasterCompare4;
        static const HrtimTimerOutputResetSource kTimerAEvent1TimerBCompare1;
        static const HrtimTimerOutputResetSource kTimerAEvent2TimerBCompare2;
        static const HrtimTimerOutputResetSource kTimerAEvent3TimerCCompare2;
        static const HrtimTimerOutputResetSource kTimerAEvent4TimerCCompare3;
        static const HrtimTimerOutputResetSource kTimerAEvent5TimerDCompare1;
        static const HrtimTimerOutputResetSource kTimerAEvent6TimerDCompare2;
        static const HrtimTimerOutputResetSource kTimerAEvent7TimerECompare3;
        static const HrtimTimerOutputResetSource kTimerAEvent8TimerECompare4;
        static const HrtimTimerOutputResetSource kTimerAEvent9TimerFCompare4;
        static const HrtimTimerOutputResetSource kTimerBEvent1TimerACompare1;
        static const HrtimTimerOutputResetSource kTimerBEvent2TimerACompare2;
        static const HrtimTimerOutputResetSource kTimerBEvent3TimerCCompare3;
        static const HrtimTimerOutputResetSource kTimerBEvent4TimerCCompare4;
        static const HrtimTimerOutputResetSource kTimerBEvent5TimerDCompare3;
        static const HrtimTimerOutputResetSource kTimerBEvent6TimerDCompare4;
        static const HrtimTimerOutputResetSource kTimerBEvent7TimerECompare1;
        static const HrtimTimerOutputResetSource kTimerBEvent8TimerECompare2;
        static const HrtimTimerOutputResetSource kTimerBEvent9TimerFCompare3;
        static const HrtimTimerOutputResetSource kTimerCEvent1TimerACompare2;
        static const HrtimTimerOutputResetSource kTimerCEvent2TimerACompare3;
        static const HrtimTimerOutputResetSource kTimerCEvent3TimerBCompare2;
        static const HrtimTimerOutputResetSource kTimerCEvent4TimerBCompare3;
        static const HrtimTimerOutputResetSource kTimerCEvent5TimerDCompare2;
        static const HrtimTimerOutputResetSource kTimerCEvent6TimerDCompare4;
        static const HrtimTimerOutputResetSource kTimerCEvent7TimerECompare3;
        static const HrtimTimerOutputResetSource kTimerCEvent8TimerECompare4;
        static const HrtimTimerOutputResetSource kTimerCEvent9TimerFCompare2;
        static const HrtimTimerOutputResetSource kTimerDEvent1TimerACompare1;
        static const HrtimTimerOutputResetSource kTimerDEvent2TimerACompare4;
        static const HrtimTimerOutputResetSource kTimerDEvent3TimerBCompare2;
        static const HrtimTimerOutputResetSource kTimerDEvent4TimerBCompare4;
        static const HrtimTimerOutputResetSource kTimerDEvent5TimerCCompare4;
        static const HrtimTimerOutputResetSource kTimerDEvent6TimerECompare1;
        static const HrtimTimerOutputResetSource kTimerDEvent7TimerECompare4;
        static const HrtimTimerOutputResetSource kTimerDEvent8TimerFCompare1;
        static const HrtimTimerOutputResetSource kTimerDEvent9TimerFCompare3;
        static const HrtimTimerOutputResetSource kTimerEEvent1TimerACompare4;
        static const HrtimTimerOutputResetSource kTimerEEvent2TimerBCompare3;
        static const HrtimTimerOutputResetSource kTimerEEvent3TimerBCompare4;
        static const HrtimTimerOutputResetSource kTimerEEvent4TimerCCompare1;
        static const HrtimTimerOutputResetSource kTimerEEvent5TimerCCompare2;
        static const HrtimTimerOutputResetSource kTimerEEvent6TimerDCompare1;
        static const HrtimTimerOutputResetSource kTimerEEvent7TimerDCompare2;
        static const HrtimTimerOutputResetSource kTimerEEvent8TimerFCompare3;
        static const HrtimTimerOutputResetSource kTimerEEvent9TimerFCompare4;
        static const HrtimTimerOutputResetSource kTimerFEvent1TimerACompare3;
        static const HrtimTimerOutputResetSource kTimerFEvent2TimerBCompare1;
        static const HrtimTimerOutputResetSource kTimerFEvent3TimerBCompare4;
        static const HrtimTimerOutputResetSource kTimerFEvent4TimerCCompare1;
        static const HrtimTimerOutputResetSource kTimerFEvent5TimerCCompare4;
        static const HrtimTimerOutputResetSource kTimerFEvent6TimerDCompare3;
        static const HrtimTimerOutputResetSource kTimerFEvent7TimerDCompare4;
        static const HrtimTimerOutputResetSource kTimerFEvent8TimerECompare2;
        static const HrtimTimerOutputResetSource kTimerFEvent9TimerECompare3;
        static const HrtimTimerOutputResetSource kExternalEvent1;
        static const HrtimTimerOutputResetSource kExternalEvent2;
        static const HrtimTimerOutputResetSource kExternalEvent3;
        static const HrtimTimerOutputResetSource kExternalEvent4;
        static const HrtimTimerOutputResetSource kExternalEvent5;
        static const HrtimTimerOutputResetSource kExternalEvent6;
        static const HrtimTimerOutputResetSource kExternalEvent7;
        static const HrtimTimerOutputResetSource kExternalEvent8;
        static const HrtimTimerOutputResetSource kExternalEvent9;
        static const HrtimTimerOutputResetSource kExternalEvent10;
        static const HrtimTimerOutputResetSource kUpdate;
    };

    inline constexpr HrtimTimerOutputResetSource HrtimTimerOutputResetSource::kNone = HrtimTimerOutputResetSource::from_ll<LL_HRTIM_OUTPUTRESET_NONE>();
    inline constexpr HrtimTimerOutputResetSource HrtimTimerOutputResetSource::kResync = HrtimTimerOutputResetSource::from_ll<LL_HRTIM_OUTPUTRESET_RESYNC>();
    inline constexpr HrtimTimerOutputResetSource HrtimTimerOutputResetSource::kTimerPeriod = HrtimTimerOutputResetSource::from_ll<LL_HRTIM_OUTPUTRESET_TIMPER>();
    inline constexpr HrtimTimerOutputResetSource HrtimTimerOutputResetSource::kTimerCompare1 = HrtimTimerOutputResetSource::from_ll<LL_HRTIM_OUTPUTRESET_TIMCMP1>();
    inline constexpr HrtimTimerOutputResetSource HrtimTimerOutputResetSource::kTimerCompare2 = HrtimTimerOutputResetSource::from_ll<LL_HRTIM_OUTPUTRESET_TIMCMP2>();
    inline constexpr HrtimTimerOutputResetSource HrtimTimerOutputResetSource::kTimerCompare3 = HrtimTimerOutputResetSource::from_ll<LL_HRTIM_OUTPUTRESET_TIMCMP3>();
    inline constexpr HrtimTimerOutputResetSource HrtimTimerOutputResetSource::kTimerCompare4 = HrtimTimerOutputResetSource::from_ll<LL_HRTIM_OUTPUTRESET_TIMCMP4>();
    inline constexpr HrtimTimerOutputResetSource HrtimTimerOutputResetSource::kMasterPeriod = HrtimTimerOutputResetSource::from_ll<LL_HRTIM_OUTPUTRESET_MASTERPER>();
    inline constexpr HrtimTimerOutputResetSource HrtimTimerOutputResetSource::kMasterCompare1 = HrtimTimerOutputResetSource::from_ll<LL_HRTIM_OUTPUTRESET_MASTERCMP1>();
    inline constexpr HrtimTimerOutputResetSource HrtimTimerOutputResetSource::kMasterCompare2 = HrtimTimerOutputResetSource::from_ll<LL_HRTIM_OUTPUTRESET_MASTERCMP2>();
    inline constexpr HrtimTimerOutputResetSource HrtimTimerOutputResetSource::kMasterCompare3 = HrtimTimerOutputResetSource::from_ll<LL_HRTIM_OUTPUTRESET_MASTERCMP3>();
    inline constexpr HrtimTimerOutputResetSource HrtimTimerOutputResetSource::kMasterCompare4 = HrtimTimerOutputResetSource::from_ll<LL_HRTIM_OUTPUTRESET_MASTERCMP4>();
    inline constexpr HrtimTimerOutputResetSource HrtimTimerOutputResetSource::kTimerAEvent1TimerBCompare1 = HrtimTimerOutputResetSource::from_ll<LL_HRTIM_OUTPUTRESET_TIMAEV1_TIMBCMP1>();
    inline constexpr HrtimTimerOutputResetSource HrtimTimerOutputResetSource::kTimerAEvent2TimerBCompare2 = HrtimTimerOutputResetSource::from_ll<LL_HRTIM_OUTPUTRESET_TIMAEV2_TIMBCMP2>();
    inline constexpr HrtimTimerOutputResetSource HrtimTimerOutputResetSource::kTimerAEvent3TimerCCompare2 = HrtimTimerOutputResetSource::from_ll<LL_HRTIM_OUTPUTRESET_TIMAEV3_TIMCCMP2>();
    inline constexpr HrtimTimerOutputResetSource HrtimTimerOutputResetSource::kTimerAEvent4TimerCCompare3 = HrtimTimerOutputResetSource::from_ll<LL_HRTIM_OUTPUTRESET_TIMAEV4_TIMCCMP3>();
    inline constexpr HrtimTimerOutputResetSource HrtimTimerOutputResetSource::kTimerAEvent5TimerDCompare1 = HrtimTimerOutputResetSource::from_ll<LL_HRTIM_OUTPUTRESET_TIMAEV5_TIMDCMP1>();
    inline constexpr HrtimTimerOutputResetSource HrtimTimerOutputResetSource::kTimerAEvent6TimerDCompare2 = HrtimTimerOutputResetSource::from_ll<LL_HRTIM_OUTPUTRESET_TIMAEV6_TIMDCMP2>();
    inline constexpr HrtimTimerOutputResetSource HrtimTimerOutputResetSource::kTimerAEvent7TimerECompare3 = HrtimTimerOutputResetSource::from_ll<LL_HRTIM_OUTPUTRESET_TIMAEV7_TIMECMP3>();
    inline constexpr HrtimTimerOutputResetSource HrtimTimerOutputResetSource::kTimerAEvent8TimerECompare4 = HrtimTimerOutputResetSource::from_ll<LL_HRTIM_OUTPUTRESET_TIMAEV8_TIMECMP4>();
    inline constexpr HrtimTimerOutputResetSource HrtimTimerOutputResetSource::kTimerAEvent9TimerFCompare4 = HrtimTimerOutputResetSource::from_ll<LL_HRTIM_OUTPUTRESET_TIMAEV9_TIMFCMP4>();
    inline constexpr HrtimTimerOutputResetSource HrtimTimerOutputResetSource::kTimerBEvent1TimerACompare1 = HrtimTimerOutputResetSource::from_ll<LL_HRTIM_OUTPUTRESET_TIMBEV1_TIMACMP1>();
    inline constexpr HrtimTimerOutputResetSource HrtimTimerOutputResetSource::kTimerBEvent2TimerACompare2 = HrtimTimerOutputResetSource::from_ll<LL_HRTIM_OUTPUTRESET_TIMBEV2_TIMACMP2>();
    inline constexpr HrtimTimerOutputResetSource HrtimTimerOutputResetSource::kTimerBEvent3TimerCCompare3 = HrtimTimerOutputResetSource::from_ll<LL_HRTIM_OUTPUTRESET_TIMBEV3_TIMCCMP3>();
    inline constexpr HrtimTimerOutputResetSource HrtimTimerOutputResetSource::kTimerBEvent4TimerCCompare4 = HrtimTimerOutputResetSource::from_ll<LL_HRTIM_OUTPUTRESET_TIMBEV4_TIMCCMP4>();
    inline constexpr HrtimTimerOutputResetSource HrtimTimerOutputResetSource::kTimerBEvent5TimerDCompare3 = HrtimTimerOutputResetSource::from_ll<LL_HRTIM_OUTPUTRESET_TIMBEV5_TIMDCMP3>();
    inline constexpr HrtimTimerOutputResetSource HrtimTimerOutputResetSource::kTimerBEvent6TimerDCompare4 = HrtimTimerOutputResetSource::from_ll<LL_HRTIM_OUTPUTRESET_TIMBEV6_TIMDCMP4>();
    inline constexpr HrtimTimerOutputResetSource HrtimTimerOutputResetSource::kTimerBEvent7TimerECompare1 = HrtimTimerOutputResetSource::from_ll<LL_HRTIM_OUTPUTRESET_TIMBEV7_TIMECMP1>();
    inline constexpr HrtimTimerOutputResetSource HrtimTimerOutputResetSource::kTimerBEvent8TimerECompare2 = HrtimTimerOutputResetSource::from_ll<LL_HRTIM_OUTPUTRESET_TIMBEV8_TIMECMP2>();
    inline constexpr HrtimTimerOutputResetSource HrtimTimerOutputResetSource::kTimerBEvent9TimerFCompare3 = HrtimTimerOutputResetSource::from_ll<LL_HRTIM_OUTPUTRESET_TIMBEV9_TIMFCMP3>();
    inline constexpr HrtimTimerOutputResetSource HrtimTimerOutputResetSource::kTimerCEvent1TimerACompare2 = HrtimTimerOutputResetSource::from_ll<LL_HRTIM_OUTPUTRESET_TIMCEV1_TIMACMP2>();
    inline constexpr HrtimTimerOutputResetSource HrtimTimerOutputResetSource::kTimerCEvent2TimerACompare3 = HrtimTimerOutputResetSource::from_ll<LL_HRTIM_OUTPUTRESET_TIMCEV2_TIMACMP3>();
    inline constexpr HrtimTimerOutputResetSource HrtimTimerOutputResetSource::kTimerCEvent3TimerBCompare2 = HrtimTimerOutputResetSource::from_ll<LL_HRTIM_OUTPUTRESET_TIMCEV3_TIMBCMP2>();
    inline constexpr HrtimTimerOutputResetSource HrtimTimerOutputResetSource::kTimerCEvent4TimerBCompare3 = HrtimTimerOutputResetSource::from_ll<LL_HRTIM_OUTPUTRESET_TIMCEV4_TIMBCMP3>();
    inline constexpr HrtimTimerOutputResetSource HrtimTimerOutputResetSource::kTimerCEvent5TimerDCompare2 = HrtimTimerOutputResetSource::from_ll<LL_HRTIM_OUTPUTRESET_TIMCEV5_TIMDCMP2>();
    inline constexpr HrtimTimerOutputResetSource HrtimTimerOutputResetSource::kTimerCEvent6TimerDCompare4 = HrtimTimerOutputResetSource::from_ll<LL_HRTIM_OUTPUTRESET_TIMCEV6_TIMDCMP4>();
    inline constexpr HrtimTimerOutputResetSource HrtimTimerOutputResetSource::kTimerCEvent7TimerECompare3 = HrtimTimerOutputResetSource::from_ll<LL_HRTIM_OUTPUTRESET_TIMCEV7_TIMECMP3>();
    inline constexpr HrtimTimerOutputResetSource HrtimTimerOutputResetSource::kTimerCEvent8TimerECompare4 = HrtimTimerOutputResetSource::from_ll<LL_HRTIM_OUTPUTRESET_TIMCEV8_TIMECMP4>();
    inline constexpr HrtimTimerOutputResetSource HrtimTimerOutputResetSource::kTimerCEvent9TimerFCompare2 = HrtimTimerOutputResetSource::from_ll<LL_HRTIM_OUTPUTRESET_TIMCEV9_TIMFCMP2>();
    inline constexpr HrtimTimerOutputResetSource HrtimTimerOutputResetSource::kTimerDEvent1TimerACompare1 = HrtimTimerOutputResetSource::from_ll<LL_HRTIM_OUTPUTRESET_TIMDEV1_TIMACMP1>();
    inline constexpr HrtimTimerOutputResetSource HrtimTimerOutputResetSource::kTimerDEvent2TimerACompare4 = HrtimTimerOutputResetSource::from_ll<LL_HRTIM_OUTPUTRESET_TIMDEV2_TIMACMP4>();
    inline constexpr HrtimTimerOutputResetSource HrtimTimerOutputResetSource::kTimerDEvent3TimerBCompare2 = HrtimTimerOutputResetSource::from_ll<LL_HRTIM_OUTPUTRESET_TIMDEV3_TIMBCMP2>();
    inline constexpr HrtimTimerOutputResetSource HrtimTimerOutputResetSource::kTimerDEvent4TimerBCompare4 = HrtimTimerOutputResetSource::from_ll<LL_HRTIM_OUTPUTRESET_TIMDEV4_TIMBCMP4>();
    inline constexpr HrtimTimerOutputResetSource HrtimTimerOutputResetSource::kTimerDEvent5TimerCCompare4 = HrtimTimerOutputResetSource::from_ll<LL_HRTIM_OUTPUTRESET_TIMDEV5_TIMCCMP4>();
    inline constexpr HrtimTimerOutputResetSource HrtimTimerOutputResetSource::kTimerDEvent6TimerECompare1 = HrtimTimerOutputResetSource::from_ll<LL_HRTIM_OUTPUTRESET_TIMDEV6_TIMECMP1>();
    inline constexpr HrtimTimerOutputResetSource HrtimTimerOutputResetSource::kTimerDEvent7TimerECompare4 = HrtimTimerOutputResetSource::from_ll<LL_HRTIM_OUTPUTRESET_TIMDEV7_TIMECMP4>();
    inline constexpr HrtimTimerOutputResetSource HrtimTimerOutputResetSource::kTimerDEvent8TimerFCompare1 = HrtimTimerOutputResetSource::from_ll<LL_HRTIM_OUTPUTRESET_TIMDEV8_TIMFCMP1>();
    inline constexpr HrtimTimerOutputResetSource HrtimTimerOutputResetSource::kTimerDEvent9TimerFCompare3 = HrtimTimerOutputResetSource::from_ll<LL_HRTIM_OUTPUTRESET_TIMDEV9_TIMFCMP3>();
    inline constexpr HrtimTimerOutputResetSource HrtimTimerOutputResetSource::kTimerEEvent1TimerACompare4 = HrtimTimerOutputResetSource::from_ll<LL_HRTIM_OUTPUTRESET_TIMEEV1_TIMACMP4>();
    inline constexpr HrtimTimerOutputResetSource HrtimTimerOutputResetSource::kTimerEEvent2TimerBCompare3 = HrtimTimerOutputResetSource::from_ll<LL_HRTIM_OUTPUTRESET_TIMEEV2_TIMBCMP3>();
    inline constexpr HrtimTimerOutputResetSource HrtimTimerOutputResetSource::kTimerEEvent3TimerBCompare4 = HrtimTimerOutputResetSource::from_ll<LL_HRTIM_OUTPUTRESET_TIMEEV3_TIMBCMP4>();
    inline constexpr HrtimTimerOutputResetSource HrtimTimerOutputResetSource::kTimerEEvent4TimerCCompare1 = HrtimTimerOutputResetSource::from_ll<LL_HRTIM_OUTPUTRESET_TIMEEV4_TIMCCMP1>();
    inline constexpr HrtimTimerOutputResetSource HrtimTimerOutputResetSource::kTimerEEvent5TimerCCompare2 = HrtimTimerOutputResetSource::from_ll<LL_HRTIM_OUTPUTRESET_TIMEEV5_TIMCCMP2>();
    inline constexpr HrtimTimerOutputResetSource HrtimTimerOutputResetSource::kTimerEEvent6TimerDCompare1 = HrtimTimerOutputResetSource::from_ll<LL_HRTIM_OUTPUTRESET_TIMEEV6_TIMDCMP1>();
    inline constexpr HrtimTimerOutputResetSource HrtimTimerOutputResetSource::kTimerEEvent7TimerDCompare2 = HrtimTimerOutputResetSource::from_ll<LL_HRTIM_OUTPUTRESET_TIMEEV7_TIMDCMP2>();
    inline constexpr HrtimTimerOutputResetSource HrtimTimerOutputResetSource::kTimerEEvent8TimerFCompare3 = HrtimTimerOutputResetSource::from_ll<LL_HRTIM_OUTPUTRESET_TIMEEV8_TIMFCMP3>();
    inline constexpr HrtimTimerOutputResetSource HrtimTimerOutputResetSource::kTimerEEvent9TimerFCompare4 = HrtimTimerOutputResetSource::from_ll<LL_HRTIM_OUTPUTRESET_TIMEEV9_TIMFCMP4>();
    inline constexpr HrtimTimerOutputResetSource HrtimTimerOutputResetSource::kTimerFEvent1TimerACompare3 = HrtimTimerOutputResetSource::from_ll<LL_HRTIM_OUTPUTRESET_TIMFEV1_TIMACMP3>();
    inline constexpr HrtimTimerOutputResetSource HrtimTimerOutputResetSource::kTimerFEvent2TimerBCompare1 = HrtimTimerOutputResetSource::from_ll<LL_HRTIM_OUTPUTRESET_TIMFEV2_TIMBCMP1>();
    inline constexpr HrtimTimerOutputResetSource HrtimTimerOutputResetSource::kTimerFEvent3TimerBCompare4 = HrtimTimerOutputResetSource::from_ll<LL_HRTIM_OUTPUTRESET_TIMFEV3_TIMBCMP4>();
    inline constexpr HrtimTimerOutputResetSource HrtimTimerOutputResetSource::kTimerFEvent4TimerCCompare1 = HrtimTimerOutputResetSource::from_ll<LL_HRTIM_OUTPUTRESET_TIMFEV4_TIMCCMP1>();
    inline constexpr HrtimTimerOutputResetSource HrtimTimerOutputResetSource::kTimerFEvent5TimerCCompare4 = HrtimTimerOutputResetSource::from_ll<LL_HRTIM_OUTPUTRESET_TIMFEV5_TIMCCMP4>();
    inline constexpr HrtimTimerOutputResetSource HrtimTimerOutputResetSource::kTimerFEvent6TimerDCompare3 = HrtimTimerOutputResetSource::from_ll<LL_HRTIM_OUTPUTRESET_TIMFEV6_TIMDCMP3>();
    inline constexpr HrtimTimerOutputResetSource HrtimTimerOutputResetSource::kTimerFEvent7TimerDCompare4 = HrtimTimerOutputResetSource::from_ll<LL_HRTIM_OUTPUTRESET_TIMFEV7_TIMDCMP4>();
    inline constexpr HrtimTimerOutputResetSource HrtimTimerOutputResetSource::kTimerFEvent8TimerECompare2 = HrtimTimerOutputResetSource::from_ll<LL_HRTIM_OUTPUTRESET_TIMFEV8_TIMECMP2>();
    inline constexpr HrtimTimerOutputResetSource HrtimTimerOutputResetSource::kTimerFEvent9TimerECompare3 = HrtimTimerOutputResetSource::from_ll<LL_HRTIM_OUTPUTRESET_TIMFEV9_TIMECMP3>();
    inline constexpr HrtimTimerOutputResetSource HrtimTimerOutputResetSource::kExternalEvent1 = HrtimTimerOutputResetSource::from_ll<LL_HRTIM_OUTPUTRESET_EEV_1>();
    inline constexpr HrtimTimerOutputResetSource HrtimTimerOutputResetSource::kExternalEvent2 = HrtimTimerOutputResetSource::from_ll<LL_HRTIM_OUTPUTRESET_EEV_2>();
    inline constexpr HrtimTimerOutputResetSource HrtimTimerOutputResetSource::kExternalEvent3 = HrtimTimerOutputResetSource::from_ll<LL_HRTIM_OUTPUTRESET_EEV_3>();
    inline constexpr HrtimTimerOutputResetSource HrtimTimerOutputResetSource::kExternalEvent4 = HrtimTimerOutputResetSource::from_ll<LL_HRTIM_OUTPUTRESET_EEV_4>();
    inline constexpr HrtimTimerOutputResetSource HrtimTimerOutputResetSource::kExternalEvent5 = HrtimTimerOutputResetSource::from_ll<LL_HRTIM_OUTPUTRESET_EEV_5>();
    inline constexpr HrtimTimerOutputResetSource HrtimTimerOutputResetSource::kExternalEvent6 = HrtimTimerOutputResetSource::from_ll<LL_HRTIM_OUTPUTRESET_EEV_6>();
    inline constexpr HrtimTimerOutputResetSource HrtimTimerOutputResetSource::kExternalEvent7 = HrtimTimerOutputResetSource::from_ll<LL_HRTIM_OUTPUTRESET_EEV_7>();
    inline constexpr HrtimTimerOutputResetSource HrtimTimerOutputResetSource::kExternalEvent8 = HrtimTimerOutputResetSource::from_ll<LL_HRTIM_OUTPUTRESET_EEV_8>();
    inline constexpr HrtimTimerOutputResetSource HrtimTimerOutputResetSource::kExternalEvent9 = HrtimTimerOutputResetSource::from_ll<LL_HRTIM_OUTPUTRESET_EEV_9>();
    inline constexpr HrtimTimerOutputResetSource HrtimTimerOutputResetSource::kExternalEvent10 = HrtimTimerOutputResetSource::from_ll<LL_HRTIM_OUTPUTRESET_EEV_10>();
    inline constexpr HrtimTimerOutputResetSource HrtimTimerOutputResetSource::kUpdate = HrtimTimerOutputResetSource::from_ll<LL_HRTIM_OUTPUTRESET_UPDATE>();


    struct HrtimTimerOutputPolarity : public LLDriverEnumValue<HrtimTimerOutputPolarity>
    {
        static const HrtimTimerOutputPolarity kPositive;
        static const HrtimTimerOutputPolarity kNegative;
    };

    inline constexpr HrtimTimerOutputPolarity HrtimTimerOutputPolarity::kPositive = HrtimTimerOutputPolarity::from_ll<LL_HRTIM_OUT_POSITIVE_POLARITY>();
    inline constexpr HrtimTimerOutputPolarity HrtimTimerOutputPolarity::kNegative = HrtimTimerOutputPolarity::from_ll<LL_HRTIM_OUT_NEGATIVE_POLARITY>();


    struct HrtimTimerOutputIdleMode : public LLDriverEnumValue<HrtimTimerOutputIdleMode>
    {
        static const HrtimTimerOutputIdleMode kNoIdle;
        static const HrtimTimerOutputIdleMode kBurst;
    };

    inline constexpr HrtimTimerOutputIdleMode HrtimTimerOutputIdleMode::kNoIdle = HrtimTimerOutputIdleMode::from_ll<LL_HRTIM_OUT_NO_IdLE>();
    inline constexpr HrtimTimerOutputIdleMode HrtimTimerOutputIdleMode::kBurst = HrtimTimerOutputIdleMode::from_ll<LL_HRTIM_OUT_IdLE_WHEN_BURST>();


    struct HrtimTimerOutputIdleLevel : public LLDriverEnumValue<HrtimTimerOutputIdleLevel>
    {
        static const HrtimTimerOutputIdleLevel kInactive;
        static const HrtimTimerOutputIdleLevel kActive;
    };

    inline constexpr HrtimTimerOutputIdleLevel HrtimTimerOutputIdleLevel::kInactive = HrtimTimerOutputIdleLevel::from_ll<LL_HRTIM_OUT_IdLELEVEL_INACTIVE>();
    inline constexpr HrtimTimerOutputIdleLevel HrtimTimerOutputIdleLevel::kActive = HrtimTimerOutputIdleLevel::from_ll<LL_HRTIM_OUT_IdLELEVEL_ACTIVE>();


    struct HrtimTimerOutputFaultState : public LLDriverEnumValue<HrtimTimerOutputFaultState>
    {
        static const HrtimTimerOutputFaultState kInactive;
        static const HrtimTimerOutputFaultState kActive;
        static const HrtimTimerOutputFaultState kNoAction;
        static const HrtimTimerOutputFaultState kHighZ;
    };

    inline constexpr HrtimTimerOutputFaultState HrtimTimerOutputFaultState::kInactive = HrtimTimerOutputFaultState::from_ll<LL_HRTIM_OUT_FAULTSTATE_INACTIVE>();
    inline constexpr HrtimTimerOutputFaultState HrtimTimerOutputFaultState::kActive = HrtimTimerOutputFaultState::from_ll<LL_HRTIM_OUT_FAULTSTATE_ACTIVE>();
    inline constexpr HrtimTimerOutputFaultState HrtimTimerOutputFaultState::kNoAction = HrtimTimerOutputFaultState::from_ll<LL_HRTIM_OUT_FAULTSTATE_NO_ACTION>();
    inline constexpr HrtimTimerOutputFaultState HrtimTimerOutputFaultState::kHighZ = HrtimTimerOutputFaultState::from_ll<LL_HRTIM_OUT_FAULTSTATE_HIGHZ>();


    struct HrtimTimerOutputChopperMode : public LLDriverEnumValue<HrtimTimerOutputChopperMode>
    {
        static const HrtimTimerOutputChopperMode kDisabled;
        static const HrtimTimerOutputChopperMode kEnabled;
    };

    inline constexpr HrtimTimerOutputChopperMode HrtimTimerOutputChopperMode::kDisabled = HrtimTimerOutputChopperMode::from_ll<LL_HRTIM_OUT_CHOPPERMODE_DISABLED>();
    inline constexpr HrtimTimerOutputChopperMode HrtimTimerOutputChopperMode::kEnabled = HrtimTimerOutputChopperMode::from_ll<LL_HRTIM_OUT_CHOPPERMODE_ENABLED>();


    struct HrtimTimerOutputBurstModeEntryMode : public LLDriverEnumValue<HrtimTimerOutputBurstModeEntryMode>
    {
        static const HrtimTimerOutputBurstModeEntryMode kRegular;
        static const HrtimTimerOutputBurstModeEntryMode kDelayed;
    };

    inline constexpr HrtimTimerOutputBurstModeEntryMode HrtimTimerOutputBurstModeEntryMode::kRegular = HrtimTimerOutputBurstModeEntryMode::from_ll<LL_HRTIM_OUT_BM_ENTRYMODE_REGULAR>();
    inline constexpr HrtimTimerOutputBurstModeEntryMode HrtimTimerOutputBurstModeEntryMode::kDelayed = HrtimTimerOutputBurstModeEntryMode::from_ll<LL_HRTIM_OUT_BM_ENTRYMODE_DELAYED>();


    struct HrtimTimerMasterAdcTriggerSourceId13 : public LLDriverEnumValue<HrtimTimerMasterAdcTriggerSourceId13>
    {
        static const HrtimTimerMasterAdcTriggerSourceId13 kNone;
        static const HrtimTimerMasterAdcTriggerSourceId13 kExternalEvent1;
        static const HrtimTimerMasterAdcTriggerSourceId13 kExternalEvent2;
        static const HrtimTimerMasterAdcTriggerSourceId13 kExternalEvent3;
        static const HrtimTimerMasterAdcTriggerSourceId13 kExternalEvent4;
        static const HrtimTimerMasterAdcTriggerSourceId13 kExternalEvent5;
        static const HrtimTimerMasterAdcTriggerSourceId13 kCompare1;
        static const HrtimTimerMasterAdcTriggerSourceId13 kCompare2;
        static const HrtimTimerMasterAdcTriggerSourceId13 kCompare3;
        static const HrtimTimerMasterAdcTriggerSourceId13 kCompare4;
        static const HrtimTimerMasterAdcTriggerSourceId13 kPeriod;
    };

    inline constexpr HrtimTimerMasterAdcTriggerSourceId13 HrtimTimerMasterAdcTriggerSourceId13::kNone = HrtimTimerMasterAdcTriggerSourceId13::from_ll<LL_HRTIM_ADCTRIG_SRC13_NONE>();
    inline constexpr HrtimTimerMasterAdcTriggerSourceId13 HrtimTimerMasterAdcTriggerSourceId13::kExternalEvent1 = HrtimTimerMasterAdcTriggerSourceId13::from_ll<LL_HRTIM_ADCTRIG_SRC13_EEV1>();
    inline constexpr HrtimTimerMasterAdcTriggerSourceId13 HrtimTimerMasterAdcTriggerSourceId13::kExternalEvent2 = HrtimTimerMasterAdcTriggerSourceId13::from_ll<LL_HRTIM_ADCTRIG_SRC13_EEV2>();
    inline constexpr HrtimTimerMasterAdcTriggerSourceId13 HrtimTimerMasterAdcTriggerSourceId13::kExternalEvent3 = HrtimTimerMasterAdcTriggerSourceId13::from_ll<LL_HRTIM_ADCTRIG_SRC13_EEV3>();
    inline constexpr HrtimTimerMasterAdcTriggerSourceId13 HrtimTimerMasterAdcTriggerSourceId13::kExternalEvent4 = HrtimTimerMasterAdcTriggerSourceId13::from_ll<LL_HRTIM_ADCTRIG_SRC13_EEV4>();
    inline constexpr HrtimTimerMasterAdcTriggerSourceId13 HrtimTimerMasterAdcTriggerSourceId13::kExternalEvent5 = HrtimTimerMasterAdcTriggerSourceId13::from_ll<LL_HRTIM_ADCTRIG_SRC13_EEV5>();
    inline constexpr HrtimTimerMasterAdcTriggerSourceId13 HrtimTimerMasterAdcTriggerSourceId13::kCompare1 = HrtimTimerMasterAdcTriggerSourceId13::from_ll<LL_HRTIM_ADCTRIG_SRC13_MCMP1>();
    inline constexpr HrtimTimerMasterAdcTriggerSourceId13 HrtimTimerMasterAdcTriggerSourceId13::kCompare2 = HrtimTimerMasterAdcTriggerSourceId13::from_ll<LL_HRTIM_ADCTRIG_SRC13_MCMP2>();
    inline constexpr HrtimTimerMasterAdcTriggerSourceId13 HrtimTimerMasterAdcTriggerSourceId13::kCompare3 = HrtimTimerMasterAdcTriggerSourceId13::from_ll<LL_HRTIM_ADCTRIG_SRC13_MCMP3>();
    inline constexpr HrtimTimerMasterAdcTriggerSourceId13 HrtimTimerMasterAdcTriggerSourceId13::kCompare4 = HrtimTimerMasterAdcTriggerSourceId13::from_ll<LL_HRTIM_ADCTRIG_SRC13_MCMP4>();
    inline constexpr HrtimTimerMasterAdcTriggerSourceId13 HrtimTimerMasterAdcTriggerSourceId13::kPeriod = HrtimTimerMasterAdcTriggerSourceId13::from_ll<LL_HRTIM_ADCTRIG_SRC13_MPER>();


    struct HrtimTimerAAdcTriggerSourceId13 : public LLDriverEnumValue<HrtimTimerAAdcTriggerSourceId13>
    {
        static const HrtimTimerAAdcTriggerSourceId13 kNone;
        static const HrtimTimerAAdcTriggerSourceId13 kExternalEvent1;
        static const HrtimTimerAAdcTriggerSourceId13 kExternalEvent2;
        static const HrtimTimerAAdcTriggerSourceId13 kExternalEvent3;
        static const HrtimTimerAAdcTriggerSourceId13 kExternalEvent4;
        static const HrtimTimerAAdcTriggerSourceId13 kExternalEvent5;
        static const HrtimTimerAAdcTriggerSourceId13 kCompare3;
        static const HrtimTimerAAdcTriggerSourceId13 kCompare4;
        static const HrtimTimerAAdcTriggerSourceId13 kPeriod;
        static const HrtimTimerAAdcTriggerSourceId13 kReset;
    };

    inline constexpr HrtimTimerAAdcTriggerSourceId13 HrtimTimerAAdcTriggerSourceId13::kNone = HrtimTimerAAdcTriggerSourceId13::from_ll<LL_HRTIM_ADCTRIG_SRC13_NONE>();
    inline constexpr HrtimTimerAAdcTriggerSourceId13 HrtimTimerAAdcTriggerSourceId13::kExternalEvent1 = HrtimTimerAAdcTriggerSourceId13::from_ll<LL_HRTIM_ADCTRIG_SRC13_EEV1>();
    inline constexpr HrtimTimerAAdcTriggerSourceId13 HrtimTimerAAdcTriggerSourceId13::kExternalEvent2 = HrtimTimerAAdcTriggerSourceId13::from_ll<LL_HRTIM_ADCTRIG_SRC13_EEV2>();
    inline constexpr HrtimTimerAAdcTriggerSourceId13 HrtimTimerAAdcTriggerSourceId13::kExternalEvent3 = HrtimTimerAAdcTriggerSourceId13::from_ll<LL_HRTIM_ADCTRIG_SRC13_EEV3>();
    inline constexpr HrtimTimerAAdcTriggerSourceId13 HrtimTimerAAdcTriggerSourceId13::kExternalEvent4 = HrtimTimerAAdcTriggerSourceId13::from_ll<LL_HRTIM_ADCTRIG_SRC13_EEV4>();
    inline constexpr HrtimTimerAAdcTriggerSourceId13 HrtimTimerAAdcTriggerSourceId13::kExternalEvent5 = HrtimTimerAAdcTriggerSourceId13::from_ll<LL_HRTIM_ADCTRIG_SRC13_EEV5>();
    inline constexpr HrtimTimerAAdcTriggerSourceId13 HrtimTimerAAdcTriggerSourceId13::kCompare3 = HrtimTimerAAdcTriggerSourceId13::from_ll<LL_HRTIM_ADCTRIG_SRC13_TIMACMP3>();
    inline constexpr HrtimTimerAAdcTriggerSourceId13 HrtimTimerAAdcTriggerSourceId13::kCompare4 = HrtimTimerAAdcTriggerSourceId13::from_ll<LL_HRTIM_ADCTRIG_SRC13_TIMACMP4>();
    inline constexpr HrtimTimerAAdcTriggerSourceId13 HrtimTimerAAdcTriggerSourceId13::kPeriod = HrtimTimerAAdcTriggerSourceId13::from_ll<LL_HRTIM_ADCTRIG_SRC13_TIMAPER>();
    inline constexpr HrtimTimerAAdcTriggerSourceId13 HrtimTimerAAdcTriggerSourceId13::kReset = HrtimTimerAAdcTriggerSourceId13::from_ll<LL_HRTIM_ADCTRIG_SRC13_TIMARST>();


    struct HrtimTimerBAdcTriggerSourceId13 : public LLDriverEnumValue<HrtimTimerBAdcTriggerSourceId13>
    {
        static const HrtimTimerBAdcTriggerSourceId13 kNone;
        static const HrtimTimerBAdcTriggerSourceId13 kExternalEvent1;
        static const HrtimTimerBAdcTriggerSourceId13 kExternalEvent2;
        static const HrtimTimerBAdcTriggerSourceId13 kExternalEvent3;
        static const HrtimTimerBAdcTriggerSourceId13 kExternalEvent4;
        static const HrtimTimerBAdcTriggerSourceId13 kExternalEvent5;
        static const HrtimTimerBAdcTriggerSourceId13 kCompare3;
        static const HrtimTimerBAdcTriggerSourceId13 kCompare4;
        static const HrtimTimerBAdcTriggerSourceId13 kPeriod;
        static const HrtimTimerBAdcTriggerSourceId13 kReset;
    };

    inline constexpr HrtimTimerBAdcTriggerSourceId13 HrtimTimerBAdcTriggerSourceId13::kNone = HrtimTimerBAdcTriggerSourceId13::from_ll<LL_HRTIM_ADCTRIG_SRC13_NONE>();
    inline constexpr HrtimTimerBAdcTriggerSourceId13 HrtimTimerBAdcTriggerSourceId13::kExternalEvent1 = HrtimTimerBAdcTriggerSourceId13::from_ll<LL_HRTIM_ADCTRIG_SRC13_EEV1>();
    inline constexpr HrtimTimerBAdcTriggerSourceId13 HrtimTimerBAdcTriggerSourceId13::kExternalEvent2 = HrtimTimerBAdcTriggerSourceId13::from_ll<LL_HRTIM_ADCTRIG_SRC13_EEV2>();
    inline constexpr HrtimTimerBAdcTriggerSourceId13 HrtimTimerBAdcTriggerSourceId13::kExternalEvent3 = HrtimTimerBAdcTriggerSourceId13::from_ll<LL_HRTIM_ADCTRIG_SRC13_EEV3>();
    inline constexpr HrtimTimerBAdcTriggerSourceId13 HrtimTimerBAdcTriggerSourceId13::kExternalEvent4 = HrtimTimerBAdcTriggerSourceId13::from_ll<LL_HRTIM_ADCTRIG_SRC13_EEV4>();
    inline constexpr HrtimTimerBAdcTriggerSourceId13 HrtimTimerBAdcTriggerSourceId13::kExternalEvent5 = HrtimTimerBAdcTriggerSourceId13::from_ll<LL_HRTIM_ADCTRIG_SRC13_EEV5>();
    inline constexpr HrtimTimerBAdcTriggerSourceId13 HrtimTimerBAdcTriggerSourceId13::kCompare3 = HrtimTimerBAdcTriggerSourceId13::from_ll<LL_HRTIM_ADCTRIG_SRC13_TIMBCMP3>();
    inline constexpr HrtimTimerBAdcTriggerSourceId13 HrtimTimerBAdcTriggerSourceId13::kCompare4 = HrtimTimerBAdcTriggerSourceId13::from_ll<LL_HRTIM_ADCTRIG_SRC13_TIMBCMP4>();
    inline constexpr HrtimTimerBAdcTriggerSourceId13 HrtimTimerBAdcTriggerSourceId13::kPeriod = HrtimTimerBAdcTriggerSourceId13::from_ll<LL_HRTIM_ADCTRIG_SRC13_TIMBPER>();
    inline constexpr HrtimTimerBAdcTriggerSourceId13 HrtimTimerBAdcTriggerSourceId13::kReset = HrtimTimerBAdcTriggerSourceId13::from_ll<LL_HRTIM_ADCTRIG_SRC13_TIMBRST>();


    struct HrtimTimerCAdcTriggerSourceId13 : public LLDriverEnumValue<HrtimTimerCAdcTriggerSourceId13>
    {
        static const HrtimTimerCAdcTriggerSourceId13 kNone;
        static const HrtimTimerCAdcTriggerSourceId13 kExternalEvent1;
        static const HrtimTimerCAdcTriggerSourceId13 kExternalEvent2;
        static const HrtimTimerCAdcTriggerSourceId13 kExternalEvent3;
        static const HrtimTimerCAdcTriggerSourceId13 kExternalEvent4;
        static const HrtimTimerCAdcTriggerSourceId13 kExternalEvent5;
        static const HrtimTimerCAdcTriggerSourceId13 kCompare3;
        static const HrtimTimerCAdcTriggerSourceId13 kCompare4;
        static const HrtimTimerCAdcTriggerSourceId13 kPeriod;
    };

    inline constexpr HrtimTimerCAdcTriggerSourceId13 HrtimTimerCAdcTriggerSourceId13::kNone = HrtimTimerCAdcTriggerSourceId13::from_ll<LL_HRTIM_ADCTRIG_SRC13_NONE>();
    inline constexpr HrtimTimerCAdcTriggerSourceId13 HrtimTimerCAdcTriggerSourceId13::kExternalEvent1 = HrtimTimerCAdcTriggerSourceId13::from_ll<LL_HRTIM_ADCTRIG_SRC13_EEV1>();
    inline constexpr HrtimTimerCAdcTriggerSourceId13 HrtimTimerCAdcTriggerSourceId13::kExternalEvent2 = HrtimTimerCAdcTriggerSourceId13::from_ll<LL_HRTIM_ADCTRIG_SRC13_EEV2>();
    inline constexpr HrtimTimerCAdcTriggerSourceId13 HrtimTimerCAdcTriggerSourceId13::kExternalEvent3 = HrtimTimerCAdcTriggerSourceId13::from_ll<LL_HRTIM_ADCTRIG_SRC13_EEV3>();
    inline constexpr HrtimTimerCAdcTriggerSourceId13 HrtimTimerCAdcTriggerSourceId13::kExternalEvent4 = HrtimTimerCAdcTriggerSourceId13::from_ll<LL_HRTIM_ADCTRIG_SRC13_EEV4>();
    inline constexpr HrtimTimerCAdcTriggerSourceId13 HrtimTimerCAdcTriggerSourceId13::kExternalEvent5 = HrtimTimerCAdcTriggerSourceId13::from_ll<LL_HRTIM_ADCTRIG_SRC13_EEV5>();
    inline constexpr HrtimTimerCAdcTriggerSourceId13 HrtimTimerCAdcTriggerSourceId13::kCompare3 = HrtimTimerCAdcTriggerSourceId13::from_ll<LL_HRTIM_ADCTRIG_SRC13_TIMCCMP3>();
    inline constexpr HrtimTimerCAdcTriggerSourceId13 HrtimTimerCAdcTriggerSourceId13::kCompare4 = HrtimTimerCAdcTriggerSourceId13::from_ll<LL_HRTIM_ADCTRIG_SRC13_TIMCCMP4>();
    inline constexpr HrtimTimerCAdcTriggerSourceId13 HrtimTimerCAdcTriggerSourceId13::kPeriod = HrtimTimerCAdcTriggerSourceId13::from_ll<LL_HRTIM_ADCTRIG_SRC13_TIMCPER>();


    struct HrtimTimerDAdcTriggerSourceId13 : public LLDriverEnumValue<HrtimTimerDAdcTriggerSourceId13>
    {
        static const HrtimTimerDAdcTriggerSourceId13 kNone;
        static const HrtimTimerDAdcTriggerSourceId13 kExternalEvent1;
        static const HrtimTimerDAdcTriggerSourceId13 kExternalEvent2;
        static const HrtimTimerDAdcTriggerSourceId13 kExternalEvent3;
        static const HrtimTimerDAdcTriggerSourceId13 kExternalEvent4;
        static const HrtimTimerDAdcTriggerSourceId13 kExternalEvent5;
        static const HrtimTimerDAdcTriggerSourceId13 kCompare3;
        static const HrtimTimerDAdcTriggerSourceId13 kCompare4;
        static const HrtimTimerDAdcTriggerSourceId13 kPeriod;
    };

    inline constexpr HrtimTimerDAdcTriggerSourceId13 HrtimTimerDAdcTriggerSourceId13::kNone = HrtimTimerDAdcTriggerSourceId13::from_ll<LL_HRTIM_ADCTRIG_SRC13_NONE>();
    inline constexpr HrtimTimerDAdcTriggerSourceId13 HrtimTimerDAdcTriggerSourceId13::kExternalEvent1 = HrtimTimerDAdcTriggerSourceId13::from_ll<LL_HRTIM_ADCTRIG_SRC13_EEV1>();
    inline constexpr HrtimTimerDAdcTriggerSourceId13 HrtimTimerDAdcTriggerSourceId13::kExternalEvent2 = HrtimTimerDAdcTriggerSourceId13::from_ll<LL_HRTIM_ADCTRIG_SRC13_EEV2>();
    inline constexpr HrtimTimerDAdcTriggerSourceId13 HrtimTimerDAdcTriggerSourceId13::kExternalEvent3 = HrtimTimerDAdcTriggerSourceId13::from_ll<LL_HRTIM_ADCTRIG_SRC13_EEV3>();
    inline constexpr HrtimTimerDAdcTriggerSourceId13 HrtimTimerDAdcTriggerSourceId13::kExternalEvent4 = HrtimTimerDAdcTriggerSourceId13::from_ll<LL_HRTIM_ADCTRIG_SRC13_EEV4>();
    inline constexpr HrtimTimerDAdcTriggerSourceId13 HrtimTimerDAdcTriggerSourceId13::kExternalEvent5 = HrtimTimerDAdcTriggerSourceId13::from_ll<LL_HRTIM_ADCTRIG_SRC13_EEV5>();
    inline constexpr HrtimTimerDAdcTriggerSourceId13 HrtimTimerDAdcTriggerSourceId13::kCompare3 = HrtimTimerDAdcTriggerSourceId13::from_ll<LL_HRTIM_ADCTRIG_SRC13_TIMDCMP3>();
    inline constexpr HrtimTimerDAdcTriggerSourceId13 HrtimTimerDAdcTriggerSourceId13::kCompare4 = HrtimTimerDAdcTriggerSourceId13::from_ll<LL_HRTIM_ADCTRIG_SRC13_TIMDCMP4>();
    inline constexpr HrtimTimerDAdcTriggerSourceId13 HrtimTimerDAdcTriggerSourceId13::kPeriod = HrtimTimerDAdcTriggerSourceId13::from_ll<LL_HRTIM_ADCTRIG_SRC13_TIMDPER>();


    struct HrtimTimerEAdcTriggerSourceId13 : public LLDriverEnumValue<HrtimTimerEAdcTriggerSourceId13>
    {
        static const HrtimTimerEAdcTriggerSourceId13 kNone;
        static const HrtimTimerEAdcTriggerSourceId13 kExternalEvent1;
        static const HrtimTimerEAdcTriggerSourceId13 kExternalEvent2;
        static const HrtimTimerEAdcTriggerSourceId13 kExternalEvent3;
        static const HrtimTimerEAdcTriggerSourceId13 kExternalEvent4;
        static const HrtimTimerEAdcTriggerSourceId13 kExternalEvent5;
        static const HrtimTimerEAdcTriggerSourceId13 kCompare3;
        static const HrtimTimerEAdcTriggerSourceId13 kCompare4;
        static const HrtimTimerEAdcTriggerSourceId13 kPeriod;
    };

    inline constexpr HrtimTimerEAdcTriggerSourceId13 HrtimTimerEAdcTriggerSourceId13::kNone = HrtimTimerEAdcTriggerSourceId13::from_ll<LL_HRTIM_ADCTRIG_SRC13_NONE>();
    inline constexpr HrtimTimerEAdcTriggerSourceId13 HrtimTimerEAdcTriggerSourceId13::kExternalEvent1 = HrtimTimerEAdcTriggerSourceId13::from_ll<LL_HRTIM_ADCTRIG_SRC13_EEV1>();
    inline constexpr HrtimTimerEAdcTriggerSourceId13 HrtimTimerEAdcTriggerSourceId13::kExternalEvent2 = HrtimTimerEAdcTriggerSourceId13::from_ll<LL_HRTIM_ADCTRIG_SRC13_EEV2>();
    inline constexpr HrtimTimerEAdcTriggerSourceId13 HrtimTimerEAdcTriggerSourceId13::kExternalEvent3 = HrtimTimerEAdcTriggerSourceId13::from_ll<LL_HRTIM_ADCTRIG_SRC13_EEV3>();
    inline constexpr HrtimTimerEAdcTriggerSourceId13 HrtimTimerEAdcTriggerSourceId13::kExternalEvent4 = HrtimTimerEAdcTriggerSourceId13::from_ll<LL_HRTIM_ADCTRIG_SRC13_EEV4>();
    inline constexpr HrtimTimerEAdcTriggerSourceId13 HrtimTimerEAdcTriggerSourceId13::kExternalEvent5 = HrtimTimerEAdcTriggerSourceId13::from_ll<LL_HRTIM_ADCTRIG_SRC13_EEV5>();
    inline constexpr HrtimTimerEAdcTriggerSourceId13 HrtimTimerEAdcTriggerSourceId13::kCompare3 = HrtimTimerEAdcTriggerSourceId13::from_ll<LL_HRTIM_ADCTRIG_SRC13_TIMECMP3>();
    inline constexpr HrtimTimerEAdcTriggerSourceId13 HrtimTimerEAdcTriggerSourceId13::kCompare4 = HrtimTimerEAdcTriggerSourceId13::from_ll<LL_HRTIM_ADCTRIG_SRC13_TIMECMP4>();
    inline constexpr HrtimTimerEAdcTriggerSourceId13 HrtimTimerEAdcTriggerSourceId13::kPeriod = HrtimTimerEAdcTriggerSourceId13::from_ll<LL_HRTIM_ADCTRIG_SRC13_TIMEPER>();


    struct HrtimTimerFAdcTriggerSourceId13 : public LLDriverEnumValue<HrtimTimerFAdcTriggerSourceId13>
    {
        static const HrtimTimerFAdcTriggerSourceId13 kNone;
        static const HrtimTimerFAdcTriggerSourceId13 kExternalEvent1;
        static const HrtimTimerFAdcTriggerSourceId13 kExternalEvent2;
        static const HrtimTimerFAdcTriggerSourceId13 kExternalEvent3;
        static const HrtimTimerFAdcTriggerSourceId13 kExternalEvent4;
        static const HrtimTimerFAdcTriggerSourceId13 kExternalEvent5;
        static const HrtimTimerFAdcTriggerSourceId13 kCompare2;
        static const HrtimTimerFAdcTriggerSourceId13 kCompare3;
        static const HrtimTimerFAdcTriggerSourceId13 kCompare4;
        static const HrtimTimerFAdcTriggerSourceId13 kPeriod;
        static const HrtimTimerFAdcTriggerSourceId13 kReset;
    };

    inline constexpr HrtimTimerFAdcTriggerSourceId13 HrtimTimerFAdcTriggerSourceId13::kNone = HrtimTimerFAdcTriggerSourceId13::from_ll<LL_HRTIM_ADCTRIG_SRC13_NONE>();
    inline constexpr HrtimTimerFAdcTriggerSourceId13 HrtimTimerFAdcTriggerSourceId13::kExternalEvent1 = HrtimTimerFAdcTriggerSourceId13::from_ll<LL_HRTIM_ADCTRIG_SRC13_EEV1>();
    inline constexpr HrtimTimerFAdcTriggerSourceId13 HrtimTimerFAdcTriggerSourceId13::kExternalEvent2 = HrtimTimerFAdcTriggerSourceId13::from_ll<LL_HRTIM_ADCTRIG_SRC13_EEV2>();
    inline constexpr HrtimTimerFAdcTriggerSourceId13 HrtimTimerFAdcTriggerSourceId13::kExternalEvent3 = HrtimTimerFAdcTriggerSourceId13::from_ll<LL_HRTIM_ADCTRIG_SRC13_EEV3>();
    inline constexpr HrtimTimerFAdcTriggerSourceId13 HrtimTimerFAdcTriggerSourceId13::kExternalEvent4 = HrtimTimerFAdcTriggerSourceId13::from_ll<LL_HRTIM_ADCTRIG_SRC13_EEV4>();
    inline constexpr HrtimTimerFAdcTriggerSourceId13 HrtimTimerFAdcTriggerSourceId13::kExternalEvent5 = HrtimTimerFAdcTriggerSourceId13::from_ll<LL_HRTIM_ADCTRIG_SRC13_EEV5>();
    inline constexpr HrtimTimerFAdcTriggerSourceId13 HrtimTimerFAdcTriggerSourceId13::kCompare2 = HrtimTimerFAdcTriggerSourceId13::from_ll<LL_HRTIM_ADCTRIG_SRC13_TIMFCMP2>();
    inline constexpr HrtimTimerFAdcTriggerSourceId13 HrtimTimerFAdcTriggerSourceId13::kCompare3 = HrtimTimerFAdcTriggerSourceId13::from_ll<LL_HRTIM_ADCTRIG_SRC13_TIMFCMP3>();
    inline constexpr HrtimTimerFAdcTriggerSourceId13 HrtimTimerFAdcTriggerSourceId13::kCompare4 = HrtimTimerFAdcTriggerSourceId13::from_ll<LL_HRTIM_ADCTRIG_SRC13_TIMFCMP4>();
    inline constexpr HrtimTimerFAdcTriggerSourceId13 HrtimTimerFAdcTriggerSourceId13::kPeriod = HrtimTimerFAdcTriggerSourceId13::from_ll<LL_HRTIM_ADCTRIG_SRC13_TIMFPER>();
    inline constexpr HrtimTimerFAdcTriggerSourceId13 HrtimTimerFAdcTriggerSourceId13::kReset = HrtimTimerFAdcTriggerSourceId13::from_ll<LL_HRTIM_ADCTRIG_SRC13_TIMFRST>();


    struct HrtimTimerMasterAdcTriggerSourceId24 : public LLDriverEnumValue<HrtimTimerMasterAdcTriggerSourceId24>
    {
        static const HrtimTimerMasterAdcTriggerSourceId24 kNone;
        static const HrtimTimerMasterAdcTriggerSourceId24 kCompare1;
        static const HrtimTimerMasterAdcTriggerSourceId24 kCompare2;
        static const HrtimTimerMasterAdcTriggerSourceId24 kCompare3;
        static const HrtimTimerMasterAdcTriggerSourceId24 kCompare4;
        static const HrtimTimerMasterAdcTriggerSourceId24 kPeriod;
        static const HrtimTimerMasterAdcTriggerSourceId24 kExternalEvent6;
        static const HrtimTimerMasterAdcTriggerSourceId24 kExternalEvent7;
        static const HrtimTimerMasterAdcTriggerSourceId24 kExternalEvent8;
        static const HrtimTimerMasterAdcTriggerSourceId24 kExternalEvent9;
        static const HrtimTimerMasterAdcTriggerSourceId24 kExternalEvent10;
    };

    inline constexpr HrtimTimerMasterAdcTriggerSourceId24 HrtimTimerMasterAdcTriggerSourceId24::kNone = HrtimTimerMasterAdcTriggerSourceId24::from_ll<LL_HRTIM_ADCTRIG_SRC24_NONE>();
    inline constexpr HrtimTimerMasterAdcTriggerSourceId24 HrtimTimerMasterAdcTriggerSourceId24::kCompare1 = HrtimTimerMasterAdcTriggerSourceId24::from_ll<LL_HRTIM_ADCTRIG_SRC24_MCMP1>();
    inline constexpr HrtimTimerMasterAdcTriggerSourceId24 HrtimTimerMasterAdcTriggerSourceId24::kCompare2 = HrtimTimerMasterAdcTriggerSourceId24::from_ll<LL_HRTIM_ADCTRIG_SRC24_MCMP2>();
    inline constexpr HrtimTimerMasterAdcTriggerSourceId24 HrtimTimerMasterAdcTriggerSourceId24::kCompare3 = HrtimTimerMasterAdcTriggerSourceId24::from_ll<LL_HRTIM_ADCTRIG_SRC24_MCMP3>();
    inline constexpr HrtimTimerMasterAdcTriggerSourceId24 HrtimTimerMasterAdcTriggerSourceId24::kCompare4 = HrtimTimerMasterAdcTriggerSourceId24::from_ll<LL_HRTIM_ADCTRIG_SRC24_MCMP4>();
    inline constexpr HrtimTimerMasterAdcTriggerSourceId24 HrtimTimerMasterAdcTriggerSourceId24::kPeriod = HrtimTimerMasterAdcTriggerSourceId24::from_ll<LL_HRTIM_ADCTRIG_SRC24_MPER>();
    inline constexpr HrtimTimerMasterAdcTriggerSourceId24 HrtimTimerMasterAdcTriggerSourceId24::kExternalEvent6 = HrtimTimerMasterAdcTriggerSourceId24::from_ll<LL_HRTIM_ADCTRIG_SRC24_EEV6>();
    inline constexpr HrtimTimerMasterAdcTriggerSourceId24 HrtimTimerMasterAdcTriggerSourceId24::kExternalEvent7 = HrtimTimerMasterAdcTriggerSourceId24::from_ll<LL_HRTIM_ADCTRIG_SRC24_EEV7>();
    inline constexpr HrtimTimerMasterAdcTriggerSourceId24 HrtimTimerMasterAdcTriggerSourceId24::kExternalEvent8 = HrtimTimerMasterAdcTriggerSourceId24::from_ll<LL_HRTIM_ADCTRIG_SRC24_EEV8>();
    inline constexpr HrtimTimerMasterAdcTriggerSourceId24 HrtimTimerMasterAdcTriggerSourceId24::kExternalEvent9 = HrtimTimerMasterAdcTriggerSourceId24::from_ll<LL_HRTIM_ADCTRIG_SRC24_EEV9>();
    inline constexpr HrtimTimerMasterAdcTriggerSourceId24 HrtimTimerMasterAdcTriggerSourceId24::kExternalEvent10 = HrtimTimerMasterAdcTriggerSourceId24::from_ll<LL_HRTIM_ADCTRIG_SRC24_EEV10>();


    struct HrtimTimerAAdcTriggerSourceId24 : public LLDriverEnumValue<HrtimTimerAAdcTriggerSourceId24>
    {
        static const HrtimTimerAAdcTriggerSourceId24 kNone;
        static const HrtimTimerAAdcTriggerSourceId24 kExternalEvent6;
        static const HrtimTimerAAdcTriggerSourceId24 kExternalEvent7;
        static const HrtimTimerAAdcTriggerSourceId24 kExternalEvent8;
        static const HrtimTimerAAdcTriggerSourceId24 kExternalEvent9;
        static const HrtimTimerAAdcTriggerSourceId24 kExternalEvent10;
        static const HrtimTimerAAdcTriggerSourceId24 kCompare2;
        static const HrtimTimerAAdcTriggerSourceId24 kCompare4;
        static const HrtimTimerAAdcTriggerSourceId24 kPeriod;
    };

    inline constexpr HrtimTimerAAdcTriggerSourceId24 HrtimTimerAAdcTriggerSourceId24::kNone = HrtimTimerAAdcTriggerSourceId24::from_ll<LL_HRTIM_ADCTRIG_SRC24_NONE>();
    inline constexpr HrtimTimerAAdcTriggerSourceId24 HrtimTimerAAdcTriggerSourceId24::kExternalEvent6 = HrtimTimerAAdcTriggerSourceId24::from_ll<LL_HRTIM_ADCTRIG_SRC24_EEV6>();
    inline constexpr HrtimTimerAAdcTriggerSourceId24 HrtimTimerAAdcTriggerSourceId24::kExternalEvent7 = HrtimTimerAAdcTriggerSourceId24::from_ll<LL_HRTIM_ADCTRIG_SRC24_EEV7>();
    inline constexpr HrtimTimerAAdcTriggerSourceId24 HrtimTimerAAdcTriggerSourceId24::kExternalEvent8 = HrtimTimerAAdcTriggerSourceId24::from_ll<LL_HRTIM_ADCTRIG_SRC24_EEV8>();
    inline constexpr HrtimTimerAAdcTriggerSourceId24 HrtimTimerAAdcTriggerSourceId24::kExternalEvent9 = HrtimTimerAAdcTriggerSourceId24::from_ll<LL_HRTIM_ADCTRIG_SRC24_EEV9>();
    inline constexpr HrtimTimerAAdcTriggerSourceId24 HrtimTimerAAdcTriggerSourceId24::kExternalEvent10 = HrtimTimerAAdcTriggerSourceId24::from_ll<LL_HRTIM_ADCTRIG_SRC24_EEV10>();
    inline constexpr HrtimTimerAAdcTriggerSourceId24 HrtimTimerAAdcTriggerSourceId24::kCompare2 = HrtimTimerAAdcTriggerSourceId24::from_ll<LL_HRTIM_ADCTRIG_SRC24_TIMACMP2>();
    inline constexpr HrtimTimerAAdcTriggerSourceId24 HrtimTimerAAdcTriggerSourceId24::kCompare4 = HrtimTimerAAdcTriggerSourceId24::from_ll<LL_HRTIM_ADCTRIG_SRC24_TIMACMP4>();
    inline constexpr HrtimTimerAAdcTriggerSourceId24 HrtimTimerAAdcTriggerSourceId24::kPeriod = HrtimTimerAAdcTriggerSourceId24::from_ll<LL_HRTIM_ADCTRIG_SRC24_TIMAPER>();


    struct HrtimTimerBAdcTriggerSourceId24 : public LLDriverEnumValue<HrtimTimerBAdcTriggerSourceId24>
    {
        static const HrtimTimerBAdcTriggerSourceId24 kNone;
        static const HrtimTimerBAdcTriggerSourceId24 kExternalEvent6;
        static const HrtimTimerBAdcTriggerSourceId24 kExternalEvent7;
        static const HrtimTimerBAdcTriggerSourceId24 kExternalEvent8;
        static const HrtimTimerBAdcTriggerSourceId24 kExternalEvent9;
        static const HrtimTimerBAdcTriggerSourceId24 kExternalEvent10;
        static const HrtimTimerBAdcTriggerSourceId24 kCompare2;
        static const HrtimTimerBAdcTriggerSourceId24 kCompare4;
        static const HrtimTimerBAdcTriggerSourceId24 kPeriod;
    };

    inline constexpr HrtimTimerBAdcTriggerSourceId24 HrtimTimerBAdcTriggerSourceId24::kNone = HrtimTimerBAdcTriggerSourceId24::from_ll<LL_HRTIM_ADCTRIG_SRC24_NONE>();
    inline constexpr HrtimTimerBAdcTriggerSourceId24 HrtimTimerBAdcTriggerSourceId24::kExternalEvent6 = HrtimTimerBAdcTriggerSourceId24::from_ll<LL_HRTIM_ADCTRIG_SRC24_EEV6>();
    inline constexpr HrtimTimerBAdcTriggerSourceId24 HrtimTimerBAdcTriggerSourceId24::kExternalEvent7 = HrtimTimerBAdcTriggerSourceId24::from_ll<LL_HRTIM_ADCTRIG_SRC24_EEV7>();
    inline constexpr HrtimTimerBAdcTriggerSourceId24 HrtimTimerBAdcTriggerSourceId24::kExternalEvent8 = HrtimTimerBAdcTriggerSourceId24::from_ll<LL_HRTIM_ADCTRIG_SRC24_EEV8>();
    inline constexpr HrtimTimerBAdcTriggerSourceId24 HrtimTimerBAdcTriggerSourceId24::kExternalEvent9 = HrtimTimerBAdcTriggerSourceId24::from_ll<LL_HRTIM_ADCTRIG_SRC24_EEV9>();
    inline constexpr HrtimTimerBAdcTriggerSourceId24 HrtimTimerBAdcTriggerSourceId24::kExternalEvent10 = HrtimTimerBAdcTriggerSourceId24::from_ll<LL_HRTIM_ADCTRIG_SRC24_EEV10>();
    inline constexpr HrtimTimerBAdcTriggerSourceId24 HrtimTimerBAdcTriggerSourceId24::kCompare2 = HrtimTimerBAdcTriggerSourceId24::from_ll<LL_HRTIM_ADCTRIG_SRC24_TIMBCMP2>();
    inline constexpr HrtimTimerBAdcTriggerSourceId24 HrtimTimerBAdcTriggerSourceId24::kCompare4 = HrtimTimerBAdcTriggerSourceId24::from_ll<LL_HRTIM_ADCTRIG_SRC24_TIMBCMP4>();
    inline constexpr HrtimTimerBAdcTriggerSourceId24 HrtimTimerBAdcTriggerSourceId24::kPeriod = HrtimTimerBAdcTriggerSourceId24::from_ll<LL_HRTIM_ADCTRIG_SRC24_TIMBPER>();


    struct HrtimTimerCAdcTriggerSourceId24 : public LLDriverEnumValue<HrtimTimerCAdcTriggerSourceId24>
    {
        static const HrtimTimerCAdcTriggerSourceId24 kNone;
        static const HrtimTimerCAdcTriggerSourceId24 kExternalEvent6;
        static const HrtimTimerCAdcTriggerSourceId24 kExternalEvent7;
        static const HrtimTimerCAdcTriggerSourceId24 kExternalEvent8;
        static const HrtimTimerCAdcTriggerSourceId24 kExternalEvent9;
        static const HrtimTimerCAdcTriggerSourceId24 kExternalEvent10;
        static const HrtimTimerCAdcTriggerSourceId24 kCompare2;
        static const HrtimTimerCAdcTriggerSourceId24 kCompare4;
        static const HrtimTimerCAdcTriggerSourceId24 kPeriod;
        static const HrtimTimerCAdcTriggerSourceId24 kReset;
    };

    inline constexpr HrtimTimerCAdcTriggerSourceId24 HrtimTimerCAdcTriggerSourceId24::kNone = HrtimTimerCAdcTriggerSourceId24::from_ll<LL_HRTIM_ADCTRIG_SRC24_NONE>();
    inline constexpr HrtimTimerCAdcTriggerSourceId24 HrtimTimerCAdcTriggerSourceId24::kExternalEvent6 = HrtimTimerCAdcTriggerSourceId24::from_ll<LL_HRTIM_ADCTRIG_SRC24_EEV6>();
    inline constexpr HrtimTimerCAdcTriggerSourceId24 HrtimTimerCAdcTriggerSourceId24::kExternalEvent7 = HrtimTimerCAdcTriggerSourceId24::from_ll<LL_HRTIM_ADCTRIG_SRC24_EEV7>();
    inline constexpr HrtimTimerCAdcTriggerSourceId24 HrtimTimerCAdcTriggerSourceId24::kExternalEvent8 = HrtimTimerCAdcTriggerSourceId24::from_ll<LL_HRTIM_ADCTRIG_SRC24_EEV8>();
    inline constexpr HrtimTimerCAdcTriggerSourceId24 HrtimTimerCAdcTriggerSourceId24::kExternalEvent9 = HrtimTimerCAdcTriggerSourceId24::from_ll<LL_HRTIM_ADCTRIG_SRC24_EEV9>();
    inline constexpr HrtimTimerCAdcTriggerSourceId24 HrtimTimerCAdcTriggerSourceId24::kExternalEvent10 = HrtimTimerCAdcTriggerSourceId24::from_ll<LL_HRTIM_ADCTRIG_SRC24_EEV10>();
    inline constexpr HrtimTimerCAdcTriggerSourceId24 HrtimTimerCAdcTriggerSourceId24::kCompare2 = HrtimTimerCAdcTriggerSourceId24::from_ll<LL_HRTIM_ADCTRIG_SRC24_TIMCCMP2>();
    inline constexpr HrtimTimerCAdcTriggerSourceId24 HrtimTimerCAdcTriggerSourceId24::kCompare4 = HrtimTimerCAdcTriggerSourceId24::from_ll<LL_HRTIM_ADCTRIG_SRC24_TIMCCMP4>();
    inline constexpr HrtimTimerCAdcTriggerSourceId24 HrtimTimerCAdcTriggerSourceId24::kPeriod = HrtimTimerCAdcTriggerSourceId24::from_ll<LL_HRTIM_ADCTRIG_SRC24_TIMCPER>();
    inline constexpr HrtimTimerCAdcTriggerSourceId24 HrtimTimerCAdcTriggerSourceId24::kReset = HrtimTimerCAdcTriggerSourceId24::from_ll<LL_HRTIM_ADCTRIG_SRC24_TIMCRST>();


    struct HrtimTimerDAdcTriggerSourceId24 : public LLDriverEnumValue<HrtimTimerDAdcTriggerSourceId24>
    {
        static const HrtimTimerDAdcTriggerSourceId24 kNone;
        static const HrtimTimerDAdcTriggerSourceId24 kExternalEvent6;
        static const HrtimTimerDAdcTriggerSourceId24 kExternalEvent7;
        static const HrtimTimerDAdcTriggerSourceId24 kExternalEvent8;
        static const HrtimTimerDAdcTriggerSourceId24 kExternalEvent9;
        static const HrtimTimerDAdcTriggerSourceId24 kExternalEvent10;
        static const HrtimTimerDAdcTriggerSourceId24 kCompare2;
        static const HrtimTimerDAdcTriggerSourceId24 kCompare4;
        static const HrtimTimerDAdcTriggerSourceId24 kPeriod;
        static const HrtimTimerDAdcTriggerSourceId24 kReset;
    };

    inline constexpr HrtimTimerDAdcTriggerSourceId24 HrtimTimerDAdcTriggerSourceId24::kNone = HrtimTimerDAdcTriggerSourceId24::from_ll<LL_HRTIM_ADCTRIG_SRC24_NONE>();
    inline constexpr HrtimTimerDAdcTriggerSourceId24 HrtimTimerDAdcTriggerSourceId24::kExternalEvent6 = HrtimTimerDAdcTriggerSourceId24::from_ll<LL_HRTIM_ADCTRIG_SRC24_EEV6>();
    inline constexpr HrtimTimerDAdcTriggerSourceId24 HrtimTimerDAdcTriggerSourceId24::kExternalEvent7 = HrtimTimerDAdcTriggerSourceId24::from_ll<LL_HRTIM_ADCTRIG_SRC24_EEV7>();
    inline constexpr HrtimTimerDAdcTriggerSourceId24 HrtimTimerDAdcTriggerSourceId24::kExternalEvent8 = HrtimTimerDAdcTriggerSourceId24::from_ll<LL_HRTIM_ADCTRIG_SRC24_EEV8>();
    inline constexpr HrtimTimerDAdcTriggerSourceId24 HrtimTimerDAdcTriggerSourceId24::kExternalEvent9 = HrtimTimerDAdcTriggerSourceId24::from_ll<LL_HRTIM_ADCTRIG_SRC24_EEV9>();
    inline constexpr HrtimTimerDAdcTriggerSourceId24 HrtimTimerDAdcTriggerSourceId24::kExternalEvent10 = HrtimTimerDAdcTriggerSourceId24::from_ll<LL_HRTIM_ADCTRIG_SRC24_EEV10>();
    inline constexpr HrtimTimerDAdcTriggerSourceId24 HrtimTimerDAdcTriggerSourceId24::kCompare2 = HrtimTimerDAdcTriggerSourceId24::from_ll<LL_HRTIM_ADCTRIG_SRC24_TIMDCMP2>();
    inline constexpr HrtimTimerDAdcTriggerSourceId24 HrtimTimerDAdcTriggerSourceId24::kCompare4 = HrtimTimerDAdcTriggerSourceId24::from_ll<LL_HRTIM_ADCTRIG_SRC24_TIMDCMP4>();
    inline constexpr HrtimTimerDAdcTriggerSourceId24 HrtimTimerDAdcTriggerSourceId24::kPeriod = HrtimTimerDAdcTriggerSourceId24::from_ll<LL_HRTIM_ADCTRIG_SRC24_TIMDPER>();
    inline constexpr HrtimTimerDAdcTriggerSourceId24 HrtimTimerDAdcTriggerSourceId24::kReset = HrtimTimerDAdcTriggerSourceId24::from_ll<LL_HRTIM_ADCTRIG_SRC24_TIMDRST>();


    struct HrtimTimerEAdcTriggerSourceId24 : public LLDriverEnumValue<HrtimTimerEAdcTriggerSourceId24>
    {
        static const HrtimTimerEAdcTriggerSourceId24 kNone;
        static const HrtimTimerEAdcTriggerSourceId24 kExternalEvent6;
        static const HrtimTimerEAdcTriggerSourceId24 kExternalEvent7;
        static const HrtimTimerEAdcTriggerSourceId24 kExternalEvent8;
        static const HrtimTimerEAdcTriggerSourceId24 kExternalEvent9;
        static const HrtimTimerEAdcTriggerSourceId24 kExternalEvent10;
        static const HrtimTimerEAdcTriggerSourceId24 kCompare2;
        static const HrtimTimerEAdcTriggerSourceId24 kCompare3;
        static const HrtimTimerEAdcTriggerSourceId24 kCompare4;
        static const HrtimTimerEAdcTriggerSourceId24 kReset;
    };

    inline constexpr HrtimTimerEAdcTriggerSourceId24 HrtimTimerEAdcTriggerSourceId24::kNone = HrtimTimerEAdcTriggerSourceId24::from_ll<LL_HRTIM_ADCTRIG_SRC24_NONE>();
    inline constexpr HrtimTimerEAdcTriggerSourceId24 HrtimTimerEAdcTriggerSourceId24::kExternalEvent6 = HrtimTimerEAdcTriggerSourceId24::from_ll<LL_HRTIM_ADCTRIG_SRC24_EEV6>();
    inline constexpr HrtimTimerEAdcTriggerSourceId24 HrtimTimerEAdcTriggerSourceId24::kExternalEvent7 = HrtimTimerEAdcTriggerSourceId24::from_ll<LL_HRTIM_ADCTRIG_SRC24_EEV7>();
    inline constexpr HrtimTimerEAdcTriggerSourceId24 HrtimTimerEAdcTriggerSourceId24::kExternalEvent8 = HrtimTimerEAdcTriggerSourceId24::from_ll<LL_HRTIM_ADCTRIG_SRC24_EEV8>();
    inline constexpr HrtimTimerEAdcTriggerSourceId24 HrtimTimerEAdcTriggerSourceId24::kExternalEvent9 = HrtimTimerEAdcTriggerSourceId24::from_ll<LL_HRTIM_ADCTRIG_SRC24_EEV9>();
    inline constexpr HrtimTimerEAdcTriggerSourceId24 HrtimTimerEAdcTriggerSourceId24::kExternalEvent10 = HrtimTimerEAdcTriggerSourceId24::from_ll<LL_HRTIM_ADCTRIG_SRC24_EEV10>();
    inline constexpr HrtimTimerEAdcTriggerSourceId24 HrtimTimerEAdcTriggerSourceId24::kCompare2 = HrtimTimerEAdcTriggerSourceId24::from_ll<LL_HRTIM_ADCTRIG_SRC24_TIMECMP2>();
    inline constexpr HrtimTimerEAdcTriggerSourceId24 HrtimTimerEAdcTriggerSourceId24::kCompare3 = HrtimTimerEAdcTriggerSourceId24::from_ll<LL_HRTIM_ADCTRIG_SRC24_TIMECMP3>();
    inline constexpr HrtimTimerEAdcTriggerSourceId24 HrtimTimerEAdcTriggerSourceId24::kCompare4 = HrtimTimerEAdcTriggerSourceId24::from_ll<LL_HRTIM_ADCTRIG_SRC24_TIMECMP4>();
    inline constexpr HrtimTimerEAdcTriggerSourceId24 HrtimTimerEAdcTriggerSourceId24::kReset = HrtimTimerEAdcTriggerSourceId24::from_ll<LL_HRTIM_ADCTRIG_SRC24_TIMERST>();


    struct HrtimTimerFAdcTriggerSourceId24 : public LLDriverEnumValue<HrtimTimerFAdcTriggerSourceId24>
    {
        static const HrtimTimerFAdcTriggerSourceId24 kNone;
        static const HrtimTimerFAdcTriggerSourceId24 kExternalEvent6;
        static const HrtimTimerFAdcTriggerSourceId24 kExternalEvent7;
        static const HrtimTimerFAdcTriggerSourceId24 kExternalEvent8;
        static const HrtimTimerFAdcTriggerSourceId24 kExternalEvent9;
        static const HrtimTimerFAdcTriggerSourceId24 kExternalEvent10;
        static const HrtimTimerFAdcTriggerSourceId24 kCompare2;
        static const HrtimTimerFAdcTriggerSourceId24 kCompare3;
        static const HrtimTimerFAdcTriggerSourceId24 kCompare4;
        static const HrtimTimerFAdcTriggerSourceId24 kPeriod;
    };

    inline constexpr HrtimTimerFAdcTriggerSourceId24 HrtimTimerFAdcTriggerSourceId24::kNone = HrtimTimerFAdcTriggerSourceId24::from_ll<LL_HRTIM_ADCTRIG_SRC24_NONE>();
    inline constexpr HrtimTimerFAdcTriggerSourceId24 HrtimTimerFAdcTriggerSourceId24::kExternalEvent6 = HrtimTimerFAdcTriggerSourceId24::from_ll<LL_HRTIM_ADCTRIG_SRC24_EEV6>();
    inline constexpr HrtimTimerFAdcTriggerSourceId24 HrtimTimerFAdcTriggerSourceId24::kExternalEvent7 = HrtimTimerFAdcTriggerSourceId24::from_ll<LL_HRTIM_ADCTRIG_SRC24_EEV7>();
    inline constexpr HrtimTimerFAdcTriggerSourceId24 HrtimTimerFAdcTriggerSourceId24::kExternalEvent8 = HrtimTimerFAdcTriggerSourceId24::from_ll<LL_HRTIM_ADCTRIG_SRC24_EEV8>();
    inline constexpr HrtimTimerFAdcTriggerSourceId24 HrtimTimerFAdcTriggerSourceId24::kExternalEvent9 = HrtimTimerFAdcTriggerSourceId24::from_ll<LL_HRTIM_ADCTRIG_SRC24_EEV9>();
    inline constexpr HrtimTimerFAdcTriggerSourceId24 HrtimTimerFAdcTriggerSourceId24::kExternalEvent10 = HrtimTimerFAdcTriggerSourceId24::from_ll<LL_HRTIM_ADCTRIG_SRC24_EEV10>();
    inline constexpr HrtimTimerFAdcTriggerSourceId24 HrtimTimerFAdcTriggerSourceId24::kCompare2 = HrtimTimerFAdcTriggerSourceId24::from_ll<LL_HRTIM_ADCTRIG_SRC24_TIMFCMP2>();
    inline constexpr HrtimTimerFAdcTriggerSourceId24 HrtimTimerFAdcTriggerSourceId24::kCompare3 = HrtimTimerFAdcTriggerSourceId24::from_ll<LL_HRTIM_ADCTRIG_SRC24_TIMFCMP3>();
    inline constexpr HrtimTimerFAdcTriggerSourceId24 HrtimTimerFAdcTriggerSourceId24::kCompare4 = HrtimTimerFAdcTriggerSourceId24::from_ll<LL_HRTIM_ADCTRIG_SRC24_TIMFCMP4>();
    inline constexpr HrtimTimerFAdcTriggerSourceId24 HrtimTimerFAdcTriggerSourceId24::kPeriod = HrtimTimerFAdcTriggerSourceId24::from_ll<LL_HRTIM_ADCTRIG_SRC24_TIMFPER>();


    struct HrtimTimerMasterAdcTriggerSourceId579 : public LLDriverEnumValue<HrtimTimerMasterAdcTriggerSourceId579>
    {
        static const HrtimTimerMasterAdcTriggerSourceId579 kCompare1;
        static const HrtimTimerMasterAdcTriggerSourceId579 kCompare2;
        static const HrtimTimerMasterAdcTriggerSourceId579 kCompare3;
        static const HrtimTimerMasterAdcTriggerSourceId579 kCompare4;
        static const HrtimTimerMasterAdcTriggerSourceId579 kPeriod;
        static const HrtimTimerMasterAdcTriggerSourceId579 kExternalEvent1;
        static const HrtimTimerMasterAdcTriggerSourceId579 kExternalEvent2;
        static const HrtimTimerMasterAdcTriggerSourceId579 kExternalEvent3;
        static const HrtimTimerMasterAdcTriggerSourceId579 kExternalEvent4;
        static const HrtimTimerMasterAdcTriggerSourceId579 kExternalEvent5;
    };

    inline constexpr HrtimTimerMasterAdcTriggerSourceId579 HrtimTimerMasterAdcTriggerSourceId579::kCompare1 = HrtimTimerMasterAdcTriggerSourceId579::from_ll<LL_HRTIM_ADCTRIG_SRC579_MCMP1>();
    inline constexpr HrtimTimerMasterAdcTriggerSourceId579 HrtimTimerMasterAdcTriggerSourceId579::kCompare2 = HrtimTimerMasterAdcTriggerSourceId579::from_ll<LL_HRTIM_ADCTRIG_SRC579_MCMP2>();
    inline constexpr HrtimTimerMasterAdcTriggerSourceId579 HrtimTimerMasterAdcTriggerSourceId579::kCompare3 = HrtimTimerMasterAdcTriggerSourceId579::from_ll<LL_HRTIM_ADCTRIG_SRC579_MCMP3>();
    inline constexpr HrtimTimerMasterAdcTriggerSourceId579 HrtimTimerMasterAdcTriggerSourceId579::kCompare4 = HrtimTimerMasterAdcTriggerSourceId579::from_ll<LL_HRTIM_ADCTRIG_SRC579_MCMP4>();
    inline constexpr HrtimTimerMasterAdcTriggerSourceId579 HrtimTimerMasterAdcTriggerSourceId579::kPeriod = HrtimTimerMasterAdcTriggerSourceId579::from_ll<LL_HRTIM_ADCTRIG_SRC579_MPER>();
    inline constexpr HrtimTimerMasterAdcTriggerSourceId579 HrtimTimerMasterAdcTriggerSourceId579::kExternalEvent1 = HrtimTimerMasterAdcTriggerSourceId579::from_ll<LL_HRTIM_ADCTRIG_SRC579_EEV1>();
    inline constexpr HrtimTimerMasterAdcTriggerSourceId579 HrtimTimerMasterAdcTriggerSourceId579::kExternalEvent2 = HrtimTimerMasterAdcTriggerSourceId579::from_ll<LL_HRTIM_ADCTRIG_SRC579_EEV2>();
    inline constexpr HrtimTimerMasterAdcTriggerSourceId579 HrtimTimerMasterAdcTriggerSourceId579::kExternalEvent3 = HrtimTimerMasterAdcTriggerSourceId579::from_ll<LL_HRTIM_ADCTRIG_SRC579_EEV3>();
    inline constexpr HrtimTimerMasterAdcTriggerSourceId579 HrtimTimerMasterAdcTriggerSourceId579::kExternalEvent4 = HrtimTimerMasterAdcTriggerSourceId579::from_ll<LL_HRTIM_ADCTRIG_SRC579_EEV4>();
    inline constexpr HrtimTimerMasterAdcTriggerSourceId579 HrtimTimerMasterAdcTriggerSourceId579::kExternalEvent5 = HrtimTimerMasterAdcTriggerSourceId579::from_ll<LL_HRTIM_ADCTRIG_SRC579_EEV5>();


    struct HrtimTimerAAdcTriggerSourceId579 : public LLDriverEnumValue<HrtimTimerAAdcTriggerSourceId579>
    {
        static const HrtimTimerAAdcTriggerSourceId579 kExternalEvent1;
        static const HrtimTimerAAdcTriggerSourceId579 kExternalEvent2;
        static const HrtimTimerAAdcTriggerSourceId579 kExternalEvent3;
        static const HrtimTimerAAdcTriggerSourceId579 kExternalEvent4;
        static const HrtimTimerAAdcTriggerSourceId579 kExternalEvent5;
        static const HrtimTimerAAdcTriggerSourceId579 kCompare3;
        static const HrtimTimerAAdcTriggerSourceId579 kCompare4;
        static const HrtimTimerAAdcTriggerSourceId579 kPeriod;
        static const HrtimTimerAAdcTriggerSourceId579 kReset;
    };

    inline constexpr HrtimTimerAAdcTriggerSourceId579 HrtimTimerAAdcTriggerSourceId579::kExternalEvent1 = HrtimTimerAAdcTriggerSourceId579::from_ll<LL_HRTIM_ADCTRIG_SRC579_EEV1>();
    inline constexpr HrtimTimerAAdcTriggerSourceId579 HrtimTimerAAdcTriggerSourceId579::kExternalEvent2 = HrtimTimerAAdcTriggerSourceId579::from_ll<LL_HRTIM_ADCTRIG_SRC579_EEV2>();
    inline constexpr HrtimTimerAAdcTriggerSourceId579 HrtimTimerAAdcTriggerSourceId579::kExternalEvent3 = HrtimTimerAAdcTriggerSourceId579::from_ll<LL_HRTIM_ADCTRIG_SRC579_EEV3>();
    inline constexpr HrtimTimerAAdcTriggerSourceId579 HrtimTimerAAdcTriggerSourceId579::kExternalEvent4 = HrtimTimerAAdcTriggerSourceId579::from_ll<LL_HRTIM_ADCTRIG_SRC579_EEV4>();
    inline constexpr HrtimTimerAAdcTriggerSourceId579 HrtimTimerAAdcTriggerSourceId579::kExternalEvent5 = HrtimTimerAAdcTriggerSourceId579::from_ll<LL_HRTIM_ADCTRIG_SRC579_EEV5>();
    inline constexpr HrtimTimerAAdcTriggerSourceId579 HrtimTimerAAdcTriggerSourceId579::kCompare3 = HrtimTimerAAdcTriggerSourceId579::from_ll<LL_HRTIM_ADCTRIG_SRC579_TIMA_CMP3>();
    inline constexpr HrtimTimerAAdcTriggerSourceId579 HrtimTimerAAdcTriggerSourceId579::kCompare4 = HrtimTimerAAdcTriggerSourceId579::from_ll<LL_HRTIM_ADCTRIG_SRC579_TIMA_CMP4>();
    inline constexpr HrtimTimerAAdcTriggerSourceId579 HrtimTimerAAdcTriggerSourceId579::kPeriod = HrtimTimerAAdcTriggerSourceId579::from_ll<LL_HRTIM_ADCTRIG_SRC579_TIMA_PER>();
    inline constexpr HrtimTimerAAdcTriggerSourceId579 HrtimTimerAAdcTriggerSourceId579::kReset = HrtimTimerAAdcTriggerSourceId579::from_ll<LL_HRTIM_ADCTRIG_SRC579_TIMA_RST>();


    struct HrtimTimerBAdcTriggerSourceId579 : public LLDriverEnumValue<HrtimTimerBAdcTriggerSourceId579>
    {
        static const HrtimTimerBAdcTriggerSourceId579 kExternalEvent1;
        static const HrtimTimerBAdcTriggerSourceId579 kExternalEvent2;
        static const HrtimTimerBAdcTriggerSourceId579 kExternalEvent3;
        static const HrtimTimerBAdcTriggerSourceId579 kExternalEvent4;
        static const HrtimTimerBAdcTriggerSourceId579 kExternalEvent5;
        static const HrtimTimerBAdcTriggerSourceId579 kCompare3;
        static const HrtimTimerBAdcTriggerSourceId579 kCompare4;
        static const HrtimTimerBAdcTriggerSourceId579 kPeriod;
        static const HrtimTimerBAdcTriggerSourceId579 kReset;
    };

    inline constexpr HrtimTimerBAdcTriggerSourceId579 HrtimTimerBAdcTriggerSourceId579::kExternalEvent1 = HrtimTimerBAdcTriggerSourceId579::from_ll<LL_HRTIM_ADCTRIG_SRC579_EEV1>();
    inline constexpr HrtimTimerBAdcTriggerSourceId579 HrtimTimerBAdcTriggerSourceId579::kExternalEvent2 = HrtimTimerBAdcTriggerSourceId579::from_ll<LL_HRTIM_ADCTRIG_SRC579_EEV2>();
    inline constexpr HrtimTimerBAdcTriggerSourceId579 HrtimTimerBAdcTriggerSourceId579::kExternalEvent3 = HrtimTimerBAdcTriggerSourceId579::from_ll<LL_HRTIM_ADCTRIG_SRC579_EEV3>();
    inline constexpr HrtimTimerBAdcTriggerSourceId579 HrtimTimerBAdcTriggerSourceId579::kExternalEvent4 = HrtimTimerBAdcTriggerSourceId579::from_ll<LL_HRTIM_ADCTRIG_SRC579_EEV4>();
    inline constexpr HrtimTimerBAdcTriggerSourceId579 HrtimTimerBAdcTriggerSourceId579::kExternalEvent5 = HrtimTimerBAdcTriggerSourceId579::from_ll<LL_HRTIM_ADCTRIG_SRC579_EEV5>();
    inline constexpr HrtimTimerBAdcTriggerSourceId579 HrtimTimerBAdcTriggerSourceId579::kCompare3 = HrtimTimerBAdcTriggerSourceId579::from_ll<LL_HRTIM_ADCTRIG_SRC579_TIMB_CMP3>();
    inline constexpr HrtimTimerBAdcTriggerSourceId579 HrtimTimerBAdcTriggerSourceId579::kCompare4 = HrtimTimerBAdcTriggerSourceId579::from_ll<LL_HRTIM_ADCTRIG_SRC579_TIMB_CMP4>();
    inline constexpr HrtimTimerBAdcTriggerSourceId579 HrtimTimerBAdcTriggerSourceId579::kPeriod = HrtimTimerBAdcTriggerSourceId579::from_ll<LL_HRTIM_ADCTRIG_SRC579_TIMB_PER>();
    inline constexpr HrtimTimerBAdcTriggerSourceId579 HrtimTimerBAdcTriggerSourceId579::kReset = HrtimTimerBAdcTriggerSourceId579::from_ll<LL_HRTIM_ADCTRIG_SRC579_TIMB_RST>();


    struct HrtimTimerCAdcTriggerSourceId579 : public LLDriverEnumValue<HrtimTimerCAdcTriggerSourceId579>
    {
        static const HrtimTimerCAdcTriggerSourceId579 kExternalEvent1;
        static const HrtimTimerCAdcTriggerSourceId579 kExternalEvent2;
        static const HrtimTimerCAdcTriggerSourceId579 kExternalEvent3;
        static const HrtimTimerCAdcTriggerSourceId579 kExternalEvent4;
        static const HrtimTimerCAdcTriggerSourceId579 kExternalEvent5;
        static const HrtimTimerCAdcTriggerSourceId579 kCompare3;
        static const HrtimTimerCAdcTriggerSourceId579 kCompare4;
        static const HrtimTimerCAdcTriggerSourceId579 kPeriod;
    };

    inline constexpr HrtimTimerCAdcTriggerSourceId579 HrtimTimerCAdcTriggerSourceId579::kExternalEvent1 = HrtimTimerCAdcTriggerSourceId579::from_ll<LL_HRTIM_ADCTRIG_SRC579_EEV1>();
    inline constexpr HrtimTimerCAdcTriggerSourceId579 HrtimTimerCAdcTriggerSourceId579::kExternalEvent2 = HrtimTimerCAdcTriggerSourceId579::from_ll<LL_HRTIM_ADCTRIG_SRC579_EEV2>();
    inline constexpr HrtimTimerCAdcTriggerSourceId579 HrtimTimerCAdcTriggerSourceId579::kExternalEvent3 = HrtimTimerCAdcTriggerSourceId579::from_ll<LL_HRTIM_ADCTRIG_SRC579_EEV3>();
    inline constexpr HrtimTimerCAdcTriggerSourceId579 HrtimTimerCAdcTriggerSourceId579::kExternalEvent4 = HrtimTimerCAdcTriggerSourceId579::from_ll<LL_HRTIM_ADCTRIG_SRC579_EEV4>();
    inline constexpr HrtimTimerCAdcTriggerSourceId579 HrtimTimerCAdcTriggerSourceId579::kExternalEvent5 = HrtimTimerCAdcTriggerSourceId579::from_ll<LL_HRTIM_ADCTRIG_SRC579_EEV5>();
    inline constexpr HrtimTimerCAdcTriggerSourceId579 HrtimTimerCAdcTriggerSourceId579::kCompare3 = HrtimTimerCAdcTriggerSourceId579::from_ll<LL_HRTIM_ADCTRIG_SRC579_TIMC_CMP3>();
    inline constexpr HrtimTimerCAdcTriggerSourceId579 HrtimTimerCAdcTriggerSourceId579::kCompare4 = HrtimTimerCAdcTriggerSourceId579::from_ll<LL_HRTIM_ADCTRIG_SRC579_TIMC_CMP4>();
    inline constexpr HrtimTimerCAdcTriggerSourceId579 HrtimTimerCAdcTriggerSourceId579::kPeriod = HrtimTimerCAdcTriggerSourceId579::from_ll<LL_HRTIM_ADCTRIG_SRC579_TIMC_PER>();


    struct HrtimTimerDAdcTriggerSourceId579 : public LLDriverEnumValue<HrtimTimerDAdcTriggerSourceId579>
    {
        static const HrtimTimerDAdcTriggerSourceId579 kExternalEvent1;
        static const HrtimTimerDAdcTriggerSourceId579 kExternalEvent2;
        static const HrtimTimerDAdcTriggerSourceId579 kExternalEvent3;
        static const HrtimTimerDAdcTriggerSourceId579 kExternalEvent4;
        static const HrtimTimerDAdcTriggerSourceId579 kExternalEvent5;
        static const HrtimTimerDAdcTriggerSourceId579 kCompare3;
        static const HrtimTimerDAdcTriggerSourceId579 kCompare4;
        static const HrtimTimerDAdcTriggerSourceId579 kPeriod;
    };

    inline constexpr HrtimTimerDAdcTriggerSourceId579 HrtimTimerDAdcTriggerSourceId579::kExternalEvent1 = HrtimTimerDAdcTriggerSourceId579::from_ll<LL_HRTIM_ADCTRIG_SRC579_EEV1>();
    inline constexpr HrtimTimerDAdcTriggerSourceId579 HrtimTimerDAdcTriggerSourceId579::kExternalEvent2 = HrtimTimerDAdcTriggerSourceId579::from_ll<LL_HRTIM_ADCTRIG_SRC579_EEV2>();
    inline constexpr HrtimTimerDAdcTriggerSourceId579 HrtimTimerDAdcTriggerSourceId579::kExternalEvent3 = HrtimTimerDAdcTriggerSourceId579::from_ll<LL_HRTIM_ADCTRIG_SRC579_EEV3>();
    inline constexpr HrtimTimerDAdcTriggerSourceId579 HrtimTimerDAdcTriggerSourceId579::kExternalEvent4 = HrtimTimerDAdcTriggerSourceId579::from_ll<LL_HRTIM_ADCTRIG_SRC579_EEV4>();
    inline constexpr HrtimTimerDAdcTriggerSourceId579 HrtimTimerDAdcTriggerSourceId579::kExternalEvent5 = HrtimTimerDAdcTriggerSourceId579::from_ll<LL_HRTIM_ADCTRIG_SRC579_EEV5>();
    inline constexpr HrtimTimerDAdcTriggerSourceId579 HrtimTimerDAdcTriggerSourceId579::kCompare3 = HrtimTimerDAdcTriggerSourceId579::from_ll<LL_HRTIM_ADCTRIG_SRC579_TIMD_CMP3>();
    inline constexpr HrtimTimerDAdcTriggerSourceId579 HrtimTimerDAdcTriggerSourceId579::kCompare4 = HrtimTimerDAdcTriggerSourceId579::from_ll<LL_HRTIM_ADCTRIG_SRC579_TIMD_CMP4>();
    inline constexpr HrtimTimerDAdcTriggerSourceId579 HrtimTimerDAdcTriggerSourceId579::kPeriod = HrtimTimerDAdcTriggerSourceId579::from_ll<LL_HRTIM_ADCTRIG_SRC579_TIMD_PER>();


    struct HrtimTimerEAdcTriggerSourceId579 : public LLDriverEnumValue<HrtimTimerEAdcTriggerSourceId579>
    {
        static const HrtimTimerEAdcTriggerSourceId579 kExternalEvent1;
        static const HrtimTimerEAdcTriggerSourceId579 kExternalEvent2;
        static const HrtimTimerEAdcTriggerSourceId579 kExternalEvent3;
        static const HrtimTimerEAdcTriggerSourceId579 kExternalEvent4;
        static const HrtimTimerEAdcTriggerSourceId579 kExternalEvent5;
        static const HrtimTimerEAdcTriggerSourceId579 kCompare3;
        static const HrtimTimerEAdcTriggerSourceId579 kCompare4;
        static const HrtimTimerEAdcTriggerSourceId579 kPeriod;
    };

    inline constexpr HrtimTimerEAdcTriggerSourceId579 HrtimTimerEAdcTriggerSourceId579::kExternalEvent1 = HrtimTimerEAdcTriggerSourceId579::from_ll<LL_HRTIM_ADCTRIG_SRC579_EEV1>();
    inline constexpr HrtimTimerEAdcTriggerSourceId579 HrtimTimerEAdcTriggerSourceId579::kExternalEvent2 = HrtimTimerEAdcTriggerSourceId579::from_ll<LL_HRTIM_ADCTRIG_SRC579_EEV2>();
    inline constexpr HrtimTimerEAdcTriggerSourceId579 HrtimTimerEAdcTriggerSourceId579::kExternalEvent3 = HrtimTimerEAdcTriggerSourceId579::from_ll<LL_HRTIM_ADCTRIG_SRC579_EEV3>();
    inline constexpr HrtimTimerEAdcTriggerSourceId579 HrtimTimerEAdcTriggerSourceId579::kExternalEvent4 = HrtimTimerEAdcTriggerSourceId579::from_ll<LL_HRTIM_ADCTRIG_SRC579_EEV4>();
    inline constexpr HrtimTimerEAdcTriggerSourceId579 HrtimTimerEAdcTriggerSourceId579::kExternalEvent5 = HrtimTimerEAdcTriggerSourceId579::from_ll<LL_HRTIM_ADCTRIG_SRC579_EEV5>();
    inline constexpr HrtimTimerEAdcTriggerSourceId579 HrtimTimerEAdcTriggerSourceId579::kCompare3 = HrtimTimerEAdcTriggerSourceId579::from_ll<LL_HRTIM_ADCTRIG_SRC579_TIME_CMP3>();
    inline constexpr HrtimTimerEAdcTriggerSourceId579 HrtimTimerEAdcTriggerSourceId579::kCompare4 = HrtimTimerEAdcTriggerSourceId579::from_ll<LL_HRTIM_ADCTRIG_SRC579_TIME_CMP4>();
    inline constexpr HrtimTimerEAdcTriggerSourceId579 HrtimTimerEAdcTriggerSourceId579::kPeriod = HrtimTimerEAdcTriggerSourceId579::from_ll<LL_HRTIM_ADCTRIG_SRC579_TIME_PER>();


    struct HrtimTimerFAdcTriggerSourceId579 : public LLDriverEnumValue<HrtimTimerFAdcTriggerSourceId579>
    {
        static const HrtimTimerFAdcTriggerSourceId579 kExternalEvent1;
        static const HrtimTimerFAdcTriggerSourceId579 kExternalEvent2;
        static const HrtimTimerFAdcTriggerSourceId579 kExternalEvent3;
        static const HrtimTimerFAdcTriggerSourceId579 kExternalEvent4;
        static const HrtimTimerFAdcTriggerSourceId579 kExternalEvent5;
        static const HrtimTimerFAdcTriggerSourceId579 kCompare2;
        static const HrtimTimerFAdcTriggerSourceId579 kCompare3;
        static const HrtimTimerFAdcTriggerSourceId579 kCompare4;
        static const HrtimTimerFAdcTriggerSourceId579 kPeriod;
        static const HrtimTimerFAdcTriggerSourceId579 kReset;
    };

    inline constexpr HrtimTimerFAdcTriggerSourceId579 HrtimTimerFAdcTriggerSourceId579::kExternalEvent1 = HrtimTimerFAdcTriggerSourceId579::from_ll<LL_HRTIM_ADCTRIG_SRC579_EEV1>();
    inline constexpr HrtimTimerFAdcTriggerSourceId579 HrtimTimerFAdcTriggerSourceId579::kExternalEvent2 = HrtimTimerFAdcTriggerSourceId579::from_ll<LL_HRTIM_ADCTRIG_SRC579_EEV2>();
    inline constexpr HrtimTimerFAdcTriggerSourceId579 HrtimTimerFAdcTriggerSourceId579::kExternalEvent3 = HrtimTimerFAdcTriggerSourceId579::from_ll<LL_HRTIM_ADCTRIG_SRC579_EEV3>();
    inline constexpr HrtimTimerFAdcTriggerSourceId579 HrtimTimerFAdcTriggerSourceId579::kExternalEvent4 = HrtimTimerFAdcTriggerSourceId579::from_ll<LL_HRTIM_ADCTRIG_SRC579_EEV4>();
    inline constexpr HrtimTimerFAdcTriggerSourceId579 HrtimTimerFAdcTriggerSourceId579::kExternalEvent5 = HrtimTimerFAdcTriggerSourceId579::from_ll<LL_HRTIM_ADCTRIG_SRC579_EEV5>();
    inline constexpr HrtimTimerFAdcTriggerSourceId579 HrtimTimerFAdcTriggerSourceId579::kCompare2 = HrtimTimerFAdcTriggerSourceId579::from_ll<LL_HRTIM_ADCTRIG_SRC579_TIMF_CMP2>();
    inline constexpr HrtimTimerFAdcTriggerSourceId579 HrtimTimerFAdcTriggerSourceId579::kCompare3 = HrtimTimerFAdcTriggerSourceId579::from_ll<LL_HRTIM_ADCTRIG_SRC579_TIMF_CMP3>();
    inline constexpr HrtimTimerFAdcTriggerSourceId579 HrtimTimerFAdcTriggerSourceId579::kCompare4 = HrtimTimerFAdcTriggerSourceId579::from_ll<LL_HRTIM_ADCTRIG_SRC579_TIMF_CMP4>();
    inline constexpr HrtimTimerFAdcTriggerSourceId579 HrtimTimerFAdcTriggerSourceId579::kPeriod = HrtimTimerFAdcTriggerSourceId579::from_ll<LL_HRTIM_ADCTRIG_SRC579_TIMF_PER>();
    inline constexpr HrtimTimerFAdcTriggerSourceId579 HrtimTimerFAdcTriggerSourceId579::kReset = HrtimTimerFAdcTriggerSourceId579::from_ll<LL_HRTIM_ADCTRIG_SRC579_TIMF_RST>();


    struct HrtimTimerMasterAdcTriggerSourceId6810 : public LLDriverEnumValue<HrtimTimerMasterAdcTriggerSourceId6810>
    {
        static const HrtimTimerMasterAdcTriggerSourceId6810 kMasterCompare1;
        static const HrtimTimerMasterAdcTriggerSourceId6810 kMasterCompare2;
        static const HrtimTimerMasterAdcTriggerSourceId6810 kMasterCompare3;
        static const HrtimTimerMasterAdcTriggerSourceId6810 kMasterCompare4;
        static const HrtimTimerMasterAdcTriggerSourceId6810 kMasterPeriod;
        static const HrtimTimerMasterAdcTriggerSourceId6810 kExternalEvent6;
        static const HrtimTimerMasterAdcTriggerSourceId6810 kExternalEvent7;
        static const HrtimTimerMasterAdcTriggerSourceId6810 kExternalEvent8;
        static const HrtimTimerMasterAdcTriggerSourceId6810 kExternalEvent9;
        static const HrtimTimerMasterAdcTriggerSourceId6810 kExternalEvent10;
    };

    inline constexpr HrtimTimerMasterAdcTriggerSourceId6810 HrtimTimerMasterAdcTriggerSourceId6810::kMasterCompare1 = HrtimTimerMasterAdcTriggerSourceId6810::from_ll<LL_HRTIM_ADCTRIG_SRC6810_MCMP1>();
    inline constexpr HrtimTimerMasterAdcTriggerSourceId6810 HrtimTimerMasterAdcTriggerSourceId6810::kMasterCompare2 = HrtimTimerMasterAdcTriggerSourceId6810::from_ll<LL_HRTIM_ADCTRIG_SRC6810_MCMP2>();
    inline constexpr HrtimTimerMasterAdcTriggerSourceId6810 HrtimTimerMasterAdcTriggerSourceId6810::kMasterCompare3 = HrtimTimerMasterAdcTriggerSourceId6810::from_ll<LL_HRTIM_ADCTRIG_SRC6810_MCMP3>();
    inline constexpr HrtimTimerMasterAdcTriggerSourceId6810 HrtimTimerMasterAdcTriggerSourceId6810::kMasterCompare4 = HrtimTimerMasterAdcTriggerSourceId6810::from_ll<LL_HRTIM_ADCTRIG_SRC6810_MCMP4>();
    inline constexpr HrtimTimerMasterAdcTriggerSourceId6810 HrtimTimerMasterAdcTriggerSourceId6810::kMasterPeriod = HrtimTimerMasterAdcTriggerSourceId6810::from_ll<LL_HRTIM_ADCTRIG_SRC6810_MPER>();
    inline constexpr HrtimTimerMasterAdcTriggerSourceId6810 HrtimTimerMasterAdcTriggerSourceId6810::kExternalEvent6 = HrtimTimerMasterAdcTriggerSourceId6810::from_ll<LL_HRTIM_ADCTRIG_SRC6810_EEV6>();
    inline constexpr HrtimTimerMasterAdcTriggerSourceId6810 HrtimTimerMasterAdcTriggerSourceId6810::kExternalEvent7 = HrtimTimerMasterAdcTriggerSourceId6810::from_ll<LL_HRTIM_ADCTRIG_SRC6810_EEV7>();
    inline constexpr HrtimTimerMasterAdcTriggerSourceId6810 HrtimTimerMasterAdcTriggerSourceId6810::kExternalEvent8 = HrtimTimerMasterAdcTriggerSourceId6810::from_ll<LL_HRTIM_ADCTRIG_SRC6810_EEV8>();
    inline constexpr HrtimTimerMasterAdcTriggerSourceId6810 HrtimTimerMasterAdcTriggerSourceId6810::kExternalEvent9 = HrtimTimerMasterAdcTriggerSourceId6810::from_ll<LL_HRTIM_ADCTRIG_SRC6810_EEV9>();
    inline constexpr HrtimTimerMasterAdcTriggerSourceId6810 HrtimTimerMasterAdcTriggerSourceId6810::kExternalEvent10 = HrtimTimerMasterAdcTriggerSourceId6810::from_ll<LL_HRTIM_ADCTRIG_SRC6810_EEV10>();


    struct HrtimTimerAAdcTriggerSourceId6810 : public LLDriverEnumValue<HrtimTimerAAdcTriggerSourceId6810>
    {
        static const HrtimTimerAAdcTriggerSourceId6810 kExternalEvent6;
        static const HrtimTimerAAdcTriggerSourceId6810 kExternalEvent7;
        static const HrtimTimerAAdcTriggerSourceId6810 kExternalEvent8;
        static const HrtimTimerAAdcTriggerSourceId6810 kExternalEvent9;
        static const HrtimTimerAAdcTriggerSourceId6810 kExternalEvent10;
        static const HrtimTimerAAdcTriggerSourceId6810 kCompare2;
        static const HrtimTimerAAdcTriggerSourceId6810 kCompare4;
        static const HrtimTimerAAdcTriggerSourceId6810 kPeriod;
    };

    inline constexpr HrtimTimerAAdcTriggerSourceId6810 HrtimTimerAAdcTriggerSourceId6810::kExternalEvent6 = HrtimTimerAAdcTriggerSourceId6810::from_ll<LL_HRTIM_ADCTRIG_SRC6810_EEV6>();
    inline constexpr HrtimTimerAAdcTriggerSourceId6810 HrtimTimerAAdcTriggerSourceId6810::kExternalEvent7 = HrtimTimerAAdcTriggerSourceId6810::from_ll<LL_HRTIM_ADCTRIG_SRC6810_EEV7>();
    inline constexpr HrtimTimerAAdcTriggerSourceId6810 HrtimTimerAAdcTriggerSourceId6810::kExternalEvent8 = HrtimTimerAAdcTriggerSourceId6810::from_ll<LL_HRTIM_ADCTRIG_SRC6810_EEV8>();
    inline constexpr HrtimTimerAAdcTriggerSourceId6810 HrtimTimerAAdcTriggerSourceId6810::kExternalEvent9 = HrtimTimerAAdcTriggerSourceId6810::from_ll<LL_HRTIM_ADCTRIG_SRC6810_EEV9>();
    inline constexpr HrtimTimerAAdcTriggerSourceId6810 HrtimTimerAAdcTriggerSourceId6810::kExternalEvent10 = HrtimTimerAAdcTriggerSourceId6810::from_ll<LL_HRTIM_ADCTRIG_SRC6810_EEV10>();
    inline constexpr HrtimTimerAAdcTriggerSourceId6810 HrtimTimerAAdcTriggerSourceId6810::kCompare2 = HrtimTimerAAdcTriggerSourceId6810::from_ll<LL_HRTIM_ADCTRIG_SRC6810_TIMA_CMP2>();
    inline constexpr HrtimTimerAAdcTriggerSourceId6810 HrtimTimerAAdcTriggerSourceId6810::kCompare4 = HrtimTimerAAdcTriggerSourceId6810::from_ll<LL_HRTIM_ADCTRIG_SRC6810_TIMA_CMP4>();
    inline constexpr HrtimTimerAAdcTriggerSourceId6810 HrtimTimerAAdcTriggerSourceId6810::kPeriod = HrtimTimerAAdcTriggerSourceId6810::from_ll<LL_HRTIM_ADCTRIG_SRC6810_TIMA_PER>();


    struct HrtimTimerBAdcTriggerSourceId6810 : public LLDriverEnumValue<HrtimTimerBAdcTriggerSourceId6810>
    {
        static const HrtimTimerBAdcTriggerSourceId6810 kExternalEvent6;
        static const HrtimTimerBAdcTriggerSourceId6810 kExternalEvent7;
        static const HrtimTimerBAdcTriggerSourceId6810 kExternalEvent8;
        static const HrtimTimerBAdcTriggerSourceId6810 kExternalEvent9;
        static const HrtimTimerBAdcTriggerSourceId6810 kExternalEvent10;
        static const HrtimTimerBAdcTriggerSourceId6810 kCompare2;
        static const HrtimTimerBAdcTriggerSourceId6810 kCompare4;
        static const HrtimTimerBAdcTriggerSourceId6810 kPeriod;
    };

    inline constexpr HrtimTimerBAdcTriggerSourceId6810 HrtimTimerBAdcTriggerSourceId6810::kExternalEvent6 = HrtimTimerBAdcTriggerSourceId6810::from_ll<LL_HRTIM_ADCTRIG_SRC6810_EEV6>();
    inline constexpr HrtimTimerBAdcTriggerSourceId6810 HrtimTimerBAdcTriggerSourceId6810::kExternalEvent7 = HrtimTimerBAdcTriggerSourceId6810::from_ll<LL_HRTIM_ADCTRIG_SRC6810_EEV7>();
    inline constexpr HrtimTimerBAdcTriggerSourceId6810 HrtimTimerBAdcTriggerSourceId6810::kExternalEvent8 = HrtimTimerBAdcTriggerSourceId6810::from_ll<LL_HRTIM_ADCTRIG_SRC6810_EEV8>();
    inline constexpr HrtimTimerBAdcTriggerSourceId6810 HrtimTimerBAdcTriggerSourceId6810::kExternalEvent9 = HrtimTimerBAdcTriggerSourceId6810::from_ll<LL_HRTIM_ADCTRIG_SRC6810_EEV9>();
    inline constexpr HrtimTimerBAdcTriggerSourceId6810 HrtimTimerBAdcTriggerSourceId6810::kExternalEvent10 = HrtimTimerBAdcTriggerSourceId6810::from_ll<LL_HRTIM_ADCTRIG_SRC6810_EEV10>();
    inline constexpr HrtimTimerBAdcTriggerSourceId6810 HrtimTimerBAdcTriggerSourceId6810::kCompare2 = HrtimTimerBAdcTriggerSourceId6810::from_ll<LL_HRTIM_ADCTRIG_SRC6810_TIMB_CMP2>();
    inline constexpr HrtimTimerBAdcTriggerSourceId6810 HrtimTimerBAdcTriggerSourceId6810::kCompare4 = HrtimTimerBAdcTriggerSourceId6810::from_ll<LL_HRTIM_ADCTRIG_SRC6810_TIMB_CMP4>();
    inline constexpr HrtimTimerBAdcTriggerSourceId6810 HrtimTimerBAdcTriggerSourceId6810::kPeriod = HrtimTimerBAdcTriggerSourceId6810::from_ll<LL_HRTIM_ADCTRIG_SRC6810_TIMB_PER>();


    struct HrtimTimerCAdcTriggerSourceId6810 : public LLDriverEnumValue<HrtimTimerCAdcTriggerSourceId6810>
    {
        static const HrtimTimerCAdcTriggerSourceId6810 kExternalEvent6;
        static const HrtimTimerCAdcTriggerSourceId6810 kExternalEvent7;
        static const HrtimTimerCAdcTriggerSourceId6810 kExternalEvent8;
        static const HrtimTimerCAdcTriggerSourceId6810 kExternalEvent9;
        static const HrtimTimerCAdcTriggerSourceId6810 kExternalEvent10;
        static const HrtimTimerCAdcTriggerSourceId6810 kCompare2;
        static const HrtimTimerCAdcTriggerSourceId6810 kCompare4;
        static const HrtimTimerCAdcTriggerSourceId6810 kPeriod;
        static const HrtimTimerCAdcTriggerSourceId6810 kReset;
    };

    inline constexpr HrtimTimerCAdcTriggerSourceId6810 HrtimTimerCAdcTriggerSourceId6810::kExternalEvent6 = HrtimTimerCAdcTriggerSourceId6810::from_ll<LL_HRTIM_ADCTRIG_SRC6810_EEV6>();
    inline constexpr HrtimTimerCAdcTriggerSourceId6810 HrtimTimerCAdcTriggerSourceId6810::kExternalEvent7 = HrtimTimerCAdcTriggerSourceId6810::from_ll<LL_HRTIM_ADCTRIG_SRC6810_EEV7>();
    inline constexpr HrtimTimerCAdcTriggerSourceId6810 HrtimTimerCAdcTriggerSourceId6810::kExternalEvent8 = HrtimTimerCAdcTriggerSourceId6810::from_ll<LL_HRTIM_ADCTRIG_SRC6810_EEV8>();
    inline constexpr HrtimTimerCAdcTriggerSourceId6810 HrtimTimerCAdcTriggerSourceId6810::kExternalEvent9 = HrtimTimerCAdcTriggerSourceId6810::from_ll<LL_HRTIM_ADCTRIG_SRC6810_EEV9>();
    inline constexpr HrtimTimerCAdcTriggerSourceId6810 HrtimTimerCAdcTriggerSourceId6810::kExternalEvent10 = HrtimTimerCAdcTriggerSourceId6810::from_ll<LL_HRTIM_ADCTRIG_SRC6810_EEV10>();
    inline constexpr HrtimTimerCAdcTriggerSourceId6810 HrtimTimerCAdcTriggerSourceId6810::kCompare2 = HrtimTimerCAdcTriggerSourceId6810::from_ll<LL_HRTIM_ADCTRIG_SRC6810_TIMC_CMP2>();
    inline constexpr HrtimTimerCAdcTriggerSourceId6810 HrtimTimerCAdcTriggerSourceId6810::kCompare4 = HrtimTimerCAdcTriggerSourceId6810::from_ll<LL_HRTIM_ADCTRIG_SRC6810_TIMC_CMP4>();
    inline constexpr HrtimTimerCAdcTriggerSourceId6810 HrtimTimerCAdcTriggerSourceId6810::kPeriod = HrtimTimerCAdcTriggerSourceId6810::from_ll<LL_HRTIM_ADCTRIG_SRC6810_TIMC_PER>();
    inline constexpr HrtimTimerCAdcTriggerSourceId6810 HrtimTimerCAdcTriggerSourceId6810::kReset = HrtimTimerCAdcTriggerSourceId6810::from_ll<LL_HRTIM_ADCTRIG_SRC6810_TIMC_RST>();


    struct HrtimTimerDAdcTriggerSourceId6810 : public LLDriverEnumValue<HrtimTimerDAdcTriggerSourceId6810>
    {
        static const HrtimTimerDAdcTriggerSourceId6810 kExternalEvent6;
        static const HrtimTimerDAdcTriggerSourceId6810 kExternalEvent7;
        static const HrtimTimerDAdcTriggerSourceId6810 kExternalEvent8;
        static const HrtimTimerDAdcTriggerSourceId6810 kExternalEvent9;
        static const HrtimTimerDAdcTriggerSourceId6810 kExternalEvent10;
        static const HrtimTimerDAdcTriggerSourceId6810 kCompare2;
        static const HrtimTimerDAdcTriggerSourceId6810 kCompare4;
        static const HrtimTimerDAdcTriggerSourceId6810 kPeriod;
        static const HrtimTimerDAdcTriggerSourceId6810 kReset;
    };

    inline constexpr HrtimTimerDAdcTriggerSourceId6810 HrtimTimerDAdcTriggerSourceId6810::kExternalEvent6 = HrtimTimerDAdcTriggerSourceId6810::from_ll<LL_HRTIM_ADCTRIG_SRC6810_EEV6>();
    inline constexpr HrtimTimerDAdcTriggerSourceId6810 HrtimTimerDAdcTriggerSourceId6810::kExternalEvent7 = HrtimTimerDAdcTriggerSourceId6810::from_ll<LL_HRTIM_ADCTRIG_SRC6810_EEV7>();
    inline constexpr HrtimTimerDAdcTriggerSourceId6810 HrtimTimerDAdcTriggerSourceId6810::kExternalEvent8 = HrtimTimerDAdcTriggerSourceId6810::from_ll<LL_HRTIM_ADCTRIG_SRC6810_EEV8>();
    inline constexpr HrtimTimerDAdcTriggerSourceId6810 HrtimTimerDAdcTriggerSourceId6810::kExternalEvent9 = HrtimTimerDAdcTriggerSourceId6810::from_ll<LL_HRTIM_ADCTRIG_SRC6810_EEV9>();
    inline constexpr HrtimTimerDAdcTriggerSourceId6810 HrtimTimerDAdcTriggerSourceId6810::kExternalEvent10 = HrtimTimerDAdcTriggerSourceId6810::from_ll<LL_HRTIM_ADCTRIG_SRC6810_EEV10>();
    inline constexpr HrtimTimerDAdcTriggerSourceId6810 HrtimTimerDAdcTriggerSourceId6810::kCompare2 = HrtimTimerDAdcTriggerSourceId6810::from_ll<LL_HRTIM_ADCTRIG_SRC6810_TIMD_CMP2>();
    inline constexpr HrtimTimerDAdcTriggerSourceId6810 HrtimTimerDAdcTriggerSourceId6810::kCompare4 = HrtimTimerDAdcTriggerSourceId6810::from_ll<LL_HRTIM_ADCTRIG_SRC6810_TIMD_CMP4>();
    inline constexpr HrtimTimerDAdcTriggerSourceId6810 HrtimTimerDAdcTriggerSourceId6810::kPeriod = HrtimTimerDAdcTriggerSourceId6810::from_ll<LL_HRTIM_ADCTRIG_SRC6810_TIMD_PER>();
    inline constexpr HrtimTimerDAdcTriggerSourceId6810 HrtimTimerDAdcTriggerSourceId6810::kReset = HrtimTimerDAdcTriggerSourceId6810::from_ll<LL_HRTIM_ADCTRIG_SRC6810_TIMD_RST>();


    struct HrtimTimerEAdcTriggerSourceId6810 : public LLDriverEnumValue<HrtimTimerEAdcTriggerSourceId6810>
    {
        static const HrtimTimerEAdcTriggerSourceId6810 kExternalEvent6;
        static const HrtimTimerEAdcTriggerSourceId6810 kExternalEvent7;
        static const HrtimTimerEAdcTriggerSourceId6810 kExternalEvent8;
        static const HrtimTimerEAdcTriggerSourceId6810 kExternalEvent9;
        static const HrtimTimerEAdcTriggerSourceId6810 kExternalEvent10;
        static const HrtimTimerEAdcTriggerSourceId6810 kCompare2;
        static const HrtimTimerEAdcTriggerSourceId6810 kCompare3;
        static const HrtimTimerEAdcTriggerSourceId6810 kCompare4;
        static const HrtimTimerEAdcTriggerSourceId6810 kReset;
    };

    inline constexpr HrtimTimerEAdcTriggerSourceId6810 HrtimTimerEAdcTriggerSourceId6810::kExternalEvent6 = HrtimTimerEAdcTriggerSourceId6810::from_ll<LL_HRTIM_ADCTRIG_SRC6810_EEV6>();
    inline constexpr HrtimTimerEAdcTriggerSourceId6810 HrtimTimerEAdcTriggerSourceId6810::kExternalEvent7 = HrtimTimerEAdcTriggerSourceId6810::from_ll<LL_HRTIM_ADCTRIG_SRC6810_EEV7>();
    inline constexpr HrtimTimerEAdcTriggerSourceId6810 HrtimTimerEAdcTriggerSourceId6810::kExternalEvent8 = HrtimTimerEAdcTriggerSourceId6810::from_ll<LL_HRTIM_ADCTRIG_SRC6810_EEV8>();
    inline constexpr HrtimTimerEAdcTriggerSourceId6810 HrtimTimerEAdcTriggerSourceId6810::kExternalEvent9 = HrtimTimerEAdcTriggerSourceId6810::from_ll<LL_HRTIM_ADCTRIG_SRC6810_EEV9>();
    inline constexpr HrtimTimerEAdcTriggerSourceId6810 HrtimTimerEAdcTriggerSourceId6810::kExternalEvent10 = HrtimTimerEAdcTriggerSourceId6810::from_ll<LL_HRTIM_ADCTRIG_SRC6810_EEV10>();
    inline constexpr HrtimTimerEAdcTriggerSourceId6810 HrtimTimerEAdcTriggerSourceId6810::kCompare2 = HrtimTimerEAdcTriggerSourceId6810::from_ll<LL_HRTIM_ADCTRIG_SRC6810_TIME_CMP2>();
    inline constexpr HrtimTimerEAdcTriggerSourceId6810 HrtimTimerEAdcTriggerSourceId6810::kCompare3 = HrtimTimerEAdcTriggerSourceId6810::from_ll<LL_HRTIM_ADCTRIG_SRC6810_TIME_CMP3>();
    inline constexpr HrtimTimerEAdcTriggerSourceId6810 HrtimTimerEAdcTriggerSourceId6810::kCompare4 = HrtimTimerEAdcTriggerSourceId6810::from_ll<LL_HRTIM_ADCTRIG_SRC6810_TIME_CMP4>();
    inline constexpr HrtimTimerEAdcTriggerSourceId6810 HrtimTimerEAdcTriggerSourceId6810::kReset = HrtimTimerEAdcTriggerSourceId6810::from_ll<LL_HRTIM_ADCTRIG_SRC6810_TIME_RST>();


    struct HrtimTimerFAdcTriggerSourceId6810 : public LLDriverEnumValue<HrtimTimerFAdcTriggerSourceId6810>
    {
        static const HrtimTimerFAdcTriggerSourceId6810 kExternalEvent6;
        static const HrtimTimerFAdcTriggerSourceId6810 kExternalEvent7;
        static const HrtimTimerFAdcTriggerSourceId6810 kExternalEvent8;
        static const HrtimTimerFAdcTriggerSourceId6810 kExternalEvent9;
        static const HrtimTimerFAdcTriggerSourceId6810 kExternalEvent10;
        static const HrtimTimerFAdcTriggerSourceId6810 kCompare2;
        static const HrtimTimerFAdcTriggerSourceId6810 kCompare3;
        static const HrtimTimerFAdcTriggerSourceId6810 kCompare4;
        static const HrtimTimerFAdcTriggerSourceId6810 kPeriod;
    };

    inline constexpr HrtimTimerFAdcTriggerSourceId6810 HrtimTimerFAdcTriggerSourceId6810::kExternalEvent6 = HrtimTimerFAdcTriggerSourceId6810::from_ll<LL_HRTIM_ADCTRIG_SRC6810_EEV6>();
    inline constexpr HrtimTimerFAdcTriggerSourceId6810 HrtimTimerFAdcTriggerSourceId6810::kExternalEvent7 = HrtimTimerFAdcTriggerSourceId6810::from_ll<LL_HRTIM_ADCTRIG_SRC6810_EEV7>();
    inline constexpr HrtimTimerFAdcTriggerSourceId6810 HrtimTimerFAdcTriggerSourceId6810::kExternalEvent8 = HrtimTimerFAdcTriggerSourceId6810::from_ll<LL_HRTIM_ADCTRIG_SRC6810_EEV8>();
    inline constexpr HrtimTimerFAdcTriggerSourceId6810 HrtimTimerFAdcTriggerSourceId6810::kExternalEvent9 = HrtimTimerFAdcTriggerSourceId6810::from_ll<LL_HRTIM_ADCTRIG_SRC6810_EEV9>();
    inline constexpr HrtimTimerFAdcTriggerSourceId6810 HrtimTimerFAdcTriggerSourceId6810::kExternalEvent10 = HrtimTimerFAdcTriggerSourceId6810::from_ll<LL_HRTIM_ADCTRIG_SRC6810_EEV10>();
    inline constexpr HrtimTimerFAdcTriggerSourceId6810 HrtimTimerFAdcTriggerSourceId6810::kCompare2 = HrtimTimerFAdcTriggerSourceId6810::from_ll<LL_HRTIM_ADCTRIG_SRC6810_TIMF_CMP2>();
    inline constexpr HrtimTimerFAdcTriggerSourceId6810 HrtimTimerFAdcTriggerSourceId6810::kCompare3 = HrtimTimerFAdcTriggerSourceId6810::from_ll<LL_HRTIM_ADCTRIG_SRC6810_TIMF_CMP3>();
    inline constexpr HrtimTimerFAdcTriggerSourceId6810 HrtimTimerFAdcTriggerSourceId6810::kCompare4 = HrtimTimerFAdcTriggerSourceId6810::from_ll<LL_HRTIM_ADCTRIG_SRC6810_TIMF_CMP4>();
    inline constexpr HrtimTimerFAdcTriggerSourceId6810 HrtimTimerFAdcTriggerSourceId6810::kPeriod = HrtimTimerFAdcTriggerSourceId6810::from_ll<LL_HRTIM_ADCTRIG_SRC6810_TIMF_PER>();


    template <HrtimTimerId tkTimerId>
    using HrtimTimerAdcTriggerSourceId13 = std::conditional_t<
        tkTimerId == HrtimTimerId::kTimerA,
        HrtimTimerAAdcTriggerSourceId13,
        std::conditional_t<
            tkTimerId == HrtimTimerId::kTimerB,
            HrtimTimerBAdcTriggerSourceId13,
            std::conditional_t<
                tkTimerId == HrtimTimerId::kTimerC,
                HrtimTimerCAdcTriggerSourceId13,
                std::conditional_t<tkTimerId == HrtimTimerId::kTimerD,
                                   HrtimTimerDAdcTriggerSourceId13,
                                   std::conditional_t<tkTimerId == HrtimTimerId::kTimerE,
                                                      HrtimTimerEAdcTriggerSourceId13,
                                                      std::conditional_t<tkTimerId == HrtimTimerId::kTimerF,
                                                                         HrtimTimerFAdcTriggerSourceId13,
                                                                         void>>>>>>;

    template <HrtimTimerId tkTimerId>
    using HrtimTimerAdcTriggerSourceId24 = std::conditional_t<
        tkTimerId == HrtimTimerId::kTimerA,
        HrtimTimerAAdcTriggerSourceId24,
        std::conditional_t<
            tkTimerId == HrtimTimerId::kTimerB,
            HrtimTimerBAdcTriggerSourceId24,
            std::conditional_t<
                tkTimerId == HrtimTimerId::kTimerC,
                HrtimTimerCAdcTriggerSourceId24,
                std::conditional_t<tkTimerId == HrtimTimerId::kTimerD,
                                   HrtimTimerDAdcTriggerSourceId24,
                                   std::conditional_t<tkTimerId == HrtimTimerId::kTimerE,
                                                      HrtimTimerEAdcTriggerSourceId24,
                                                      std::conditional_t<tkTimerId == HrtimTimerId::kTimerF,
                                                                         HrtimTimerFAdcTriggerSourceId24,
                                                                         void>>>>>>;

    template <HrtimTimerId tkTimerId>
    using HrtimTimerAdcTriggerSourceId579 = std::conditional_t<
        tkTimerId == HrtimTimerId::kTimerA,
        HrtimTimerAAdcTriggerSourceId579,
        std::conditional_t<
            tkTimerId == HrtimTimerId::kTimerB,
            HrtimTimerBAdcTriggerSourceId579,
            std::conditional_t<
                tkTimerId == HrtimTimerId::kTimerC,
                HrtimTimerCAdcTriggerSourceId579,
                std::conditional_t<tkTimerId == HrtimTimerId::kTimerD,
                                   HrtimTimerDAdcTriggerSourceId579,
                                   std::conditional_t<tkTimerId == HrtimTimerId::kTimerE,
                                                      HrtimTimerEAdcTriggerSourceId579,
                                                      std::conditional_t<tkTimerId == HrtimTimerId::kTimerF,
                                                                         HrtimTimerFAdcTriggerSourceId579,
                                                                         void>>>>>>;

    template <HrtimTimerId tkTimerId>
    using HrtimTimerAdcTriggerSourceId6810 = std::conditional_t<
        tkTimerId == HrtimTimerId::kTimerA,
        HrtimTimerAAdcTriggerSourceId6810,
        std::conditional_t<
            tkTimerId == HrtimTimerId::kTimerB,
            HrtimTimerBAdcTriggerSourceId6810,
            std::conditional_t<
                tkTimerId == HrtimTimerId::kTimerC,
                HrtimTimerCAdcTriggerSourceId6810,
                std::conditional_t<tkTimerId == HrtimTimerId::kTimerD,
                                   HrtimTimerDAdcTriggerSourceId6810,
                                   std::conditional_t<tkTimerId == HrtimTimerId::kTimerE,
                                                      HrtimTimerEAdcTriggerSourceId6810,
                                                      std::conditional_t<tkTimerId == HrtimTimerId::kTimerF,
                                                                         HrtimTimerFAdcTriggerSourceId6810,
                                                                         void>>>>>>;

    template <HrtimTimerId tkTimerId>
    using HrtimTimerAdcTriggerSourceId1 = HrtimTimerAdcTriggerSourceId13<tkTimerId>;

    template <HrtimTimerId tkTimerId>
    using HrtimTimerAdcTriggerSourceId2 = HrtimTimerAdcTriggerSourceId24<tkTimerId>;

    template <HrtimTimerId tkTimerId>
    using HrtimTimerAdcTriggerSourceId3 = HrtimTimerAdcTriggerSourceId13<tkTimerId>;

    template <HrtimTimerId tkTimerId>
    using HrtimTimerAdcTriggerSourceId4 = HrtimTimerAdcTriggerSourceId24<tkTimerId>;

    template <HrtimTimerId tkTimerId>
    using HrtimTimerAdcTriggerSourceId5 = HrtimTimerAdcTriggerSourceId579<tkTimerId>;
    template <HrtimTimerId tkTimerId>
    using HrtimTimerAdcTriggerSourceId6 = HrtimTimerAdcTriggerSourceId6810<tkTimerId>;
    template <HrtimTimerId tkTimerId>
    using HrtimTimerAdcTriggerSourceId7 = HrtimTimerAdcTriggerSourceId579<tkTimerId>;

    template <HrtimTimerId tkTimerId>
    using HrtimTimerAdcTriggerSourceId8 = HrtimTimerAdcTriggerSourceId6810<tkTimerId>;

    template <HrtimTimerId tkTimerId>
    using HrtimTimerAdcTriggerSourceId9 = HrtimTimerAdcTriggerSourceId579<tkTimerId>;

    template <HrtimTimerId tkTimerId>
    using HrtimTimerAdcTriggerSourceId10 = HrtimTimerAdcTriggerSourceId6810<tkTimerId>;

    struct HrtimTimerAdcTriggerId : public LLDriverEnumValue<HrtimTimerAdcTriggerId>
    {
        static const HrtimTimerAdcTriggerId kTrig1;
        static const HrtimTimerAdcTriggerId kTrig2;
        static const HrtimTimerAdcTriggerId kTrig3;
        static const HrtimTimerAdcTriggerId kTrig4;
        static const HrtimTimerAdcTriggerId kTrig5;
        static const HrtimTimerAdcTriggerId kTrig6;
        static const HrtimTimerAdcTriggerId kTrig7;
        static const HrtimTimerAdcTriggerId kTrig8;
        static const HrtimTimerAdcTriggerId kTrig9;
        static const HrtimTimerAdcTriggerId kTrig10;
    };

    inline constexpr HrtimTimerAdcTriggerId HrtimTimerAdcTriggerId::kTrig1 = HrtimTimerAdcTriggerId::from_ll<LL_HRTIM_ADCTRIG_1>();
    inline constexpr HrtimTimerAdcTriggerId HrtimTimerAdcTriggerId::kTrig2 = HrtimTimerAdcTriggerId::from_ll<LL_HRTIM_ADCTRIG_2>();
    inline constexpr HrtimTimerAdcTriggerId HrtimTimerAdcTriggerId::kTrig3 = HrtimTimerAdcTriggerId::from_ll<LL_HRTIM_ADCTRIG_3>();
    inline constexpr HrtimTimerAdcTriggerId HrtimTimerAdcTriggerId::kTrig4 = HrtimTimerAdcTriggerId::from_ll<LL_HRTIM_ADCTRIG_4>();
    inline constexpr HrtimTimerAdcTriggerId HrtimTimerAdcTriggerId::kTrig5 = HrtimTimerAdcTriggerId::from_ll<LL_HRTIM_ADCTRIG_5>();
    inline constexpr HrtimTimerAdcTriggerId HrtimTimerAdcTriggerId::kTrig6 = HrtimTimerAdcTriggerId::from_ll<LL_HRTIM_ADCTRIG_6>();
    inline constexpr HrtimTimerAdcTriggerId HrtimTimerAdcTriggerId::kTrig7 = HrtimTimerAdcTriggerId::from_ll<LL_HRTIM_ADCTRIG_7>();
    inline constexpr HrtimTimerAdcTriggerId HrtimTimerAdcTriggerId::kTrig8 = HrtimTimerAdcTriggerId::from_ll<LL_HRTIM_ADCTRIG_8>();
    inline constexpr HrtimTimerAdcTriggerId HrtimTimerAdcTriggerId::kTrig9 = HrtimTimerAdcTriggerId::from_ll<LL_HRTIM_ADCTRIG_9>();
    inline constexpr HrtimTimerAdcTriggerId HrtimTimerAdcTriggerId::kTrig10 = HrtimTimerAdcTriggerId::from_ll<LL_HRTIM_ADCTRIG_10>();


    template <HrtimTimerId tkTimerId, HrtimTimerAdcTriggerId tkTriggerId>
    using HrtimTimerAdcTriggerSourceId = std::conditional_t<
        tkTriggerId == HrtimTimerAdcTriggerId::kTrig1,
        HrtimTimerAdcTriggerSourceId1<tkTimerId>,
        std::conditional_t<
            tkTriggerId == HrtimTimerAdcTriggerId::kTrig2,
            HrtimTimerAdcTriggerSourceId2<tkTimerId>,
            std::conditional_t<
                tkTriggerId == HrtimTimerAdcTriggerId::kTrig3,
                HrtimTimerAdcTriggerSourceId3<tkTimerId>,
                std::conditional_t<
                    tkTriggerId == HrtimTimerAdcTriggerId::kTrig4,
                    HrtimTimerAdcTriggerSourceId4<tkTimerId>,
                    std::conditional_t<
                        tkTriggerId == HrtimTimerAdcTriggerId::kTrig5,
                        HrtimTimerAdcTriggerSourceId5<tkTimerId>,
                        std::conditional_t<
                            tkTriggerId == HrtimTimerAdcTriggerId::kTrig6,
                            HrtimTimerAdcTriggerSourceId6<tkTimerId>,
                            std::conditional_t<
                                tkTriggerId == HrtimTimerAdcTriggerId::kTrig7,
                                HrtimTimerAdcTriggerSourceId7<tkTimerId>,
                                std::conditional_t<
                                    tkTriggerId == HrtimTimerAdcTriggerId::kTrig8,
                                    HrtimTimerAdcTriggerSourceId8<tkTimerId>,
                                    std::conditional_t<
                                        tkTriggerId == HrtimTimerAdcTriggerId::kTrig9,
                                        HrtimTimerAdcTriggerSourceId9<tkTimerId>,
                                        std::conditional_t<tkTriggerId == HrtimTimerAdcTriggerId::kTrig10,
                                                           HrtimTimerAdcTriggerSourceId10<tkTimerId>,
                                                           void>>>>>>>>>>;

    struct HrtimTimerAdcRolloverMode : public LLDriverEnumValue<HrtimTimerAdcRolloverMode>
    {
        static const HrtimTimerAdcRolloverMode kPeriodReset;
        static const HrtimTimerAdcRolloverMode kReset;
        static const HrtimTimerAdcRolloverMode kPeriod;
    };

    inline constexpr HrtimTimerAdcRolloverMode HrtimTimerAdcRolloverMode::kPeriodReset = HrtimTimerAdcRolloverMode::from_ll<LL_HRTIM_ROLLOVER_MODE_BOTH>();
    inline constexpr HrtimTimerAdcRolloverMode HrtimTimerAdcRolloverMode::kReset = HrtimTimerAdcRolloverMode::from_ll<LL_HRTIM_ROLLOVER_MODE_RST>();
    inline constexpr HrtimTimerAdcRolloverMode HrtimTimerAdcRolloverMode::kPeriod = HrtimTimerAdcRolloverMode::from_ll<LL_HRTIM_ROLLOVER_MODE_PER>();


    // ============================================================================
    // HARDWARE TRAITS
    // ============================================================================
    template <HrtimControllerId tkControllerId>
    struct HrtimControllerTraits;

    template <>
    struct HrtimControllerTraits<HrtimControllerId::kHrtim1>
    {
        static inline Hrtim_TypeDef* const skInstance = Hrtim1;

        void enable_clock() const
        {
            LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_HRTim1);
        }
    };

    // ---------------------------------------------------------------------------
    // HRTIM FAULT
    // ---------------------------------------------------------------------------
    template <HrtimControllerId tkControllerId, HrtimFaultId tkFaultId>
    struct HrtimFaultTraits
    {
    private:
        [[nodiscard]] static constexpr uint32_t get_ll_id()
        {
            if constexpr (tkFaultId == HrtimFaultId::kFault1)
            {
                return LL_HRTIM_FAULT_1;
            }
            else if constexpr (tkFaultId == HrtimFaultId::kFault2)
            {
                return LL_HRTIM_FAULT_2;
            }
            else if constexpr (tkFaultId == HrtimFaultId::kFault3)
            {
                return LL_HRTIM_FAULT_3;
            }
            else if constexpr (tkFaultId == HrtimFaultId::kFault4)
            {
                return LL_HRTIM_FAULT_4;
            }
            else if constexpr (tkFaultId == HrtimFaultId::kFault5)
            {
                return LL_HRTIM_FAULT_5;
            }
            else if constexpr (tkFaultId == HrtimFaultId::kFault6)
            {
                return LL_HRTIM_FAULT_6;
            }
            else
            {
                static_assert(kAlwaysFalseV<tkFaultId>, "Invalid HrtimFaultId");
            }
        }

    public:
        static constexpr uint32_t skLLId = get_ll_id();
    };

    // ---------------------------------------------------------------------------
    // HRTIM EEV
    // ---------------------------------------------------------------------------
    template <HrtimControllerId tkControllerId, HrtimExternalEventId tkExternalEventId>
    struct HrtimExternalEventTraits
    {
    private:
        [[nodiscard]] static constexpr uint32_t get_ll_id()
        {
            if constexpr (tkExternalEventId == HrtimExternalEventId::kExternalEvent1)
            {
                return LL_HRTIM_EVENT_1;
            }
            else if constexpr (tkExternalEventId == HrtimExternalEventId::kExternalEvent2)
            {
                return LL_HRTIM_EVENT_2;
            }
            else if constexpr (tkExternalEventId == HrtimExternalEventId::kExternalEvent3)
            {
                return LL_HRTIM_EVENT_3;
            }
            else if constexpr (tkExternalEventId == HrtimExternalEventId::kExternalEvent4)
            {
                return LL_HRTIM_EVENT_4;
            }
            else if constexpr (tkExternalEventId == HrtimExternalEventId::kExternalEvent5)
            {
                return LL_HRTIM_EVENT_5;
            }
            else if constexpr (tkExternalEventId == HrtimExternalEventId::kExternalEvent6)
            {
                return LL_HRTIM_EVENT_6;
            }
            else if constexpr (tkExternalEventId == HrtimExternalEventId::kExternalEvent7)
            {
                return LL_HRTIM_EVENT_7;
            }
            else if constexpr (tkExternalEventId == HrtimExternalEventId::kExternalEvent8)
            {
                return LL_HRTIM_EVENT_8;
            }
            else if constexpr (tkExternalEventId == HrtimExternalEventId::kExternalEvent9)
            {
                return LL_HRTIM_EVENT_9;
            }
            else if constexpr (tkExternalEventId == HrtimExternalEventId::kExternalEvent10)
            {
                return LL_HRTIM_EVENT_10;
            }
            else
            {
                static_assert(kAlwaysFalseV<tkExternalEventId>, "Invalid HrtimExternalEventId");
            }
        }

    public:
        static constexpr uint32_t skLLId = get_ll_id();
    };

    // ----------------------------------------------------------------------------
    // HRTIM TIMER
    // ----------------------------------------------------------------------------
    template <HrtimControllerId tkControllerId, HrtimTimerId tkTimerId>
    struct HrtimTimerTraits;

    // Hrtim1 Timer A (tkHrtimTimerId 0)
    template <>
    struct HrtimTimerTraits<HrtimControllerId::kHrtim1, HrtimTimerId::kTimerA>
    {
        static constexpr uint32_t  skLLId            = LL_HRTIM_TIMER_A;
        static constexpr uint32_t  skTimerIdx        = Hrtim_TIMERINDEX_TIMER_A;
        static constexpr uint32_t  skOutput1         = Hrtim_OUTPUT_TA1;
        static constexpr uint32_t  skOutput2         = Hrtim_OUTPUT_TA2;
        static constexpr IRQn_Type skIRQn            = Hrtim1_TIMA_IRQn;
        static constexpr uint32_t  skAdcTrigUpdateId = LL_HRTIM_ADCTRIG_UPDATE_TIMER_A;
    };

    // Hrtim1 Timer B
    template <>
    struct HrtimTimerTraits<HrtimControllerId::kHrtim1, HrtimTimerId::kTimerB>
    {
        static constexpr uint32_t  skLLId            = LL_HRTIM_TIMER_B;
        static constexpr uint32_t  skTimerIdx        = Hrtim_TIMERINDEX_TIMER_B;
        static constexpr uint32_t  skOutput1         = Hrtim_OUTPUT_TB1;
        static constexpr uint32_t  skOutput2         = Hrtim_OUTPUT_TB2;
        static constexpr IRQn_Type skIRQn            = Hrtim1_TIMB_IRQn;
        static constexpr uint32_t  skAdcTrigUpdateId = LL_HRTIM_ADCTRIG_UPDATE_TIMER_B;
    };

    // Hrtim1 Timer C
    template <>
    struct HrtimTimerTraits<HrtimControllerId::kHrtim1, HrtimTimerId::kTimerC>
    {
        static constexpr uint32_t  skLLId            = LL_HRTIM_TIMER_C;
        static constexpr uint32_t  skTimerIdx        = Hrtim_TIMERINDEX_TIMER_C;
        static constexpr uint32_t  skOutput1         = Hrtim_OUTPUT_TC1;
        static constexpr uint32_t  skOutput2         = Hrtim_OUTPUT_TC2;
        static constexpr IRQn_Type skIRQn            = Hrtim1_TIMC_IRQn;
        static constexpr uint32_t  skAdcTrigUpdateId = LL_HRTIM_ADCTRIG_UPDATE_TIMER_C;
    };

    // Hrtim1 Timer D
    template <>
    struct HrtimTimerTraits<HrtimControllerId::kHrtim1, HrtimTimerId::kTimerD>
    {
        static constexpr uint32_t  skLLId            = LL_HRTIM_TIMER_D;
        static constexpr uint32_t  skTimerIdx        = Hrtim_TIMERINDEX_TIMER_D;
        static constexpr uint32_t  skOutput1         = Hrtim_OUTPUT_TD1;
        static constexpr uint32_t  skOutput2         = Hrtim_OUTPUT_TD2;
        static constexpr IRQn_Type skIRQn            = Hrtim1_TIMD_IRQn;
        static constexpr uint32_t  skAdcTrigUpdateId = LL_HRTIM_ADCTRIG_UPDATE_TIMER_D;
    };

    // Hrtim1 Timer E
    template <>
    struct HrtimTimerTraits<HrtimControllerId::kHrtim1, HrtimTimerId::kTimerE>
    {
        static constexpr uint32_t  skLLId            = LL_HRTIM_TIMER_E;
        static constexpr uint32_t  skTimerIdx        = Hrtim_TIMERINDEX_TIMER_E;
        static constexpr uint32_t  skOutput1         = Hrtim_OUTPUT_TE1;
        static constexpr uint32_t  skOutput2         = Hrtim_OUTPUT_TE2;
        static constexpr IRQn_Type skIRQn            = Hrtim1_TIME_IRQn;
        static constexpr uint32_t  skAdcTrigUpdateId = LL_HRTIM_ADCTRIG_UPDATE_TIMER_E;
    };

    // Hrtim1 Timer F
    template <>
    struct HrtimTimerTraits<HrtimControllerId::kHrtim1, HrtimTimerId::kTimerF>
    {
        static constexpr uint32_t  skLLId            = LL_HRTIM_TIMER_F;
        static constexpr uint32_t  skTimerIdx        = Hrtim_TIMERINDEX_TIMER_F;
        static constexpr uint32_t  skOutput1         = Hrtim_OUTPUT_TF1;
        static constexpr uint32_t  skOutput2         = Hrtim_OUTPUT_TF2;
        static constexpr IRQn_Type skIRQn            = Hrtim1_TIMF_IRQn;
        static constexpr uint32_t  skAdcTrigUpdateId = LL_HRTIM_ADCTRIG_UPDATE_TIMER_F;
    };

    // ============================================================================
    // Interface
    // ============================================================================

    template <HrtimControllerId tkControllerId, HrtimTimerId tkTimerId>
    struct HrtimTimerInterface
    {
        static constexpr HrtimControllerId skControllerId = tkControllerId;
        static constexpr HrtimTimerId      skTimerId      = tkTimerId;

        using ControllerTraitsT = HrtimControllerTraits<tkControllerId>;
        using TimerTraitsT      = HrtimTimerTraits<tkControllerId, tkTimerId>;

        static constexpr uint32_t            skMinPrescalerNum      = 0;
        static constexpr uint32_t            skMaxPrescalerNum      = 7;
        static constexpr HrtimTimerPrescaler skMinPrescaler         = HrtimTimerPrescaler::kMul32;
        static constexpr HrtimTimerPrescaler skMaxPrescaler         = HrtimTimerPrescaler::kDiv4;
        static constexpr uint32_t            skPrescalerScaleFactor = 32;

        static constexpr uint32_t skMaxPeriod = 65535;

        static constexpr uint32_t                    skDeadtimeMinPrescalerNum = 0;
        static constexpr uint32_t                    skDeadtimeMaxPrescalerNum = 7;
        static constexpr HrtimTimerDeadtimePrescaler skDeadtimeMinPrescaler    = HrtimTimerDeadtimePrescaler::kMul8;
        static constexpr HrtimTimerDeadtimePrescaler skDeadtimeMaxPrescaler    = HrtimTimerDeadtimePrescaler::kDiv16;
        static constexpr uint32_t                    skDeadtimePrescalerScaleFactor = 8;
        static constexpr uint32_t                    skDeadtimeCounterMax           = 511;

        // -------------------------------------------------------------------------
        // TimING
        // -------------------------------------------------------------------------
        [[nodiscard]] HrtimTimerPrescaler get_timer_prescaler() const
        {
            return static_cast<HrtimTimerPrescaler>(
                LL_HRTIM_TIM_GetPrescaler(ControllerTraitsT::skInstance, TimerTraitsT::skLLId));
        }

        [[nodiscard]] uint8_t timer_prescaler_to_prescaler_num(HrtimTimerPrescaler prescaler) const
        {
            // NOLINTBEGIN(readability-magic-numbers)
            static_assert(static_cast<uint32_t>(HrtimTimerPrescaler::kMul32) == 0,
                          "Assumes prescaler enum starts at 0 with Mul32");
            static_assert(static_cast<uint32_t>(HrtimTimerPrescaler::kMul16) == 1,
                          "Assumes prescaler enum values are sequential");
            static_assert(static_cast<uint32_t>(HrtimTimerPrescaler::kMul8) == 2,
                          "Assumes prescaler enum values are sequential");
            static_assert(static_cast<uint32_t>(HrtimTimerPrescaler::kMul4) == 3,
                          "Assumes prescaler enum values are sequential");
            static_assert(static_cast<uint32_t>(HrtimTimerPrescaler::kMul2) == 4,
                          "Assumes prescaler enum values are sequential");
            static_assert(static_cast<uint32_t>(HrtimTimerPrescaler::kDiv1) == 5,
                          "Assumes prescaler enum values are sequential");
            static_assert(static_cast<uint32_t>(HrtimTimerPrescaler::kDiv2) == 6,
                          "Assumes prescaler enum values are sequential");
            static_assert(static_cast<uint32_t>(HrtimTimerPrescaler::kDiv4) == 7,
                          "Assumes prescaler enum values are sequential");
            // NOLINTEND(readability-magic-numbers)

            return static_cast<uint8_t>(prescaler);
        }

        [[nodiscard]] static constexpr HrtimTimerPrescaler timer_prescaler_num_to_prescaler(uint8_t prescaler_num)
        {
            // NOLINTBEGIN(readability-magic-numbers)
            static_assert(static_cast<uint32_t>(HrtimTimerPrescaler::kMul32) == 0,
                          "Assumes prescaler enum starts at 0 with Mul32");
            static_assert(static_cast<uint32_t>(HrtimTimerPrescaler::kMul16) == 1,
                          "Assumes prescaler enum values are sequential");
            static_assert(static_cast<uint32_t>(HrtimTimerPrescaler::kMul8) == 2,
                          "Assumes prescaler enum values are sequential");
            static_assert(static_cast<uint32_t>(HrtimTimerPrescaler::kMul4) == 3,
                          "Assumes prescaler enum values are sequential");
            static_assert(static_cast<uint32_t>(HrtimTimerPrescaler::kMul2) == 4,
                          "Assumes prescaler enum values are sequential");
            static_assert(static_cast<uint32_t>(HrtimTimerPrescaler::kDiv1) == 5,
                          "Assumes prescaler enum values are sequential");
            static_assert(static_cast<uint32_t>(HrtimTimerPrescaler::kDiv2) == 6,
                          "Assumes prescaler enum values are sequential");
            static_assert(static_cast<uint32_t>(HrtimTimerPrescaler::kDiv4) == 7,
                          "Assumes prescaler enum values are sequential");
            // NOLINTEND(readability-magic-numbers)

            return static_cast<HrtimTimerPrescaler>(prescaler_num);
        }

        [[nodiscard]] uint32_t get_period_ticks() const
        {
            return LL_HRTIM_TIM_GetPeriod(ControllerTraitsT::skInstance, TimerTraitsT::skLLId);
        }

        [[nodiscard]] uint32_t get_repetition() const
        {
            return LL_HRTIM_TIM_GetRepetition(ControllerTraitsT::skInstance, TimerTraitsT::skLLId);
        }

        [[nodiscard]] static uint64_t calculate_hrck_freq_hz(const uint32_t            f_hrtim_hz,
                                                             const HrtimTimerPrescaler prescaler)
        {
            const uint8_t prescaler_num = timer_prescaler_to_prescaler_num(prescaler);
            return (static_cast<uint64_t>(f_hrtim_hz) * static_cast<uint64_t>(skPrescalerScaleFactor)) /
                   (1 << prescaler_num);
        }

        [[nodiscard]] uint64_t get_hrck_freq_hz(const uint32_t f_hrtim_hz) const
        {
            return calculate_hrck_freq_hz(f_hrtim_hz, get_timer_prescaler());
        }

        // NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
        [[nodiscard]] static constexpr uint64_t calculate_timer_freq_hz(const uint64_t f_hrck_hz,
                                                                        const uint32_t period_ticks)
        {
            return f_hrck_hz / (period_ticks + 1);
        }

        [[nodiscard]] uint64_t get_timer_freq_hz(const uint32_t f_hrtim_hz) const
        {
            return calculate_timer_freq_hz(get_hrck_freq_hz(f_hrtim_hz), get_period_ticks());
        }

        // NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
        [[nodiscard]] static constexpr uint64_t calculate_int_freq_hz(const uint32_t f_timer_hz,
                                                                      const uint32_t repetition)
        {
            return f_timer_hz / (repetition + 1);
        }

        [[nodiscard]] uint64_t get_int_freq_hz(const uint32_t f_hrtim_hz) const
        {
            return calculate_int_freq_hz(get_timer_freq_hz(f_hrtim_hz), get_repetition());
        }

        [[nodiscard]] static constexpr uint64_t calculate_min_freq_hz(const uint32_t f_hrtim_hz)
        {
            // Minimum frequency is achieved with maximum prescaler (7) and maximum period (65535)
            return calculate_timer_freq_hz(calculate_hrck_freq_hz(f_hrtim_hz, HrtimTimerPrescaler::kDiv4), skMaxPeriod);
        }

        [[nodiscard]] static constexpr uint64_t calculate_max_freq_hz(const uint32_t f_hrtim_hz)
        {
            // Maximum frequency is achieved with minimum prescaler (0) and minimum period (1)
            return calculate_timer_freq_hz(calculate_hrck_freq_hz(f_hrtim_hz, HrtimTimerPrescaler::kMul32), 1);
        }

        [[nodiscard]] static constexpr HrtimTimerPrescaler
        calculate_timer_prescaler_for_freq(  // NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
            const uint32_t f_hrtim_hz,
            const uint64_t target_freq_hz)
        {
            // We need to find the appropriate prescaler for the clock.
            // Each prescaler will have a minimum allowable PWM frequency since the counter is 16-bit.
            // This minimum frequency is given by f_min = f_hrtim * prescaler / 2^16
            // We need to find the smallest integer i such that f_hrtim * 32 / (2^i * f_target) <= 65535.
            // We arrive at the following condition for i:
            // i > log2(f_hrtim / f_target) - 11 --> i = floor(log2(f_hrtim / f_target)) - 10
            // To implement this efficiently, we can take advantage of the fact that floor(log2(floor(x))) =
            // floor(log2(x)) and then we can use __CLZ to compute floor(log2(x)) = 31 - __CLZ(x) for 32-bit integers.

            // Validate input to prevent divide-by-zero and __builtin_clz(0)
            // RM0440 Section 29.3.6: HRTIM frequency range is ~81 Hz to 5.3 MHz @ 170 MHz

            // Minimum: f_HRTIM / (32 × 65536) = 170 MHz / 2,097,152 ≈ 81 Hz
            // Maximum: f_HRTIM / 32 = 170 MHz / 32 ≈ 5.3 MHz

            // Validate and clamp target frequency to valid range
            if (target_freq_hz < calculate_min_freq_hz(f_hrtim_hz))
            {
                // Too low - use minimum (prescaler 7)
                return skMaxPrescaler;
            }

            if (target_freq_hz > calculate_max_freq_hz(f_hrtim_hz))
            {
                // Too high - use maximum (prescaler 0)
                return skMinPrescaler;
            }

            const uint32_t ratio = f_hrtim_hz / target_freq_hz;
            const int8_t   index = static_cast<int8_t>(31 - __builtin_clz(ratio) - 10);
            return timer_prescaler_num_to_prescaler(std::clamp<int8_t>(index, skMinPrescalerNum, skMaxPrescalerNum));
        }

        // -------------------------------------------------------------------------
        // DEADTimE
        // -------------------------------------------------------------------------

        [[nodiscard]] static constexpr HrtimTimerDeadtimePrescaler deadtime_prescaler_num_to_prescaler(
            uint8_t prescaler_num)
        {
            static_assert(static_cast<HrtimTimerDeadtimePrescaler>(0 << Hrtim_DTR_DTPRSC_Pos) ==
                              HrtimTimerDeadtimePrescaler::kMul8,
                          "Assumes deadtime prescaler enum values are sequential and start at 0 with Mul8");

            static_assert(static_cast<HrtimTimerDeadtimePrescaler>(1 << Hrtim_DTR_DTPRSC_Pos) ==
                              HrtimTimerDeadtimePrescaler::kMul4,
                          "Assumes deadtime prescaler enum values are sequential and start at 0 with Mul8");

            static_assert(static_cast<HrtimTimerDeadtimePrescaler>(2 << Hrtim_DTR_DTPRSC_Pos) ==
                              HrtimTimerDeadtimePrescaler::kMul2,
                          "Assumes deadtime prescaler enum values are sequential and start at 0 with Mul8");

            static_assert(static_cast<HrtimTimerDeadtimePrescaler>(3 << Hrtim_DTR_DTPRSC_Pos) ==
                              HrtimTimerDeadtimePrescaler::kDiv1,
                          "Assumes deadtime prescaler enum values are sequential and start at 0 with Mul8");

            static_assert(static_cast<HrtimTimerDeadtimePrescaler>(4 << Hrtim_DTR_DTPRSC_Pos) ==
                              HrtimTimerDeadtimePrescaler::kDiv2,
                          "Assumes deadtime prescaler enum values are sequential and start at 0 with Mul8");

            static_assert(static_cast<HrtimTimerDeadtimePrescaler>(5 << Hrtim_DTR_DTPRSC_Pos) ==
                              HrtimTimerDeadtimePrescaler::kDiv4,
                          "Assumes deadtime prescaler enum values are sequential and start at 0 with Mul8");

            static_assert(static_cast<HrtimTimerDeadtimePrescaler>(6 << Hrtim_DTR_DTPRSC_Pos) ==
                              HrtimTimerDeadtimePrescaler::kDiv8,
                          "Assumes deadtime prescaler enum values are sequential and start at 0 with Mul8");

            static_assert(static_cast<HrtimTimerDeadtimePrescaler>(7 << Hrtim_DTR_DTPRSC_Pos) ==
                              HrtimTimerDeadtimePrescaler::kDiv16,
                          "Assumes deadtime prescaler enum values are sequential and start at 0 with Mul8");

            uint8_t clamped_num =
                std::clamp<uint8_t>(prescaler_num, skDeadtimeMinPrescalerNum, skDeadtimeMaxPrescalerNum);

            // Deadtime prescaler value is offset by bit 0 of the DTRRSC register (Hrtim_DTR_DTPRSC_Pos)
            return static_cast<HrtimTimerDeadtimePrescaler>(clamped_num << Hrtim_DTR_DTPRSC_Pos);
        }

        [[nodiscard]] static constexpr uint8_t deadtime_prescaler_to_prescaler_num(
            const HrtimTimerDeadtimePrescaler prescaler)
        {
            return (static_cast<uint32_t>(prescaler) >> Hrtim_DTR_DTPRSC_Pos) & 0x7;
        }

        [[nodiscard]] static constexpr float calculate_deadtime_period(const uint32_t                    f_hrtim_hz,
                                                                       const HrtimTimerDeadtimePrescaler prescaler)
        {
            constexpr float to_ns_factor = 1e9F;

            // Deadtime period is given by t_deadtime = 1 / f_deadtime where f_deadtime = f_hrtim * 8 / 2^(prescaler).
            // Therefore, t_deadtime = 2^(prescaler) / (f_hrtim * 8) in seconds, convert to ns
            const uint32_t prescaler_num    = deadtime_prescaler_to_prescaler_num(prescaler);
            const float    prescaler_factor = static_cast<float>(1ULL << prescaler_num);
            return (prescaler_factor * to_ns_factor) /
                   (static_cast<float>(f_hrtim_hz) * static_cast<float>(skDeadtimePrescalerScaleFactor));
        }

        [[nodiscard]] static constexpr float calculate_max_deadtime_ns(const uint32_t f_hrtim_hz)
        {
            return skDeadtimeCounterMax * calculate_deadtime_period(f_hrtim_hz, HrtimTimerDeadtimePrescaler::kDiv16);
        }

        [[nodiscard]] static constexpr float calculate_min_deadtime_ns(const uint32_t f_hrtim_hz)
        {
            return calculate_deadtime_period(f_hrtim_hz, HrtimTimerDeadtimePrescaler::kMul8);
        }

        [[nodiscard]] static constexpr HrtimTimerDeadtimePrescaler
        calculate_deadtime_prescaler(  // NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
            const uint32_t f_hrtim_hz,
            const float    target_deadtime_ns)
        {
            // The maximum deadtime time is given by the deadtime clock period multiplied by 511 (max register value).
            // The deadtime clock frequency is given by f_deadtime = f_hrtim * 8 / 2^(prescaler).
            // The deadtime period in ns is given by t_deadtime = 1 / f_deadtime = 2^(prescaler) / (f_hrtim * 8) * 10^9.
            // The maximum deadtime is given by t_max_deadtime = t_deadtime * 511 = 511 * 2^(prescaler) / (f_hrtim * 8)
            // * 10^9. We need to ensure that target_deadtime_ns <= t_max_deadtime, which gives us the condition:
            // target_deadtime_ns <= 511 * 2^(prescaler) / (f_hrtim * 8) * 10^9
            // Rearranging gives us:
            // 2^(prescaler) >= (target_deadtime_ns * f_hrtim * 8) / (511 * 10^9)
            // Taking log2 of both sides gives us:
            // prescaler >= log2((target_deadtime_ns * f_hrtim * 8) / (511 * 10^9))
            // prescaler = ceil(log2((target_deadtime_ns * f_hrtim * 8) / (511 * 10^9)))

            if (target_deadtime_ns < calculate_min_deadtime_ns(f_hrtim_hz))
            {
                // Too low - use minimum (prescaler 0)
                return skDeadtimeMinPrescaler;
            }

            if (target_deadtime_ns > calculate_max_deadtime_ns(f_hrtim_hz))
            {
                // Too high - use maximum (prescaler 7)
                return skDeadtimeMaxPrescaler;
            }

            const double threshold = (static_cast<double>(target_deadtime_ns) * static_cast<double>(f_hrtim_hz) *
                                      static_cast<double>(skDeadtimePrescalerScaleFactor)) /
                                     (static_cast<double>(skDeadtimeCounterMax) * 1e9);

            if (threshold <= 1.0)
            {
                // Too low - use minimum (prescaler 0)
                return skDeadtimeMinPrescaler;
            }

            const uint8_t prescaler_num = static_cast<uint8_t>(std::ceil(std::log2(threshold)));
            return deadtime_prescaler_num_to_prescaler(prescaler_num);
        }

        [[nodiscard]] static constexpr uint32_t calculate_deadtime_count(const uint32_t                    f_hrtim_hz,
                                                                         const HrtimTimerDeadtimePrescaler prescaler,
                                                                         const float target_deadtime_ns)
        {
            // Once we have the appropriate prescaler, we can calculate the required counts for the desired deadtime.
            // counts is given by: counts = target_deadtime_ns / t_deadtime where t_deadtime = 2^(prescaler) / (f_hrtim
            // * 8) * 10^9. Rearranging gives us: counts = (target_deadtime_ns * f_hrtim * 8) / (2^(prescaler) * 10^9)

            const float deadtime_period_ns = calculate_deadtime_period(f_hrtim_hz, prescaler);
            return std::clamp(static_cast<uint32_t>(target_deadtime_ns / deadtime_period_ns),
                              static_cast<uint32_t>(0U),
                              static_cast<uint32_t>(skDeadtimeCounterMax));
        }
        // ---------------------------------------------------------------------------
        // COUNTERS
        // ---------------------------------------------------------------------------
        void enable_counter() const
        {
            LL_HRTIM_TIM_CounterEnable(ControllerTraitsT::skInstance, TimerTraitsT::skLLId);
        }

        void disable_counter() const
        {
            LL_HRTIM_TIM_CounterDisable(ControllerTraitsT::skInstance, TimerTraitsT::skLLId);
        }

        // ----------------------------------------------------------------------------
        // OUTPUTS
        // ----------------------------------------------------------------------------
        void enable_output1() const
        {
            LL_HRTIM_TIM_EnableOutput(ControllerTraitsT::skInstance, TimerTraitsT::skOutput1);
        }

        void disable_output1() const
        {
            LL_HRTIM_TIM_DisableOutput(ControllerTraitsT::skInstance, TimerTraitsT::skOutput1);
        }

        void enable_output2() const
        {
            LL_HRTIM_TIM_EnableOutput(ControllerTraitsT::skInstance, TimerTraitsT::skOutput2);
        }

        void disable_output2() const
        {
            LL_HRTIM_TIM_DisableOutput(ControllerTraitsT::skInstance, TimerTraitsT::skOutput2);
        }

        void enable_outputs() const
        {
            LL_HRTIM_EnableOutput(ControllerTraitsT::skInstance, TimerTraitsT::skOutput1 | TimerTraitsT::skOutput2);
        }

        void disable_outputs() const
        {
            LL_HRTIM_DisableOutput(ControllerTraitsT::skInstance, TimerTraitsT::skOutput1 | TimerTraitsT::skOutput2);
        }

        // ----------------------------------------------------------------------------
        // COMPARE
        // ----------------------------------------------------------------------------

        template <HrtimTimerCompareUnit tkCompareUnit>
        static constexpr void set_compare(uint32_t compare_value)
        {
            if constexpr (tkCompareUnit == HrtimTimerCompareUnit::kCompare1)
            {
                LL_HRTIM_TIM_SetCompare1(ControllerTraitsT::skInstance, TimerTraitsT::skLLId, compare_value);
            }
            else if constexpr (tkCompareUnit == HrtimTimerCompareUnit::kCompare2)
            {
                LL_HRTIM_TIM_SetCompare2(ControllerTraitsT::skInstance, TimerTraitsT::skLLId, compare_value);
            }
            else if constexpr (tkCompareUnit == HrtimTimerCompareUnit::kCompare3)
            {
                LL_HRTIM_TIM_SetCompare3(ControllerTraitsT::skInstance, TimerTraitsT::skLLId, compare_value);
            }
            else if constexpr (tkCompareUnit == HrtimTimerCompareUnit::kCompare4)
            {
                LL_HRTIM_TIM_SetCompare4(ControllerTraitsT::skInstance, TimerTraitsT::skLLId, compare_value);
            }
            else
            {
                static_assert(kAlwaysFalseV<tkCompareUnit>, "Invalid HrtimTimerCompareUnit");
            }
        }

        static constexpr void set_compare(HrtimTimerCompareUnit compare_unit, uint32_t compare_value)
        {
            switch (compare_unit)
            {
                case HrtimTimerCompareUnit::kCompare1:
                    set_compare<HrtimTimerCompareUnit::kCompare1>(compare_value);
                    break;
                case HrtimTimerCompareUnit::kCompare2:
                    set_compare<HrtimTimerCompareUnit::kCompare2>(compare_value);
                    break;
                case HrtimTimerCompareUnit::kCompare3:
                    set_compare<HrtimTimerCompareUnit::kCompare3>(compare_value);
                    break;
                case HrtimTimerCompareUnit::kCompare4:
                    set_compare<HrtimTimerCompareUnit::kCompare4>(compare_value);
                    break;
                default:
                    // Invalid compare unit, default to Compare1
                    set_compare<HrtimTimerCompareUnit::kCompare1>(compare_value);
                    break;
            }
        }

        template <HrtimTimerCompareUnit tkCompareUnit>
        [[nodiscard]] static constexpr HrtimTimerOutputSetSource set_source_for_compare_unit()
        {
            if constexpr (tkCompareUnit == HrtimTimerCompareUnit::kCompare1)
            {
                return HrtimTimerOutputSetSource::kTimerCompare1;
            }
            else if constexpr (tkCompareUnit == HrtimTimerCompareUnit::kCompare2)
            {
                return HrtimTimerOutputSetSource::kTimerCompare2;
            }
            else if constexpr (tkCompareUnit == HrtimTimerCompareUnit::kCompare3)
            {
                return HrtimTimerOutputSetSource::kTimerCompare3;
            }
            else if constexpr (tkCompareUnit == HrtimTimerCompareUnit::kCompare4)
            {
                return HrtimTimerOutputSetSource::kTimerCompare4;
            }
            else
            {
                static_assert(kAlwaysFalseV<tkCompareUnit>, "Invalid HrtimTimerCompareUnit");
            }
        }

        [[nodiscard]] static constexpr HrtimTimerOutputSetSource set_source_for_compare_unit(
            HrtimTimerCompareUnit compare_unit)
        {
            switch (compare_unit)
            {
                case HrtimTimerCompareUnit::kCompare1:
                    return set_source_for_compare_unit<HrtimTimerCompareUnit::kCompare1>();
                case HrtimTimerCompareUnit::kCompare2:
                    return set_source_for_compare_unit<HrtimTimerCompareUnit::kCompare2>();
                case HrtimTimerCompareUnit::kCompare3:
                    return set_source_for_compare_unit<HrtimTimerCompareUnit::kCompare3>();
                case HrtimTimerCompareUnit::kCompare4:
                    return set_source_for_compare_unit<HrtimTimerCompareUnit::kCompare4>();
                default:
                    // Invalid compare unit, default to Compare1
                    return set_source_for_compare_unit<HrtimTimerCompareUnit::kCompare1>();
            }
        }

        template <HrtimTimerCompareUnit tkCompareUnit>
        [[nodiscard]] static constexpr HrtimTimerOutputResetSource reset_source_for_compare_unit()
        {
            if constexpr (tkCompareUnit == HrtimTimerCompareUnit::kCompare1)
            {
                return HrtimTimerOutputResetSource::kTimerCompare1;
            }
            else if constexpr (tkCompareUnit == HrtimTimerCompareUnit::kCompare2)
            {
                return HrtimTimerOutputResetSource::kTimerCompare2;
            }
            else if constexpr (tkCompareUnit == HrtimTimerCompareUnit::kCompare3)
            {
                return HrtimTimerOutputResetSource::kTimerCompare3;
            }
            else if constexpr (tkCompareUnit == HrtimTimerCompareUnit::kCompare4)
            {
                return HrtimTimerOutputResetSource::kTimerCompare4;
            }
            else
            {
                static_assert(kAlwaysFalseV<tkCompareUnit>, "Invalid HrtimTimerCompareUnit");
            }
        }

        [[nodiscard]] static constexpr HrtimTimerOutputResetSource reset_source_for_compare_unit(
            HrtimTimerCompareUnit compare_unit)
        {
            switch (compare_unit)
            {
                case HrtimTimerCompareUnit::kCompare1:
                    return reset_source_for_compare_unit<HrtimTimerCompareUnit::kCompare1>();
                case HrtimTimerCompareUnit::kCompare2:
                    return reset_source_for_compare_unit<HrtimTimerCompareUnit::kCompare2>();
                case HrtimTimerCompareUnit::kCompare3:
                    return reset_source_for_compare_unit<HrtimTimerCompareUnit::kCompare3>();
                case HrtimTimerCompareUnit::kCompare4:
                    return reset_source_for_compare_unit<HrtimTimerCompareUnit::kCompare4>();
                default:
                    // Invalid compare unit, default to Compare1
                    return reset_source_for_compare_unit<HrtimTimerCompareUnit::kCompare1>();
            }
        }
    };

}  // namespace valle::platform