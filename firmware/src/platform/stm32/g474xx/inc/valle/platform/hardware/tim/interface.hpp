#pragma once
#include "stm32g4xx_ll_bus.h"
#include "stm32g4xx_ll_tim.h"
#include "valle/platform/hardware/tim/id.hpp"

namespace valle::platform
{
    // =========================================================================
    // ENUMERATIONS
    // =========================================================================

    // -------------------------------------------------------------------------
    // COUNTER ENUMS
    // -------------------------------------------------------------------------

    struct TimControllerCounterMode : public LLDriverEnumValue<TimControllerCounterMode>
    {
        static const TimControllerCounterMode kUp;
        static const TimControllerCounterMode kDown;
        static const TimControllerCounterMode kCenterDown;
        static const TimControllerCounterMode kCenterUp;
        static const TimControllerCounterMode kCenterUpDown;
    };

    inline constexpr TimControllerCounterMode TimControllerCounterMode::kUp = TimControllerCounterMode::from_ll<LL_TIM_COUNTERMODE_UP>();
    inline constexpr TimControllerCounterMode TimControllerCounterMode::kDown = TimControllerCounterMode::from_ll<LL_TIM_COUNTERMODE_DOWN>();
    inline constexpr TimControllerCounterMode TimControllerCounterMode::kCenterDown = TimControllerCounterMode::from_ll<LL_TIM_COUNTERMODE_CENTER_DOWN>();
    inline constexpr TimControllerCounterMode TimControllerCounterMode::kCenterUp = TimControllerCounterMode::from_ll<LL_TIM_COUNTERMODE_CENTER_UP>();
    inline constexpr TimControllerCounterMode TimControllerCounterMode::kCenterUpDown = TimControllerCounterMode::from_ll<LL_TIM_COUNTERMODE_CENTER_UP_DOWN>();


    struct TimControllerDirection : public LLDriverEnumValue<TimControllerDirection>
    {
        static const TimControllerDirection kUp;
        static const TimControllerDirection kDown;
    };

    inline constexpr TimControllerDirection TimControllerDirection::kUp = TimControllerDirection::from_ll<LL_TIM_COUNTERDIRECTION_UP>();
    inline constexpr TimControllerDirection TimControllerDirection::kDown = TimControllerDirection::from_ll<LL_TIM_COUNTERDIRECTION_DOWN>();


    struct TimControllerClockDivision : public LLDriverEnumValue<TimControllerClockDivision>
    {
        static const TimControllerClockDivision kDiv1;
        static const TimControllerClockDivision kDiv2;
        static const TimControllerClockDivision kDiv4;
    };

    inline constexpr TimControllerClockDivision TimControllerClockDivision::kDiv1 = TimControllerClockDivision::from_ll<LL_TIM_CLOCKDIVISION_DIV1>();
    inline constexpr TimControllerClockDivision TimControllerClockDivision::kDiv2 = TimControllerClockDivision::from_ll<LL_TIM_CLOCKDIVISION_DIV2>();
    inline constexpr TimControllerClockDivision TimControllerClockDivision::kDiv4 = TimControllerClockDivision::from_ll<LL_TIM_CLOCKDIVISION_DIV4>();


    struct TimControllerUpdateSource : public LLDriverEnumValue<TimControllerUpdateSource>
    {
        static const TimControllerUpdateSource kRegular;
        static const TimControllerUpdateSource kCounter;
    };

    inline constexpr TimControllerUpdateSource TimControllerUpdateSource::kRegular = TimControllerUpdateSource::from_ll<LL_TIM_UPDATESOURCE_REGULAR>();
    inline constexpr TimControllerUpdateSource TimControllerUpdateSource::kCounter = TimControllerUpdateSource::from_ll<LL_TIM_UPDATESOURCE_COUNTER>();


    struct TimControllerDmaBurstBase : public LLDriverEnumValue<TimControllerDmaBurstBase>
    {
        static const TimControllerDmaBurstBase kControlRegister1;
        static const TimControllerDmaBurstBase kControlRegister2;
        static const TimControllerDmaBurstBase kSlaveModeControl;
        static const TimControllerDmaBurstBase kDmaInterruptEnable;
        static const TimControllerDmaBurstBase kStatus;
        static const TimControllerDmaBurstBase kEventGeneration;
        static const TimControllerDmaBurstBase kCaptureCompareMode1;
        static const TimControllerDmaBurstBase kCaptureCompareMode2;
        static const TimControllerDmaBurstBase kCaptureCompareEnable;
        static const TimControllerDmaBurstBase kCounter;
        static const TimControllerDmaBurstBase kPrescaler;
        static const TimControllerDmaBurstBase kAutoReload;
        static const TimControllerDmaBurstBase kRepetitionCounter;
        static const TimControllerDmaBurstBase kCaptureCompare1;
        static const TimControllerDmaBurstBase kCaptureCompare2;
        static const TimControllerDmaBurstBase kCaptureCompare3;
        static const TimControllerDmaBurstBase kCaptureCompare4;
        static const TimControllerDmaBurstBase kBreakDeadTime;
        static const TimControllerDmaBurstBase kOptionRegister;
        static const TimControllerDmaBurstBase kCaptureCompareMode3;
        static const TimControllerDmaBurstBase kCaptureCompare5;
        static const TimControllerDmaBurstBase kCaptureCompare6;
        static const TimControllerDmaBurstBase kAlternateFunction1;
        static const TimControllerDmaBurstBase kAlternateFunction2;
        static const TimControllerDmaBurstBase kTimerInputSelection;
    };

    inline constexpr TimControllerDmaBurstBase TimControllerDmaBurstBase::kControlRegister1 = TimControllerDmaBurstBase::from_ll<LL_TIM_DMABURST_BASEADDR_CR1>();
    inline constexpr TimControllerDmaBurstBase TimControllerDmaBurstBase::kControlRegister2 = TimControllerDmaBurstBase::from_ll<LL_TIM_DMABURST_BASEADDR_CR2>();
    inline constexpr TimControllerDmaBurstBase TimControllerDmaBurstBase::kSlaveModeControl = TimControllerDmaBurstBase::from_ll<LL_TIM_DMABURST_BASEADDR_SMCR>();
    inline constexpr TimControllerDmaBurstBase TimControllerDmaBurstBase::kDmaInterruptEnable = TimControllerDmaBurstBase::from_ll<LL_TIM_DMABURST_BASEADDR_DIER>();
    inline constexpr TimControllerDmaBurstBase TimControllerDmaBurstBase::kStatus = TimControllerDmaBurstBase::from_ll<LL_TIM_DMABURST_BASEADDR_SR>();
    inline constexpr TimControllerDmaBurstBase TimControllerDmaBurstBase::kEventGeneration = TimControllerDmaBurstBase::from_ll<LL_TIM_DMABURST_BASEADDR_EGR>();
    inline constexpr TimControllerDmaBurstBase TimControllerDmaBurstBase::kCaptureCompareMode1 = TimControllerDmaBurstBase::from_ll<LL_TIM_DMABURST_BASEADDR_CCMR1>();
    inline constexpr TimControllerDmaBurstBase TimControllerDmaBurstBase::kCaptureCompareMode2 = TimControllerDmaBurstBase::from_ll<LL_TIM_DMABURST_BASEADDR_CCMR2>();
    inline constexpr TimControllerDmaBurstBase TimControllerDmaBurstBase::kCaptureCompareEnable = TimControllerDmaBurstBase::from_ll<LL_TIM_DMABURST_BASEADDR_CCER>();
    inline constexpr TimControllerDmaBurstBase TimControllerDmaBurstBase::kCounter = TimControllerDmaBurstBase::from_ll<LL_TIM_DMABURST_BASEADDR_CNT>();
    inline constexpr TimControllerDmaBurstBase TimControllerDmaBurstBase::kPrescaler = TimControllerDmaBurstBase::from_ll<LL_TIM_DMABURST_BASEADDR_PSC>();
    inline constexpr TimControllerDmaBurstBase TimControllerDmaBurstBase::kAutoReload = TimControllerDmaBurstBase::from_ll<LL_TIM_DMABURST_BASEADDR_ARR>();
    inline constexpr TimControllerDmaBurstBase TimControllerDmaBurstBase::kRepetitionCounter = TimControllerDmaBurstBase::from_ll<LL_TIM_DMABURST_BASEADDR_RCR>();
    inline constexpr TimControllerDmaBurstBase TimControllerDmaBurstBase::kCaptureCompare1 = TimControllerDmaBurstBase::from_ll<LL_TIM_DMABURST_BASEADDR_CCR1>();
    inline constexpr TimControllerDmaBurstBase TimControllerDmaBurstBase::kCaptureCompare2 = TimControllerDmaBurstBase::from_ll<LL_TIM_DMABURST_BASEADDR_CCR2>();
    inline constexpr TimControllerDmaBurstBase TimControllerDmaBurstBase::kCaptureCompare3 = TimControllerDmaBurstBase::from_ll<LL_TIM_DMABURST_BASEADDR_CCR3>();
    inline constexpr TimControllerDmaBurstBase TimControllerDmaBurstBase::kCaptureCompare4 = TimControllerDmaBurstBase::from_ll<LL_TIM_DMABURST_BASEADDR_CCR4>();
    inline constexpr TimControllerDmaBurstBase TimControllerDmaBurstBase::kBreakDeadTime = TimControllerDmaBurstBase::from_ll<LL_TIM_DMABURST_BASEADDR_BDTR>();
    inline constexpr TimControllerDmaBurstBase TimControllerDmaBurstBase::kOptionRegister = TimControllerDmaBurstBase::from_ll<LL_TIM_DMABURST_BASEADDR_OR>();
    inline constexpr TimControllerDmaBurstBase TimControllerDmaBurstBase::kCaptureCompareMode3 = TimControllerDmaBurstBase::from_ll<LL_TIM_DMABURST_BASEADDR_CCMR3>();
    inline constexpr TimControllerDmaBurstBase TimControllerDmaBurstBase::kCaptureCompare5 = TimControllerDmaBurstBase::from_ll<LL_TIM_DMABURST_BASEADDR_CCR5>();
    inline constexpr TimControllerDmaBurstBase TimControllerDmaBurstBase::kCaptureCompare6 = TimControllerDmaBurstBase::from_ll<LL_TIM_DMABURST_BASEADDR_CCR6>();
    inline constexpr TimControllerDmaBurstBase TimControllerDmaBurstBase::kAlternateFunction1 = TimControllerDmaBurstBase::from_ll<LL_TIM_DMABURST_BASEADDR_AF1>();
    inline constexpr TimControllerDmaBurstBase TimControllerDmaBurstBase::kAlternateFunction2 = TimControllerDmaBurstBase::from_ll<LL_TIM_DMABURST_BASEADDR_AF2>();
    inline constexpr TimControllerDmaBurstBase TimControllerDmaBurstBase::kTimerInputSelection = TimControllerDmaBurstBase::from_ll<LL_TIM_DMABURST_BASEADDR_TISEL>();


    struct TimControllerDmaBurstLength : public LLDriverEnumValue<TimControllerDmaBurstLength>
    {
        static const TimControllerDmaBurstLength k1Transfer;
        static const TimControllerDmaBurstLength k2Transfers;
        static const TimControllerDmaBurstLength k3Transfers;
        static const TimControllerDmaBurstLength k4Transfers;
        static const TimControllerDmaBurstLength k18Transfers;
    };

    inline constexpr TimControllerDmaBurstLength TimControllerDmaBurstLength::k1Transfer = TimControllerDmaBurstLength::from_ll<LL_TIM_DMABURST_LENGTH_1TRANSFER>();
    inline constexpr TimControllerDmaBurstLength TimControllerDmaBurstLength::k2Transfers = TimControllerDmaBurstLength::from_ll<LL_TIM_DMABURST_LENGTH_2TRANSFERS>();
    inline constexpr TimControllerDmaBurstLength TimControllerDmaBurstLength::k3Transfers = TimControllerDmaBurstLength::from_ll<LL_TIM_DMABURST_LENGTH_3TRANSFERS>();
    inline constexpr TimControllerDmaBurstLength TimControllerDmaBurstLength::k4Transfers = TimControllerDmaBurstLength::from_ll<LL_TIM_DMABURST_LENGTH_4TRANSFERS>();
    inline constexpr TimControllerDmaBurstLength TimControllerDmaBurstLength::k18Transfers = TimControllerDmaBurstLength::from_ll<LL_TIM_DMABURST_LENGTH_18TRANSFERS>();


    struct TimControllerOnePulseMode : public LLDriverEnumValue<TimControllerOnePulseMode>
    {
        static const TimControllerOnePulseMode kRepetitive;
        static const TimControllerOnePulseMode kSingle;
    };

    inline constexpr TimControllerOnePulseMode TimControllerOnePulseMode::kRepetitive = TimControllerOnePulseMode::from_ll<LL_TIM_ONEPULSEMODE_REPETITIVE>();
    inline constexpr TimControllerOnePulseMode TimControllerOnePulseMode::kSingle = TimControllerOnePulseMode::from_ll<LL_TIM_ONEPULSEMODE_SINGLE>();


    struct TimControllerUpdateMode : public EnumValue<TimControllerUpdateMode, uint32_t>
    {
        static const TimControllerUpdateMode kImmediate;
        static const TimControllerUpdateMode kPreload;
    };

    inline constexpr TimControllerUpdateMode TimControllerUpdateMode::kImmediate = TimControllerUpdateMode::from_number<0>();
    inline constexpr TimControllerUpdateMode TimControllerUpdateMode::kPreload = TimControllerUpdateMode::from_number<1>();


    // -------------------------------------------------------------------------
    // ENCODER
    // -------------------------------------------------------------------------

    struct TimControllerEncoderMode : public LLDriverEnumValue<TimControllerEncoderMode>
    {
        static const TimControllerEncoderMode kX2TimerInput1;
        static const TimControllerEncoderMode kX2TimerInput2;
        static const TimControllerEncoderMode kX4TimerInput12;
        static const TimControllerEncoderMode kClockPlusDirectionX2;
        static const TimControllerEncoderMode kClockPlusDirectionX1;
        static const TimControllerEncoderMode kDirectionalClockX2;
        static const TimControllerEncoderMode kDirectionalClockX1TimerInput12;
        static const TimControllerEncoderMode kX1TimerInput1;
        static const TimControllerEncoderMode kX1TimerInput2;
    };

    inline constexpr TimControllerEncoderMode TimControllerEncoderMode::kX2TimerInput1 = TimControllerEncoderMode::from_ll<LL_TIM_ENCODERMODE_X2_TI1>();
    inline constexpr TimControllerEncoderMode TimControllerEncoderMode::kX2TimerInput2 = TimControllerEncoderMode::from_ll<LL_TIM_ENCODERMODE_X2_TI2>();
    inline constexpr TimControllerEncoderMode TimControllerEncoderMode::kX4TimerInput12 = TimControllerEncoderMode::from_ll<LL_TIM_ENCODERMODE_X4_TI12>();
    inline constexpr TimControllerEncoderMode TimControllerEncoderMode::kClockPlusDirectionX2 = TimControllerEncoderMode::from_ll<LL_TIM_ENCODERMODE_CLOCKPLUSDIRECTION_X2>();
    inline constexpr TimControllerEncoderMode TimControllerEncoderMode::kClockPlusDirectionX1 = TimControllerEncoderMode::from_ll<LL_TIM_ENCODERMODE_CLOCKPLUSDIRECTION_X1>();
    inline constexpr TimControllerEncoderMode TimControllerEncoderMode::kDirectionalClockX2 = TimControllerEncoderMode::from_ll<LL_TIM_ENCODERMODE_DIRECTIONALCLOCK_X2>();
    inline constexpr TimControllerEncoderMode TimControllerEncoderMode::kDirectionalClockX1TimerInput12 = TimControllerEncoderMode::from_ll<LL_TIM_ENCODERMODE_DIRECTIONALCLOCK_X1_TI12>();
    inline constexpr TimControllerEncoderMode TimControllerEncoderMode::kX1TimerInput1 = TimControllerEncoderMode::from_ll<LL_TIM_ENCODERMODE_X1_TI1>();
    inline constexpr TimControllerEncoderMode TimControllerEncoderMode::kX1TimerInput2 = TimControllerEncoderMode::from_ll<LL_TIM_ENCODERMODE_X1_TI2>();


    // -------------------------------------------------------------------------
    // INPUT CAPTURE
    // -------------------------------------------------------------------------

    struct TimChannelInputCapturePolarity : public LLDriverEnumValue<TimChannelInputCapturePolarity>
    {
        static const TimChannelInputCapturePolarity kRising;
        static const TimChannelInputCapturePolarity kFalling;
        static const TimChannelInputCapturePolarity kBothEdges;
    };

    inline constexpr TimChannelInputCapturePolarity TimChannelInputCapturePolarity::kRising = TimChannelInputCapturePolarity::from_ll<LL_TIM_IC_POLARITY_RISING>();
    inline constexpr TimChannelInputCapturePolarity TimChannelInputCapturePolarity::kFalling = TimChannelInputCapturePolarity::from_ll<LL_TIM_IC_POLARITY_FALLING>();
    inline constexpr TimChannelInputCapturePolarity TimChannelInputCapturePolarity::kBothEdges = TimChannelInputCapturePolarity::from_ll<LL_TIM_IC_POLARITY_BOTHEDGE>();


    struct TimChannelInputCaptureActiveInput : public LLDriverEnumValue<TimChannelInputCaptureActiveInput>
    {
        static const TimChannelInputCaptureActiveInput kDirectTimerInput;
        static const TimChannelInputCaptureActiveInput kIndirectTimerInput;
        static const TimChannelInputCaptureActiveInput kTimerReferenceClock;
    };

    inline constexpr TimChannelInputCaptureActiveInput TimChannelInputCaptureActiveInput::kDirectTimerInput = TimChannelInputCaptureActiveInput::from_ll<LL_TIM_ACTIVEINPUT_DIRECTTI>();
    inline constexpr TimChannelInputCaptureActiveInput TimChannelInputCaptureActiveInput::kIndirectTimerInput = TimChannelInputCaptureActiveInput::from_ll<LL_TIM_ACTIVEINPUT_INDIRECTTI>();
    inline constexpr TimChannelInputCaptureActiveInput TimChannelInputCaptureActiveInput::kTimerReferenceClock = TimChannelInputCaptureActiveInput::from_ll<LL_TIM_ACTIVEINPUT_TRC>();


    struct TimChannelInputCapturePrescaler : public LLDriverEnumValue<TimChannelInputCapturePrescaler>
    {
        static const TimChannelInputCapturePrescaler kDiv1;
        static const TimChannelInputCapturePrescaler kDiv2;
        static const TimChannelInputCapturePrescaler kDiv4;
        static const TimChannelInputCapturePrescaler kDiv8;
    };

    inline constexpr TimChannelInputCapturePrescaler TimChannelInputCapturePrescaler::kDiv1 = TimChannelInputCapturePrescaler::from_ll<LL_TIM_ICPSC_DIV1>();
    inline constexpr TimChannelInputCapturePrescaler TimChannelInputCapturePrescaler::kDiv2 = TimChannelInputCapturePrescaler::from_ll<LL_TIM_ICPSC_DIV2>();
    inline constexpr TimChannelInputCapturePrescaler TimChannelInputCapturePrescaler::kDiv4 = TimChannelInputCapturePrescaler::from_ll<LL_TIM_ICPSC_DIV4>();
    inline constexpr TimChannelInputCapturePrescaler TimChannelInputCapturePrescaler::kDiv8 = TimChannelInputCapturePrescaler::from_ll<LL_TIM_ICPSC_DIV8>();


    struct TimChannelInputCaptureFilter : public LLDriverEnumValue<TimChannelInputCaptureFilter>
    {
        static const TimChannelInputCaptureFilter kNone;
        static const TimChannelInputCaptureFilter kFreqDiv1N2Samples;
        static const TimChannelInputCaptureFilter kFreqDiv1N4Samples;
        static const TimChannelInputCaptureFilter kFreqDiv1N8Samples;
        static const TimChannelInputCaptureFilter kFreqDiv2N6Samples;
        static const TimChannelInputCaptureFilter kFreqDiv2N8Samples;
        static const TimChannelInputCaptureFilter kFreqDiv4N6Samples;
        static const TimChannelInputCaptureFilter kFreqDiv4N8Samples;
        static const TimChannelInputCaptureFilter kFreqDiv8N6Samples;
        static const TimChannelInputCaptureFilter kFreqDiv8N8Samples;
        static const TimChannelInputCaptureFilter kFreqDiv16N5Samples;
        static const TimChannelInputCaptureFilter kFreqDiv16N6Samples;
        static const TimChannelInputCaptureFilter kFreqDiv16N8Samples;
        static const TimChannelInputCaptureFilter kFreqDiv32N5Samples;
        static const TimChannelInputCaptureFilter kFreqDiv32N6Samples;
        static const TimChannelInputCaptureFilter kFreqDiv32N8Samples;
    };

    inline constexpr TimChannelInputCaptureFilter TimChannelInputCaptureFilter::kNone = TimChannelInputCaptureFilter::from_ll<LL_TIM_IC_FILTER_FDIV1>();
    inline constexpr TimChannelInputCaptureFilter TimChannelInputCaptureFilter::kFreqDiv1N2Samples = TimChannelInputCaptureFilter::from_ll<LL_TIM_IC_FILTER_FDIV1_N2>();
    inline constexpr TimChannelInputCaptureFilter TimChannelInputCaptureFilter::kFreqDiv1N4Samples = TimChannelInputCaptureFilter::from_ll<LL_TIM_IC_FILTER_FDIV1_N4>();
    inline constexpr TimChannelInputCaptureFilter TimChannelInputCaptureFilter::kFreqDiv1N8Samples = TimChannelInputCaptureFilter::from_ll<LL_TIM_IC_FILTER_FDIV1_N8>();
    inline constexpr TimChannelInputCaptureFilter TimChannelInputCaptureFilter::kFreqDiv2N6Samples = TimChannelInputCaptureFilter::from_ll<LL_TIM_IC_FILTER_FDIV2_N6>();
    inline constexpr TimChannelInputCaptureFilter TimChannelInputCaptureFilter::kFreqDiv2N8Samples = TimChannelInputCaptureFilter::from_ll<LL_TIM_IC_FILTER_FDIV2_N8>();
    inline constexpr TimChannelInputCaptureFilter TimChannelInputCaptureFilter::kFreqDiv4N6Samples = TimChannelInputCaptureFilter::from_ll<LL_TIM_IC_FILTER_FDIV4_N6>();
    inline constexpr TimChannelInputCaptureFilter TimChannelInputCaptureFilter::kFreqDiv4N8Samples = TimChannelInputCaptureFilter::from_ll<LL_TIM_IC_FILTER_FDIV4_N8>();
    inline constexpr TimChannelInputCaptureFilter TimChannelInputCaptureFilter::kFreqDiv8N6Samples = TimChannelInputCaptureFilter::from_ll<LL_TIM_IC_FILTER_FDIV8_N6>();
    inline constexpr TimChannelInputCaptureFilter TimChannelInputCaptureFilter::kFreqDiv8N8Samples = TimChannelInputCaptureFilter::from_ll<LL_TIM_IC_FILTER_FDIV8_N8>();
    inline constexpr TimChannelInputCaptureFilter TimChannelInputCaptureFilter::kFreqDiv16N5Samples = TimChannelInputCaptureFilter::from_ll<LL_TIM_IC_FILTER_FDIV16_N5>();
    inline constexpr TimChannelInputCaptureFilter TimChannelInputCaptureFilter::kFreqDiv16N6Samples = TimChannelInputCaptureFilter::from_ll<LL_TIM_IC_FILTER_FDIV16_N6>();
    inline constexpr TimChannelInputCaptureFilter TimChannelInputCaptureFilter::kFreqDiv16N8Samples = TimChannelInputCaptureFilter::from_ll<LL_TIM_IC_FILTER_FDIV16_N8>();
    inline constexpr TimChannelInputCaptureFilter TimChannelInputCaptureFilter::kFreqDiv32N5Samples = TimChannelInputCaptureFilter::from_ll<LL_TIM_IC_FILTER_FDIV32_N5>();
    inline constexpr TimChannelInputCaptureFilter TimChannelInputCaptureFilter::kFreqDiv32N6Samples = TimChannelInputCaptureFilter::from_ll<LL_TIM_IC_FILTER_FDIV32_N6>();
    inline constexpr TimChannelInputCaptureFilter TimChannelInputCaptureFilter::kFreqDiv32N8Samples = TimChannelInputCaptureFilter::from_ll<LL_TIM_IC_FILTER_FDIV32_N8>();


    // -------------------------------------------------------------------------
    // OUTPUT COMPARE
    // -------------------------------------------------------------------------

    struct TimChannelOutputCaptureMode : public LLDriverEnumValue<TimChannelOutputCaptureMode>
    {
        static const TimChannelOutputCaptureMode kFrozen;
        static const TimChannelOutputCaptureMode kActive;
        static const TimChannelOutputCaptureMode kInactive;
        static const TimChannelOutputCaptureMode kToggle;
        static const TimChannelOutputCaptureMode kForcedInactive;
        static const TimChannelOutputCaptureMode kForcedActive;
        static const TimChannelOutputCaptureMode kPwm1;
        static const TimChannelOutputCaptureMode kPwm2;
        static const TimChannelOutputCaptureMode kRetriggerableOnePulseMode1;
        static const TimChannelOutputCaptureMode kRetriggerableOnePulseMode2;
        static const TimChannelOutputCaptureMode kCombinedPwm1;
        static const TimChannelOutputCaptureMode kCombinedPwm2;
        static const TimChannelOutputCaptureMode kAsymmetricPwm1;
        static const TimChannelOutputCaptureMode kAsymmetricPwm2;
        static const TimChannelOutputCaptureMode kDirectionOutput;
    };

    inline constexpr TimChannelOutputCaptureMode TimChannelOutputCaptureMode::kFrozen = TimChannelOutputCaptureMode::from_ll<LL_TIM_OCMODE_FROZEN>();
    inline constexpr TimChannelOutputCaptureMode TimChannelOutputCaptureMode::kActive = TimChannelOutputCaptureMode::from_ll<LL_TIM_OCMODE_ACTIVE>();
    inline constexpr TimChannelOutputCaptureMode TimChannelOutputCaptureMode::kInactive = TimChannelOutputCaptureMode::from_ll<LL_TIM_OCMODE_INACTIVE>();
    inline constexpr TimChannelOutputCaptureMode TimChannelOutputCaptureMode::kToggle = TimChannelOutputCaptureMode::from_ll<LL_TIM_OCMODE_TOGGLE>();
    inline constexpr TimChannelOutputCaptureMode TimChannelOutputCaptureMode::kForcedInactive = TimChannelOutputCaptureMode::from_ll<LL_TIM_OCMODE_FORCED_INACTIVE>();
    inline constexpr TimChannelOutputCaptureMode TimChannelOutputCaptureMode::kForcedActive = TimChannelOutputCaptureMode::from_ll<LL_TIM_OCMODE_FORCED_ACTIVE>();
    inline constexpr TimChannelOutputCaptureMode TimChannelOutputCaptureMode::kPwm1 = TimChannelOutputCaptureMode::from_ll<LL_TIM_OCMODE_PWM1>();
    inline constexpr TimChannelOutputCaptureMode TimChannelOutputCaptureMode::kPwm2 = TimChannelOutputCaptureMode::from_ll<LL_TIM_OCMODE_PWM2>();
    inline constexpr TimChannelOutputCaptureMode TimChannelOutputCaptureMode::kRetriggerableOnePulseMode1 = TimChannelOutputCaptureMode::from_ll<LL_TIM_OCMODE_RETRIG_OPM1>();
    inline constexpr TimChannelOutputCaptureMode TimChannelOutputCaptureMode::kRetriggerableOnePulseMode2 = TimChannelOutputCaptureMode::from_ll<LL_TIM_OCMODE_RETRIG_OPM2>();
    inline constexpr TimChannelOutputCaptureMode TimChannelOutputCaptureMode::kCombinedPwm1 = TimChannelOutputCaptureMode::from_ll<LL_TIM_OCMODE_COMBINED_PWM1>();
    inline constexpr TimChannelOutputCaptureMode TimChannelOutputCaptureMode::kCombinedPwm2 = TimChannelOutputCaptureMode::from_ll<LL_TIM_OCMODE_COMBINED_PWM2>();
    inline constexpr TimChannelOutputCaptureMode TimChannelOutputCaptureMode::kAsymmetricPwm1 = TimChannelOutputCaptureMode::from_ll<LL_TIM_OCMODE_ASYMMETRIC_PWM1>();
    inline constexpr TimChannelOutputCaptureMode TimChannelOutputCaptureMode::kAsymmetricPwm2 = TimChannelOutputCaptureMode::from_ll<LL_TIM_OCMODE_ASYMMETRIC_PWM2>();
    inline constexpr TimChannelOutputCaptureMode TimChannelOutputCaptureMode::kDirectionOutput = TimChannelOutputCaptureMode::from_ll<LL_TIM_OCMODE_DIRECTION_OUTPUT>();


    struct TimChannelOutputCapturePolarity : public LLDriverEnumValue<TimChannelOutputCapturePolarity>
    {
        static const TimChannelOutputCapturePolarity kHigh;
        static const TimChannelOutputCapturePolarity kLow;
    };

    inline constexpr TimChannelOutputCapturePolarity TimChannelOutputCapturePolarity::kHigh = TimChannelOutputCapturePolarity::from_ll<LL_TIM_OCPOLARITY_HIGH>();
    inline constexpr TimChannelOutputCapturePolarity TimChannelOutputCapturePolarity::kLow = TimChannelOutputCapturePolarity::from_ll<LL_TIM_OCPOLARITY_LOW>();


    struct TimChannelOutputCaptureIdleState : public LLDriverEnumValue<TimChannelOutputCaptureIdleState>
    {
        static const TimChannelOutputCaptureIdleState kLow;
        static const TimChannelOutputCaptureIdleState kHigh;
    };

    inline constexpr TimChannelOutputCaptureIdleState TimChannelOutputCaptureIdleState::kLow = TimChannelOutputCaptureIdleState::from_ll<LL_TIM_OCIdLESTATE_LOW>();
    inline constexpr TimChannelOutputCaptureIdleState TimChannelOutputCaptureIdleState::kHigh = TimChannelOutputCaptureIdleState::from_ll<LL_TIM_OCIdLESTATE_HIGH>();


    // -------------------------------------------------------------------------
    // MASTER / SLAVE / TRIGGER ENUMS
    // -------------------------------------------------------------------------

    struct TimControllerTriggerOutputSource : public LLDriverEnumValue<TimControllerTriggerOutputSource>
    {
        static const TimControllerTriggerOutputSource kReset;
        static const TimControllerTriggerOutputSource kEnable;
        static const TimControllerTriggerOutputSource kUpdate;
        static const TimControllerTriggerOutputSource kCompareChannel1Flag;
        static const TimControllerTriggerOutputSource kOutputCompare1Ref;
        static const TimControllerTriggerOutputSource kOutputCompare2Ref;
        static const TimControllerTriggerOutputSource kOutputCompare3Ref;
        static const TimControllerTriggerOutputSource kOutputCompare4Ref;
        static const TimControllerTriggerOutputSource kEncoderClockSource;
    };

    inline constexpr TimControllerTriggerOutputSource TimControllerTriggerOutputSource::kReset = TimControllerTriggerOutputSource::from_ll<LL_TIM_TRGO_RESET>();
    inline constexpr TimControllerTriggerOutputSource TimControllerTriggerOutputSource::kEnable = TimControllerTriggerOutputSource::from_ll<LL_TIM_TRGO_ENABLE>();
    inline constexpr TimControllerTriggerOutputSource TimControllerTriggerOutputSource::kUpdate = TimControllerTriggerOutputSource::from_ll<LL_TIM_TRGO_UPDATE>();
    inline constexpr TimControllerTriggerOutputSource TimControllerTriggerOutputSource::kCompareChannel1Flag = TimControllerTriggerOutputSource::from_ll<LL_TIM_TRGO_CC1IF>();
    inline constexpr TimControllerTriggerOutputSource TimControllerTriggerOutputSource::kOutputCompare1Ref = TimControllerTriggerOutputSource::from_ll<LL_TIM_TRGO_OC1REF>();
    inline constexpr TimControllerTriggerOutputSource TimControllerTriggerOutputSource::kOutputCompare2Ref = TimControllerTriggerOutputSource::from_ll<LL_TIM_TRGO_OC2REF>();
    inline constexpr TimControllerTriggerOutputSource TimControllerTriggerOutputSource::kOutputCompare3Ref = TimControllerTriggerOutputSource::from_ll<LL_TIM_TRGO_OC3REF>();
    inline constexpr TimControllerTriggerOutputSource TimControllerTriggerOutputSource::kOutputCompare4Ref = TimControllerTriggerOutputSource::from_ll<LL_TIM_TRGO_OC4REF>();
    inline constexpr TimControllerTriggerOutputSource TimControllerTriggerOutputSource::kEncoderClockSource = TimControllerTriggerOutputSource::from_ll<LL_TIM_TRGO_ENCODERCLK>();


    struct TimControllerTriggerOutput2Source : public LLDriverEnumValue<TimControllerTriggerOutput2Source>
    {
        static const TimControllerTriggerOutput2Source kReset;
        static const TimControllerTriggerOutput2Source kEnable;
        static const TimControllerTriggerOutput2Source kUpdate;
        static const TimControllerTriggerOutput2Source kCompareChannel1Flag;
        static const TimControllerTriggerOutput2Source kOutputCompare1Ref;
        static const TimControllerTriggerOutput2Source kOutputCompare2Ref;
        static const TimControllerTriggerOutput2Source kOutputCompare3Ref;
        static const TimControllerTriggerOutput2Source kOutputCompare4Ref;
        static const TimControllerTriggerOutput2Source kOutputCompare5Ref;
        static const TimControllerTriggerOutput2Source kOutputCompare6Ref;
        static const TimControllerTriggerOutput2Source kOutputCompare4RisingFallingEdge;
        static const TimControllerTriggerOutput2Source kOutputCompare6RisingFallingEdge;
        static const TimControllerTriggerOutput2Source kOutputCompare4RisingOutputCompare6Rising;
        static const TimControllerTriggerOutput2Source kOutputCompare4RisingOutputCompare6Falling;
        static const TimControllerTriggerOutput2Source kOutputCompare5RisingOutputCompare6Rising;
        static const TimControllerTriggerOutput2Source kOutputCompare5RisingOutputCompare6Falling;
    };

    inline constexpr TimControllerTriggerOutput2Source TimControllerTriggerOutput2Source::kReset = TimControllerTriggerOutput2Source::from_ll<LL_TIM_TRGO2_RESET>();
    inline constexpr TimControllerTriggerOutput2Source TimControllerTriggerOutput2Source::kEnable = TimControllerTriggerOutput2Source::from_ll<LL_TIM_TRGO2_ENABLE>();
    inline constexpr TimControllerTriggerOutput2Source TimControllerTriggerOutput2Source::kUpdate = TimControllerTriggerOutput2Source::from_ll<LL_TIM_TRGO2_UPDATE>();
    inline constexpr TimControllerTriggerOutput2Source TimControllerTriggerOutput2Source::kCompareChannel1Flag = TimControllerTriggerOutput2Source::from_ll<LL_TIM_TRGO2_CC1F>();
    inline constexpr TimControllerTriggerOutput2Source TimControllerTriggerOutput2Source::kOutputCompare1Ref = TimControllerTriggerOutput2Source::from_ll<LL_TIM_TRGO2_OC1>();
    inline constexpr TimControllerTriggerOutput2Source TimControllerTriggerOutput2Source::kOutputCompare2Ref = TimControllerTriggerOutput2Source::from_ll<LL_TIM_TRGO2_OC2>();
    inline constexpr TimControllerTriggerOutput2Source TimControllerTriggerOutput2Source::kOutputCompare3Ref = TimControllerTriggerOutput2Source::from_ll<LL_TIM_TRGO2_OC3>();
    inline constexpr TimControllerTriggerOutput2Source TimControllerTriggerOutput2Source::kOutputCompare4Ref = TimControllerTriggerOutput2Source::from_ll<LL_TIM_TRGO2_OC4>();
    inline constexpr TimControllerTriggerOutput2Source TimControllerTriggerOutput2Source::kOutputCompare5Ref = TimControllerTriggerOutput2Source::from_ll<LL_TIM_TRGO2_OC5>();
    inline constexpr TimControllerTriggerOutput2Source TimControllerTriggerOutput2Source::kOutputCompare6Ref = TimControllerTriggerOutput2Source::from_ll<LL_TIM_TRGO2_OC6>();
    inline constexpr TimControllerTriggerOutput2Source TimControllerTriggerOutput2Source::kOutputCompare4RisingFallingEdge = TimControllerTriggerOutput2Source::from_ll<LL_TIM_TRGO2_OC4_RISINGFALLING>();
    inline constexpr TimControllerTriggerOutput2Source TimControllerTriggerOutput2Source::kOutputCompare6RisingFallingEdge = TimControllerTriggerOutput2Source::from_ll<LL_TIM_TRGO2_OC6_RISINGFALLING>();
    inline constexpr TimControllerTriggerOutput2Source TimControllerTriggerOutput2Source::kOutputCompare4RisingOutputCompare6Rising = TimControllerTriggerOutput2Source::from_ll<LL_TIM_TRGO2_OC4_RISING_OC6_RISING>();
    inline constexpr TimControllerTriggerOutput2Source TimControllerTriggerOutput2Source::kOutputCompare4RisingOutputCompare6Falling = TimControllerTriggerOutput2Source::from_ll<LL_TIM_TRGO2_OC4_RISING_OC6_FALLING>();
    inline constexpr TimControllerTriggerOutput2Source TimControllerTriggerOutput2Source::kOutputCompare5RisingOutputCompare6Rising = TimControllerTriggerOutput2Source::from_ll<LL_TIM_TRGO2_OC5_RISING_OC6_RISING>();
    inline constexpr TimControllerTriggerOutput2Source TimControllerTriggerOutput2Source::kOutputCompare5RisingOutputCompare6Falling = TimControllerTriggerOutput2Source::from_ll<LL_TIM_TRGO2_OC5_RISING_OC6_FALLING>();


    struct TimControllerSlaveMode : public LLDriverEnumValue<TimControllerSlaveMode>
    {
        static const TimControllerSlaveMode kDisabled;
        static const TimControllerSlaveMode kReset;
        static const TimControllerSlaveMode kGated;
        static const TimControllerSlaveMode kTrigger;
        static const TimControllerSlaveMode kResetTrigger;
        static const TimControllerSlaveMode kGatedReset;
    };

    inline constexpr TimControllerSlaveMode TimControllerSlaveMode::kDisabled = TimControllerSlaveMode::from_ll<LL_TIM_SLAVEMODE_DISABLED>();
    inline constexpr TimControllerSlaveMode TimControllerSlaveMode::kReset = TimControllerSlaveMode::from_ll<LL_TIM_SLAVEMODE_RESET>();
    inline constexpr TimControllerSlaveMode TimControllerSlaveMode::kGated = TimControllerSlaveMode::from_ll<LL_TIM_SLAVEMODE_GATED>();
    inline constexpr TimControllerSlaveMode TimControllerSlaveMode::kTrigger = TimControllerSlaveMode::from_ll<LL_TIM_SLAVEMODE_TRIGGER>();
    inline constexpr TimControllerSlaveMode TimControllerSlaveMode::kResetTrigger = TimControllerSlaveMode::from_ll<LL_TIM_SLAVEMODE_COMBINED_RESETTRIGGER>();
    inline constexpr TimControllerSlaveMode TimControllerSlaveMode::kGatedReset = TimControllerSlaveMode::from_ll<LL_TIM_SLAVEMODE_COMBINED_GATEDRESET>();


    struct TimControllerTriggerSource : public LLDriverEnumValue<TimControllerTriggerSource>
    {
        static const TimControllerTriggerSource kInternalTrigger0;
        static const TimControllerTriggerSource kInternalTrigger1;
        static const TimControllerTriggerSource kInternalTrigger2;
        static const TimControllerTriggerSource kInternalTrigger3;
        static const TimControllerTriggerSource kInternalTrigger4;
        static const TimControllerTriggerSource kInternalTrigger5;
        static const TimControllerTriggerSource kInternalTrigger6;
        static const TimControllerTriggerSource kInternalTrigger7;
        static const TimControllerTriggerSource kInternalTrigger8;
        static const TimControllerTriggerSource kInternalTrigger9;
        static const TimControllerTriggerSource kInternalTrigger10;
        static const TimControllerTriggerSource kInternalTrigger11;
        static const TimControllerTriggerSource kTimerInput1FallingEdgeDetection;
        static const TimControllerTriggerSource kTimerInput1FilteredInput1;
        static const TimControllerTriggerSource kTimerInput2FilteredInput2;
        static const TimControllerTriggerSource kExternalTriggerFiltered;
    };

    inline constexpr TimControllerTriggerSource TimControllerTriggerSource::kInternalTrigger0 = TimControllerTriggerSource::from_ll<LL_TIM_TS_ITR0>();
    inline constexpr TimControllerTriggerSource TimControllerTriggerSource::kInternalTrigger1 = TimControllerTriggerSource::from_ll<LL_TIM_TS_ITR1>();
    inline constexpr TimControllerTriggerSource TimControllerTriggerSource::kInternalTrigger2 = TimControllerTriggerSource::from_ll<LL_TIM_TS_ITR2>();
    inline constexpr TimControllerTriggerSource TimControllerTriggerSource::kInternalTrigger3 = TimControllerTriggerSource::from_ll<LL_TIM_TS_ITR3>();
    inline constexpr TimControllerTriggerSource TimControllerTriggerSource::kInternalTrigger4 = TimControllerTriggerSource::from_ll<LL_TIM_TS_ITR4>();
    inline constexpr TimControllerTriggerSource TimControllerTriggerSource::kInternalTrigger5 = TimControllerTriggerSource::from_ll<LL_TIM_TS_ITR5>();
    inline constexpr TimControllerTriggerSource TimControllerTriggerSource::kInternalTrigger6 = TimControllerTriggerSource::from_ll<LL_TIM_TS_ITR6>();
    inline constexpr TimControllerTriggerSource TimControllerTriggerSource::kInternalTrigger7 = TimControllerTriggerSource::from_ll<LL_TIM_TS_ITR7>();
    inline constexpr TimControllerTriggerSource TimControllerTriggerSource::kInternalTrigger8 = TimControllerTriggerSource::from_ll<LL_TIM_TS_ITR8>();
    inline constexpr TimControllerTriggerSource TimControllerTriggerSource::kInternalTrigger9 = TimControllerTriggerSource::from_ll<LL_TIM_TS_ITR9>();
    inline constexpr TimControllerTriggerSource TimControllerTriggerSource::kInternalTrigger10 = TimControllerTriggerSource::from_ll<LL_TIM_TS_ITR10>();
    inline constexpr TimControllerTriggerSource TimControllerTriggerSource::kInternalTrigger11 = TimControllerTriggerSource::from_ll<LL_TIM_TS_ITR11>();
    inline constexpr TimControllerTriggerSource TimControllerTriggerSource::kTimerInput1FallingEdgeDetection = TimControllerTriggerSource::from_ll<LL_TIM_TS_TI1F_ED>();
    inline constexpr TimControllerTriggerSource TimControllerTriggerSource::kTimerInput1FilteredInput1 = TimControllerTriggerSource::from_ll<LL_TIM_TS_TI1FP1>();
    inline constexpr TimControllerTriggerSource TimControllerTriggerSource::kTimerInput2FilteredInput2 = TimControllerTriggerSource::from_ll<LL_TIM_TS_TI2FP2>();
    inline constexpr TimControllerTriggerSource TimControllerTriggerSource::kExternalTriggerFiltered = TimControllerTriggerSource::from_ll<LL_TIM_TS_ETRF>();


    // -------------------------------------------------------------------------
    // BREAK ENUMS
    // -------------------------------------------------------------------------

    struct TimControllerBreakPolarity : public LLDriverEnumValue<TimControllerBreakPolarity>
    {
        static const TimControllerBreakPolarity kLow;
        static const TimControllerBreakPolarity kHigh;
    };

    inline constexpr TimControllerBreakPolarity TimControllerBreakPolarity::kLow = TimControllerBreakPolarity::from_ll<LL_TIM_BREAK_POLARITY_LOW>();
    inline constexpr TimControllerBreakPolarity TimControllerBreakPolarity::kHigh = TimControllerBreakPolarity::from_ll<LL_TIM_BREAK_POLARITY_HIGH>();


    struct TimControllerBreakAFMode : public LLDriverEnumValue<TimControllerBreakAFMode>
    {
        static const TimControllerBreakAFMode kInput;
        static const TimControllerBreakAFMode kBidirectional;
    };

    inline constexpr TimControllerBreakAFMode TimControllerBreakAFMode::kInput = TimControllerBreakAFMode::from_ll<LL_TIM_BREAK_AFMODE_INPUT>();
    inline constexpr TimControllerBreakAFMode TimControllerBreakAFMode::kBidirectional = TimControllerBreakAFMode::from_ll<LL_TIM_BREAK_AFMODE_BIdIRECTIONAL>();


    struct TimControllerOffState : public LLDriverEnumValue<TimControllerOffState>
    {
        static const TimControllerOffState kRun;
        static const TimControllerOffState kIdle;
    };

    inline constexpr TimControllerOffState TimControllerOffState::kRun = TimControllerOffState::from_ll<LL_TIM_OSSR_ENABLE>();
    inline constexpr TimControllerOffState TimControllerOffState::kIdle = TimControllerOffState::from_ll<LL_TIM_OSSI_ENABLE>();


    struct TimControllerLockLevel : public LLDriverEnumValue<TimControllerLockLevel>
    {
        static const TimControllerLockLevel kOff;
        static const TimControllerLockLevel k1;
        static const TimControllerLockLevel k2;
        static const TimControllerLockLevel k3;
    };

    inline constexpr TimControllerLockLevel TimControllerLockLevel::kOff = TimControllerLockLevel::from_ll<LL_TIM_LOCKLEVEL_OFF>();
    inline constexpr TimControllerLockLevel TimControllerLockLevel::k1 = TimControllerLockLevel::from_ll<LL_TIM_LOCKLEVEL_1>();
    inline constexpr TimControllerLockLevel TimControllerLockLevel::k2 = TimControllerLockLevel::from_ll<LL_TIM_LOCKLEVEL_2>();
    inline constexpr TimControllerLockLevel TimControllerLockLevel::k3 = TimControllerLockLevel::from_ll<LL_TIM_LOCKLEVEL_3>();


    struct TimControllerBreakInput : public LLDriverEnumValue<TimControllerBreakInput>
    {
        static const TimControllerBreakInput kBreakInput;
        static const TimControllerBreakInput kBreakInput2;
    };

    inline constexpr TimControllerBreakInput TimControllerBreakInput::kBreakInput = TimControllerBreakInput::from_ll<LL_TIM_BREAK_INPUT_BKIN>();
    inline constexpr TimControllerBreakInput TimControllerBreakInput::kBreakInput2 = TimControllerBreakInput::from_ll<LL_TIM_BREAK_INPUT_BKIN2>();


    struct TimControllerBreakSource : public LLDriverEnumValue<TimControllerBreakSource>
    {
        static const TimControllerBreakSource kBreakInput;
        static const TimControllerBreakSource kComparator1;
        static const TimControllerBreakSource kComparator2;
        static const TimControllerBreakSource kComparator3;
        static const TimControllerBreakSource kComparator4;
        static const TimControllerBreakSource kComparator5;
        static const TimControllerBreakSource kComparator6;
        static const TimControllerBreakSource kComparator7;
    };

    inline constexpr TimControllerBreakSource TimControllerBreakSource::kBreakInput = TimControllerBreakSource::from_ll<LL_TIM_BKIN_SOURCE_BKIN>();
    inline constexpr TimControllerBreakSource TimControllerBreakSource::kComparator1 = TimControllerBreakSource::from_ll<LL_TIM_BKIN_SOURCE_BKCOMP1>();
    inline constexpr TimControllerBreakSource TimControllerBreakSource::kComparator2 = TimControllerBreakSource::from_ll<LL_TIM_BKIN_SOURCE_BKCOMP2>();
    inline constexpr TimControllerBreakSource TimControllerBreakSource::kComparator3 = TimControllerBreakSource::from_ll<LL_TIM_BKIN_SOURCE_BKCOMP3>();
    inline constexpr TimControllerBreakSource TimControllerBreakSource::kComparator4 = TimControllerBreakSource::from_ll<LL_TIM_BKIN_SOURCE_BKCOMP4>();
    inline constexpr TimControllerBreakSource TimControllerBreakSource::kComparator5 = TimControllerBreakSource::from_ll<LL_TIM_BKIN_SOURCE_BKCOMP5>();
    inline constexpr TimControllerBreakSource TimControllerBreakSource::kComparator6 = TimControllerBreakSource::from_ll<LL_TIM_BKIN_SOURCE_BKCOMP6>();
    inline constexpr TimControllerBreakSource TimControllerBreakSource::kComparator7 = TimControllerBreakSource::from_ll<LL_TIM_BKIN_SOURCE_BKCOMP7>();


    // =========================================================================
    // TRAITS
    // =========================================================================

    // -------------------------------------------------------------------------
    // CONTROLLER
    // -------------------------------------------------------------------------

    template <TimControllerId tkControllerId>
    struct TimControllerTraits;

#define DEFINE_TIM_TRAITS(tkControllerId, num, bus, counter_type)         \
    template <>                                                           \
    struct TimControllerTraits<tkControllerId>                            \
    {                                                                     \
        using CounterValueT                         = counter_type;       \
        static inline TIM_TypeDef* const skInstance = TIM##num;           \
        inline void                      enable_clock() const             \
        {                                                                 \
            LL_##bus##_GRP1_EnableClock(LL_##bus##_GRP1_PERIPH_TIM##num); \
        }                                                                 \
    };

    DEFINE_TIM_TRAITS(TimControllerId::kTim1, 1, APB2, uint16_t);    // Advanced 16-bit
    DEFINE_TIM_TRAITS(TimControllerId::kTim2, 2, APB1, uint32_t);    // General Purpose 32-bit
    DEFINE_TIM_TRAITS(TimControllerId::kTim3, 3, APB1, uint16_t);    // General Purpose 16-bit
    DEFINE_TIM_TRAITS(TimControllerId::kTim4, 4, APB1, uint16_t);    // General Purpose 16-bit
    DEFINE_TIM_TRAITS(TimControllerId::kTim5, 5, APB1, uint32_t);    // General Purpose 32-bit
    DEFINE_TIM_TRAITS(TimControllerId::kTim6, 6, APB1, uint16_t);    // Basic 16-bit
    DEFINE_TIM_TRAITS(TimControllerId::kTim7, 7, APB1, uint16_t);    // Basic 16-bit
    DEFINE_TIM_TRAITS(TimControllerId::kTim8, 8, APB2, uint16_t);    // Advanced 16-bit
    DEFINE_TIM_TRAITS(TimControllerId::kTim15, 15, APB2, uint16_t);  // General Purpose 16-bit
    DEFINE_TIM_TRAITS(TimControllerId::kTim16, 16, APB2, uint16_t);  // General Purpose 16-bit
    DEFINE_TIM_TRAITS(TimControllerId::kTim17, 17, APB2, uint16_t);  // General Purpose 16-bit
    DEFINE_TIM_TRAITS(TimControllerId::kTim20, 20, APB2, uint16_t);  // Advanced 16-bit

#undef DEFINE_TIM_TRAITS

    // -------------------------------------------------------------------------
    // TIMER TRAITS
    // -------------------------------------------------------------------------
    template <TimControllerId tkControllerId, TimChannelId tkChannelId>
    struct TimChannelTraits;

    template <TimControllerId tkControllerId>
    struct TimChannelTraits<tkControllerId, TimChannelId::kCh1>
    {
        static constexpr uint32_t skLLChannelId              = LL_TIM_CHANNEL_CH1;
        static constexpr bool     skHasComplementaryChannel  = true;
        static constexpr uint32_t skLLComplementaryChannelId = LL_TIM_CHANNEL_CH1N;
    };

    template <TimControllerId tkControllerId>
    struct TimChannelTraits<tkControllerId, TimChannelId::kCh2>
    {
        static constexpr uint32_t skLLChannelId              = LL_TIM_CHANNEL_CH2;
        static constexpr bool     skHasComplementaryChannel  = true;
        static constexpr uint32_t skLLComplementaryChannelId = LL_TIM_CHANNEL_CH2N;
    };

    template <TimControllerId tkControllerId>
    struct TimChannelTraits<tkControllerId, TimChannelId::kCh3>
    {
        static constexpr uint32_t skLLChannelId              = LL_TIM_CHANNEL_CH3;
        static constexpr bool     skHasComplementaryChannel  = true;
        static constexpr uint32_t skLLComplementaryChannelId = LL_TIM_CHANNEL_CH3N;
    };

    template <TimControllerId tkControllerId>
    struct TimChannelTraits<tkControllerId, TimChannelId::kCh4>
    {
        static constexpr uint32_t skLLChannelId              = LL_TIM_CHANNEL_CH4;
        static constexpr bool     skHasComplementaryChannel  = true;
        static constexpr uint32_t skLLComplementaryChannelId = LL_TIM_CHANNEL_CH4N;
    };

    template <TimControllerId tkControllerId>
    struct TimChannelTraits<tkControllerId, TimChannelId::kCh5>
    {
        static constexpr uint32_t skLLChannelId             = LL_TIM_CHANNEL_CH5;
        static constexpr bool     skHasComplementaryChannel = true;
    };

    template <TimControllerId tkControllerId>
    struct TimChannelTraits<tkControllerId, TimChannelId::kCh6>
    {
        static constexpr uint32_t skLLChannelId             = LL_TIM_CHANNEL_CH6;
        static constexpr bool     skHasComplementaryChannel = true;
    };

    // ============================================================================
    // HARDWARE INTERFACES
    // ============================================================================

    template <TimControllerId tkControllerId>
    struct TimControllerInterface
    {
        using ControllerTraitsT = TimControllerTraits<tkControllerId>;
        using CounterValueT     = typename ControllerTraitsT::CounterValueT;

        // ---------------------------------------------------------------------
        // COUNTER CONTROL
        // ---------------------------------------------------------------------

        void enable_counter() const
        {
            LL_TIM_EnableCounter(ControllerTraitsT::skInstance);
        }
        void disable_counter() const
        {
            LL_TIM_DisableCounter(ControllerTraitsT::skInstance);
        }
        CounterValueT get_counter() const
        {
            return LL_TIM_GetCounter(ControllerTraitsT::skInstance);
        }
        void set_counter(const CounterValueT val) const
        {
            LL_TIM_SetCounter(ControllerTraitsT::skInstance, val);
        }
        void set_prescaler(const uint32_t val) const
        {
            LL_TIM_SetPrescaler(ControllerTraitsT::skInstance, val);
        }
        void set_auto_reload(const uint32_t val) const
        {
            LL_TIM_SetAutoReload(ControllerTraitsT::skInstance, val);
        }
        void set_counter_mode(const TimControllerCounterMode mode) const
        {
            LL_TIM_SetCounterMode(ControllerTraitsT::skInstance, static_cast<uint32_t>(mode));
        }

        void set_one_pulse_mode(const TimControllerOnePulseMode mode) const
        {
            LL_TIM_SetOnePulseMode(ControllerTraitsT::skInstance, static_cast<uint32_t>(mode));
        }

        void set_update_source(const TimControllerUpdateSource source) const
        {
            LL_TIM_SetUpdateSource(ControllerTraitsT::skInstance, static_cast<uint32_t>(source));
        }

        void enable_uiev_event() const
        {
            LL_TIM_EnableUpdateEvent(ControllerTraitsT::skInstance);
        }

        void disable_uiev_event() const
        {
            LL_TIM_DisableUpdateEvent(ControllerTraitsT::skInstance);
        }

        void enable_arr_preload() const
        {
            LL_TIM_EnableARRPreload(ControllerTraitsT::skInstance);
        }

        void disable_arr_preload() const
        {
            LL_TIM_DisableARRPreload(ControllerTraitsT::skInstance);
        }

        void set_repetition_counter(const uint32_t val) const
        {
            LL_TIM_SetRepetitionCounter(ControllerTraitsT::skInstance, val);
        }

        // ---------------------------------------------------------------------
        // HALL SENSOR / XOR CONTROL
        // ---------------------------------------------------------------------

        void enable_xor_combination() const
        {
            LL_TIM_IC_EnableXORCombination(ControllerTraitsT::skInstance);
        }

        void disable_xor_combination() const
        {
            LL_TIM_IC_DisableXORCombination(ControllerTraitsT::skInstance);
        }

        // ---------------------------------------------------------------------
        // ENCODER CONFIGURATION
        // ---------------------------------------------------------------------

#ifdef USE_FULL_LL_DRIVER
        static void config_encoder(const TimControllerEncoderMode       mode,
                                   const TimChannelInputCapturePolarity ti1_pol,
                                   const TimChannelInputCapturePolarity ti2_pol)
        {
            LL_TIM_ENCODER_InitTypeDef config;
            LL_TIM_ENCODER_StructInit(&config);
            config.EncoderMode = static_cast<uint32_t>(mode);
            config.IC1Polarity = static_cast<uint32_t>(ti1_pol);
            config.IC2Polarity = static_cast<uint32_t>(ti2_pol);
            LL_TIM_ENCODER_Init(ControllerTraitsT::skInstance, &config);
        }
#endif

        void set_encoder_mode(const TimControllerEncoderMode mode) const
        {
            LL_TIM_SetEncoderMode(ControllerTraitsT::skInstance, static_cast<uint32_t>(mode));
        }

        // ---------------------------------------------------------------------
        // MASTER / SLAVE CONTROL
        // ---------------------------------------------------------------------

        void set_slave_mode(const TimControllerSlaveMode mode) const
        {
            LL_TIM_SetSlaveMode(ControllerTraitsT::skInstance, static_cast<uint32_t>(mode));
        }

        void set_trigger_source(const TimControllerTriggerSource source) const
        {
            LL_TIM_SetTriggerSource(ControllerTraitsT::skInstance, static_cast<uint32_t>(source));
        }

        void set_trgo_source(const TimControllerTriggerOutputSource source) const
        {
            LL_TIM_SetTriggerOutput(ControllerTraitsT::skInstance, static_cast<uint32_t>(source));
        }

        void set_trgo2_source(const TimControllerTriggerOutput2Source source) const
        {
            LL_TIM_SetTriggerOutput2(ControllerTraitsT::skInstance, static_cast<uint32_t>(source));
        }

        // ---------------------------------------------------------------------
        // EVENT GENERATION
        // ---------------------------------------------------------------------

        void generate_update() const
        {
            LL_TIM_GenerateEvent_UPDATE(ControllerTraitsT::skInstance);
        }

        // ---------------------------------------------------------------------
        // DMA BURST CONTROL
        // ---------------------------------------------------------------------

        void config_dma_burst(const TimControllerDmaBurstBase base, const TimControllerDmaBurstLength len) const
        {
            LL_TIM_ConfigDmaBurst(
                ControllerTraitsT::skInstance, static_cast<uint32_t>(base), static_cast<uint32_t>(len));
        }

        // ---------------------------------------------------------------------
        // BDTR / BREAK CONTROL
        // ---------------------------------------------------------------------

        void enable_main_output() const
        {
            LL_TIM_EnableAllOutputs(ControllerTraitsT::skInstance);
        }

        void disable_main_output() const
        {
            LL_TIM_DisableAllOutputs(ControllerTraitsT::skInstance);
        }

        void set_deadtime(const uint32_t val) const
        {
            LL_TIM_OC_SetDeadTime(ControllerTraitsT::skInstance, val);
        }

        void enable_asymmetric_deadtime() const
        {
            LL_TIM_OC_EnableAsymmetricalDeadTime(ControllerTraitsT::skInstance);
        }

        void disable_asymmetric_deadtime() const
        {
            LL_TIM_OC_DisableAsymmetricalDeadTime(ControllerTraitsT::skInstance);
        }

        void set_falling_deadtime(const uint32_t val) const
        {
            LL_TIM_OC_SetFallingDeadTime(ControllerTraitsT::skInstance, val);
        }

        // ---------------------------------------------------------------------
        // BREAK CONTROL
        // ---------------------------------------------------------------------

        void enable_break() const
        {
            LL_TIM_EnableBRK(ControllerTraitsT::skInstance);
        }

        void disable_break() const
        {
            LL_TIM_DisableBRK(ControllerTraitsT::skInstance);
        }

        void enable_break2() const
        {
            LL_TIM_EnableBRK2(ControllerTraitsT::skInstance);
        }

        void disable_break2() const
        {
            LL_TIM_DisableBRK2(ControllerTraitsT::skInstance);
        }

        void set_break_polarity(const TimControllerBreakPolarity pol) const
        {
            LL_TIM_ConfigBRK(ControllerTraitsT::skInstance, static_cast<uint32_t>(pol));
        }

        void set_break2_polarity(const TimControllerBreakPolarity pol) const
        {
            LL_TIM_ConfigBRK2(ControllerTraitsT::skInstance, static_cast<uint32_t>(pol));
        }

        void set_break_filter(const uint32_t filter) const
        {
            LL_TIM_SetBreakFilter(ControllerTraitsT::skInstance, filter);
        }

        void set_break2_filter(const uint32_t filter) const
        {
            LL_TIM_SetBreak2Filter(ControllerTraitsT::skInstance, filter);
        }

        void enable_break_source(const TimControllerBreakInput input, const TimControllerBreakSource source) const
        {
            LL_TIM_EnableBreakInputSource(
                ControllerTraitsT::skInstance, static_cast<uint32_t>(input), static_cast<uint32_t>(source));
        }

        void disable_break_source(const TimControllerBreakInput input, const TimControllerBreakSource source) const
        {
            LL_TIM_DisableBreakInputSource(
                ControllerTraitsT::skInstance, static_cast<uint32_t>(input), static_cast<uint32_t>(source));
        }

        void disarm_break() const
        {
            LL_TIM_DisarmBRK(ControllerTraitsT::skInstance);
        }

        void rearm_break() const
        {
            LL_TIM_ReArmBRK(ControllerTraitsT::skInstance);
        }

#ifdef USE_FULL_LL_DRIVER
        static void config_bdtr(const TimControllerOffState      ossr,
                                const TimControllerOffState      ossi,
                                const TimControllerLockLevel     lock,
                                const uint32_t                   deadtime,
                                const TimControllerBreakPolarity break_pol,
                                const TimControllerBreakAFMode   break_af)
        {
            LL_TIM_BDTR_InitTypeDef config;
            LL_TIM_BDTR_StructInit(&config);
            config.OSSRState     = (ossr == TimControllerOffState::kRun) ? LL_TIM_OSSR_ENABLE : LL_TIM_OSSR_DISABLE;
            config.OSSIState     = (ossi == TimControllerOffState::kIdle) ? LL_TIM_OSSI_ENABLE : LL_TIM_OSSI_DISABLE;
            config.LockLevel     = static_cast<uint32_t>(lock);
            config.DeadTime      = deadtime;
            config.BreakState    = LL_TIM_BREAK_ENABLE;
            config.BreakPolarity = static_cast<uint32_t>(break_pol);
            config.BreakAFMode   = static_cast<uint32_t>(break_af);
            LL_TIM_BDTR_Init(ControllerTraitsT::skInstance, &config);
        }
#endif
    };

    // =========================================================================
    // CHANNEL INTERFACE
    // =========================================================================

    template <TimControllerId tkControllerId, TimChannelId tkChannel>
    struct TimChannelInterface
    {
        using ControllerTraitsT = TimControllerTraits<tkControllerId>;
        using ChannelTraitsT    = TimChannelTraits<tkControllerId, tkChannel>;

        // ---------------------------------------------------------------------
        // CHANNEL ENABLE
        // ---------------------------------------------------------------------

        void enable() const
        {
            LL_TIM_CC_EnableChannel(ControllerTraitsT::skInstance, ChannelTraitsT::skLLChannelId);
        }
        void disable() const
        {
            LL_TIM_CC_DisableChannel(ControllerTraitsT::skInstance, ChannelTraitsT::skLLChannelId);
        }

        void enable_complementary() const
        {
            LL_TIM_CC_EnableChannel(ControllerTraitsT::skInstance, ChannelTraitsT::skLLComplementaryChannelId);
        }

        void disable_complementary() const
        {
            LL_TIM_CC_DisableChannel(ControllerTraitsT::skInstance, ChannelTraitsT::skLLComplementaryChannelId);
        }

        // ---------------------------------------------------------------------
        // OUTPUT COMPARE / PWM
        // ---------------------------------------------------------------------

        void set_compare(const uint32_t val) const
        {
            switch (tkChannel)
            {
                case TimChannelId::kCh1:
                    LL_TIM_OC_SetCompareCH1(ControllerTraitsT::skInstance, val);
                    break;
                case TimChannelId::kCh2:
                    LL_TIM_OC_SetCompareCH2(ControllerTraitsT::skInstance, val);
                    break;
                case TimChannelId::kCh3:
                    LL_TIM_OC_SetCompareCH3(ControllerTraitsT::skInstance, val);
                    break;
                case TimChannelId::kCh4:
                    LL_TIM_OC_SetCompareCH4(ControllerTraitsT::skInstance, val);
                    break;
                case TimChannelId::kCh5:
                    LL_TIM_OC_SetCompareCH5(ControllerTraitsT::skInstance, val);
                    break;
                case TimChannelId::kCh6:
                    LL_TIM_OC_SetCompareCH6(ControllerTraitsT::skInstance, val);
                    break;
                default:
                    break;
            }
        }

        uint32_t get_compare() const
        {
            return get_capture();  // Alias
        }

        void set_mode(const TimChannelOutputCaptureMode mode) const
        {
            LL_TIM_OC_SetMode(
                ControllerTraitsT::skInstance, ChannelTraitsT::skLLChannelId, static_cast<uint32_t>(mode));
        }

        void set_polarity(const TimChannelOutputCapturePolarity pol) const
        {
            LL_TIM_OC_SetPolarity(
                ControllerTraitsT::skInstance, ChannelTraitsT::skLLChannelId, static_cast<uint32_t>(pol));
        }

        void set_idle_state(const TimChannelOutputCaptureIdleState state) const
        {
            LL_TIM_OC_SetIdleState(
                ControllerTraitsT::skInstance, ChannelTraitsT::skLLChannelId, static_cast<uint32_t>(state));
        }

        void enable_output_capture_preload() const
        {
            LL_TIM_OC_EnablePreload(ControllerTraitsT::skInstance, ChannelTraitsT::skLLChannelId);
        }

        void disable_output_capture_preload() const
        {
            LL_TIM_OC_DisablePreload(ControllerTraitsT::skInstance, ChannelTraitsT::skLLChannelId);
        }

        void set_output_capture_preload(const bool enable) const
        {
            if (enable)
            {
                enable_output_capture_preload();
            }
            else
            {
                disable_output_capture_preload();
            }
        }

        void enable_output_capture_fast_mode() const
        {
            LL_TIM_OC_EnableFast(ControllerTraitsT::skInstance, ChannelTraitsT::skLLChannelId);
        }

        void disable_output_capture_fast_mode() const
        {
            LL_TIM_OC_DisableFast(ControllerTraitsT::skInstance, ChannelTraitsT::skLLChannelId);
        }

        void set_output_capture_fast_mode(const bool enable) const
        {
            if (enable)
            {
                enable_output_capture_fast_mode();
            }
            else
            {
                disable_output_capture_fast_mode();
            }
        }

        void enable_output_capture_clear() const
        {
            LL_TIM_OC_EnableClear(ControllerTraitsT::skInstance, ChannelTraitsT::skLLChannelId);
        }

        void disable_output_capture_clear() const
        {
            LL_TIM_OC_DisableClear(ControllerTraitsT::skInstance, ChannelTraitsT::skLLChannelId);
        }

        void set_output_capture_clear(const bool enable) const
        {
            if (enable)
            {
                enable_output_capture_clear();
            }
            else
            {
                disable_output_capture_clear();
            }
        }

        void set_complementary_output_capture_polarity(const TimChannelOutputCapturePolarity pol) const
        {
            LL_TIM_OC_SetPolarity(
                ControllerTraitsT::skInstance, ChannelTraitsT::skLLComplementaryChannelId, static_cast<uint32_t>(pol));
        }

        void set_complementary_output_capture_idle_state(const TimChannelOutputCaptureIdleState state) const
        {
            LL_TIM_OC_SetIdleState(ControllerTraitsT::skInstance,
                                   ChannelTraitsT::skLLComplementaryChannelId,
                                   static_cast<uint32_t>(state));
        }

#ifdef USE_FULL_LL_DRIVER
        static void config_output_capture(const TimChannelOutputCaptureMode      mode,
                                          const uint32_t                         compare,
                                          const TimChannelOutputCapturePolarity  pol,
                                          const TimChannelOutputCaptureIdleState idle)
        {
            LL_TIM_OC_InitTypeDef config;
            LL_TIM_OC_StructInit(&config);
            config.OCMode       = static_cast<uint32_t>(mode);
            config.OCState      = LL_TIM_OCSTATE_ENABLE;
            config.OCNState     = LL_TIM_OCSTATE_ENABLE;
            config.CompareValue = compare;
            config.OCPolarity   = static_cast<uint32_t>(pol);
            config.OCNPolarity  = static_cast<uint32_t>(pol);
            config.OCIdleState  = static_cast<uint32_t>(idle);
            config.OCNIdleState = static_cast<uint32_t>(idle);
            LL_TIM_OC_Init(ControllerTraitsT::skInstance, ChannelTraitsT::skLLChannelId, &config);
        }
#endif

        // ---------------------------------------------------------------------
        // INPUT CAPTURE
        // ---------------------------------------------------------------------

        void set_input_capture_active_input(const TimChannelInputCaptureActiveInput input) const
        {
            LL_TIM_IC_SetActiveInput(
                ControllerTraitsT::skInstance, ChannelTraitsT::skLLChannelId, static_cast<uint32_t>(input));
        }

        void set_input_capture_prescaler(const TimChannelInputCapturePrescaler psc) const
        {
            LL_TIM_IC_SetPrescaler(
                ControllerTraitsT::skInstance, ChannelTraitsT::skLLChannelId, static_cast<uint32_t>(psc));
        }

        void set_input_capture_filter(const TimChannelInputCaptureFilter filter) const
        {
            LL_TIM_IC_SetFilter(
                ControllerTraitsT::skInstance, ChannelTraitsT::skLLChannelId, static_cast<uint32_t>(filter));
        }

        void set_input_capture_polarity(const TimChannelInputCapturePolarity polarity) const
        {
            LL_TIM_IC_SetPolarity(
                ControllerTraitsT::skInstance, ChannelTraitsT::skLLChannelId, static_cast<uint32_t>(polarity));
        }

#ifdef USE_FULL_LL_DRIVER
        static void config_input_capture(const TimChannelInputCaptureActiveInput input,
                                         const TimChannelInputCapturePrescaler   psc,
                                         const TimChannelInputCaptureFilter      filter,
                                         const TimChannelInputCapturePolarity    polarity)
        {
            LL_TIM_IC_InitTypeDef config;
            LL_TIM_IC_StructInit(&config);
            config.ICActiveInput = static_cast<uint32_t>(input);
            config.ICPrescaler   = static_cast<uint32_t>(psc);
            config.ICFilter      = static_cast<uint32_t>(filter);
            config.ICPolarity    = static_cast<uint32_t>(polarity);
            LL_TIM_IC_Init(ControllerTraitsT::skInstance, ChannelTraitsT::skLLChannelId, &config);
        }
#endif

        uint32_t get_capture() const
        {
            switch (tkChannel)
            {
                case TimChannelId::kCh1:
                    return LL_TIM_OC_GetCompareCH1(ControllerTraitsT::skInstance);
                case TimChannelId::kCh2:
                    return LL_TIM_OC_GetCompareCH2(ControllerTraitsT::skInstance);
                case TimChannelId::kCh3:
                    return LL_TIM_OC_GetCompareCH3(ControllerTraitsT::skInstance);
                case TimChannelId::kCh4:
                    return LL_TIM_OC_GetCompareCH4(ControllerTraitsT::skInstance);
                case TimChannelId::kCh5:
                    return LL_TIM_OC_GetCompareCH5(ControllerTraitsT::skInstance);
                case TimChannelId::kCh6:
                    return LL_TIM_OC_GetCompareCH6(ControllerTraitsT::skInstance);
                default:
                    return 0;
            }
        }
    };
}  // namespace valle::platform