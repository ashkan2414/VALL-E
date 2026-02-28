#pragma once

#include "stm32g4xx_ll_bus.h"
#include "stm32g4xx_ll_hrtim.h"
#include "valle/core.hpp"
#include "valle/platform/hardware/hrtim/id.hpp"
#include "valle/utils/template_utils.hpp"

namespace valle
{
    // ============================================================================
    // ENUMERATIONS
    // ============================================================================
    enum class HRTIMFaultSource : uint32_t
    {
        kDigitalInput = LL_HRTIM_FLT_SRC_DIGITALINPUT,
        kInternal     = LL_HRTIM_FLT_SRC_INTERNAL,
        kEEVInput     = LL_HRTIM_FLT_SRC_EEVINPUT,
    };

    enum class HRTIMFaultPolarity : uint32_t
    {
        kLow  = LL_HRTIM_FLT_POLARITY_LOW,
        kHigh = LL_HRTIM_FLT_POLARITY_HIGH,
    };

    enum class HRTIMFaultFilter : uint32_t
    {
        kNone     = LL_HRTIM_FLT_FILTER_NONE,
        kFilter1  = LL_HRTIM_FLT_FILTER_1,
        kFilter2  = LL_HRTIM_FLT_FILTER_2,
        kFilter3  = LL_HRTIM_FLT_FILTER_3,
        kFilter4  = LL_HRTIM_FLT_FILTER_4,
        kFilter5  = LL_HRTIM_FLT_FILTER_5,
        kFilter6  = LL_HRTIM_FLT_FILTER_6,
        kFilter7  = LL_HRTIM_FLT_FILTER_7,
        kFilter8  = LL_HRTIM_FLT_FILTER_8,
        kFilter9  = LL_HRTIM_FLT_FILTER_9,
        kFilter10 = LL_HRTIM_FLT_FILTER_10,
        kFilter11 = LL_HRTIM_FLT_FILTER_11,
        kFilter12 = LL_HRTIM_FLT_FILTER_12,
        kFilter13 = LL_HRTIM_FLT_FILTER_13,
        kFilter14 = LL_HRTIM_FLT_FILTER_14,
        kFilter15 = LL_HRTIM_FLT_FILTER_15
    };

    enum class HRTIMFaultPrescaler : uint32_t
    {
        kDiv1 = LL_HRTIM_FLT_PRESCALER_DIV1,
        kDiv2 = LL_HRTIM_FLT_PRESCALER_DIV2,
        kDiv4 = LL_HRTIM_FLT_PRESCALER_DIV4,
        kDiv8 = LL_HRTIM_FLT_PRESCALER_DIV8,
    };

    enum class HRTIMFaultBlankingSource : uint32_t
    {
        kRSTAligned = LL_HRTIM_FLT_BLANKING_RSTALIGNED,
        kMoving     = LL_HRTIM_FLT_BLANKING_MOVING,
    };

    enum class HRTIMFaultResetMode : uint32_t
    {
        kUnconditional = LL_HRTIM_FLT_COUNTERRST_UNCONDITIONAL,
        kConditional   = LL_HRTIM_FLT_COUNTERRST_CONDITIONAL
    };

    enum class HRTIMEEVSource : uint32_t
    {
        kSource1 = 0x00000000U,
        kSource2 = HRTIM_EECR1_EE1SRC_0,
        kSource3 = HRTIM_EECR1_EE1SRC_1,
        kSource4 = HRTIM_EECR1_EE1SRC_1 | HRTIM_EECR1_EE1SRC_0,
    };

    enum class HRTIMEEVPolarity : uint32_t
    {
        kHigh = LL_HRTIM_EE_POLARITY_HIGH,
        kLow  = LL_HRTIM_EE_POLARITY_LOW,
    };

    enum class HRTIMEEVSensitivity : uint32_t
    {
        kLevel       = LL_HRTIM_EE_SENSITIVITY_LEVEL,
        kRisingEdge  = LL_HRTIM_EE_SENSITIVITY_RISINGEDGE,
        kFallingEdge = LL_HRTIM_EE_SENSITIVITY_FALLINGEDGE,
        kBothEdges   = LL_HRTIM_EE_SENSITIVITY_BOTHEDGES
    };

    enum class HRTIMEEVFastMode : uint32_t
    {
        kDisable = LL_HRTIM_EE_FASTMODE_DISABLE,
        kEnable  = LL_HRTIM_EE_FASTMODE_ENABLE,
    };

    enum class HRTIMEEVFilter : uint32_t
    {
        kNone     = LL_HRTIM_EE_FILTER_NONE,
        kFilter1  = LL_HRTIM_EE_FILTER_1,
        kFilter2  = LL_HRTIM_EE_FILTER_2,
        kFilter3  = LL_HRTIM_EE_FILTER_3,
        kFilter4  = LL_HRTIM_EE_FILTER_4,
        kFilter5  = LL_HRTIM_EE_FILTER_5,
        kFilter6  = LL_HRTIM_EE_FILTER_6,
        kFilter7  = LL_HRTIM_EE_FILTER_7,
        kFilter8  = LL_HRTIM_EE_FILTER_8,
        kFilter9  = LL_HRTIM_EE_FILTER_9,
        kFilter10 = LL_HRTIM_EE_FILTER_10,
        kFilter11 = LL_HRTIM_EE_FILTER_11,
        kFilter12 = LL_HRTIM_EE_FILTER_12,
        kFilter13 = LL_HRTIM_EE_FILTER_13,
        kFilter14 = LL_HRTIM_EE_FILTER_14,
        kFilter15 = LL_HRTIM_EE_FILTER_15
    };

    enum class HRTIMEEVPrescaler : uint32_t
    {
        kDiv1 = LL_HRTIM_EE_PRESCALER_DIV1,
        kDiv2 = LL_HRTIM_EE_PRESCALER_DIV2,
        kDiv4 = LL_HRTIM_EE_PRESCALER_DIV4,
        kDiv8 = LL_HRTIM_EE_PRESCALER_DIV8,
    };

    enum class HRTIMTimerPrescaler : uint32_t
    {
        kMul32 = LL_HRTIM_PRESCALERRATIO_MUL32,
        kMul16 = LL_HRTIM_PRESCALERRATIO_MUL16,
        kMul8  = LL_HRTIM_PRESCALERRATIO_MUL8,
        kMul4  = LL_HRTIM_PRESCALERRATIO_MUL4,
        kMul2  = LL_HRTIM_PRESCALERRATIO_MUL2,
        kDiv1  = LL_HRTIM_PRESCALERRATIO_DIV1,
        kDiv2  = LL_HRTIM_PRESCALERRATIO_DIV2,
        kDiv4  = LL_HRTIM_PRESCALERRATIO_DIV4,
    };

    enum class HRTIMTimerDeadtimePrescaler : uint32_t
    {
        kMul8  = LL_HRTIM_DT_PRESCALER_MUL8,
        kMul4  = LL_HRTIM_DT_PRESCALER_MUL4,
        kMul2  = LL_HRTIM_DT_PRESCALER_MUL2,
        kDiv1  = LL_HRTIM_DT_PRESCALER_DIV1,
        kDiv2  = LL_HRTIM_DT_PRESCALER_DIV2,
        kDiv4  = LL_HRTIM_DT_PRESCALER_DIV4,
        kDiv8  = LL_HRTIM_DT_PRESCALER_DIV8,
        kDiv16 = LL_HRTIM_DT_PRESCALER_DIV16,
    };

    enum class HRTIMTimerCounterMode : uint32_t
    {
        kContinuous = LL_HRTIM_MODE_CONTINUOUS,
        kSingleShot = LL_HRTIM_MODE_SINGLESHOT,
        kBurst      = LL_HRTIM_MODE_RETRIGGERABLE,
    };

    enum class HRTIMTimerCountingMode : uint32_t
    {
        kUp     = LL_HRTIM_COUNTING_MODE_UP,
        kUpDown = LL_HRTIM_COUNTING_MODE_UP_DOWN,
    };

    enum class HRTIMTimerRolloverMode : uint32_t
    {
        kPeriodReset = LL_HRTIM_ROLLOVER_MODE_BOTH,
        kReset       = LL_HRTIM_ROLLOVER_MODE_RST,
        kPeriod      = LL_HRTIM_ROLLOVER_MODE_PER,
    };

    enum class HRTIMTimerCompareUnit : uint8_t
    {
        kCompare1 = 1,
        kCompare2 = 2,
        kCompare3 = 3,
        kCompare4 = 4
    };

    enum class HRTIMTimerOutputSetSource : uint32_t
    {
        kNone                       = LL_HRTIM_OUTPUTSET_NONE,
        kResync                     = LL_HRTIM_OUTPUTSET_RESYNC,
        kTimerPeriod                = LL_HRTIM_OUTPUTSET_TIMPER,
        kTimerCompare1              = LL_HRTIM_OUTPUTSET_TIMCMP1,
        kTimerCompare2              = LL_HRTIM_OUTPUTSET_TIMCMP2,
        kTimerCompare3              = LL_HRTIM_OUTPUTSET_TIMCMP3,
        kTimerCompare4              = LL_HRTIM_OUTPUTSET_TIMCMP4,
        kMasterPeriod               = LL_HRTIM_OUTPUTSET_MASTERPER,
        kMasterCompare1             = LL_HRTIM_OUTPUTSET_MASTERCMP1,
        kMasterCompare2             = LL_HRTIM_OUTPUTSET_MASTERCMP2,
        kMasterCompare3             = LL_HRTIM_OUTPUTSET_MASTERCMP3,
        kMasterCompare4             = LL_HRTIM_OUTPUTSET_MASTERCMP4,
        kTimerAEvent1TimerBCompare1 = LL_HRTIM_OUTPUTSET_TIMAEV1_TIMBCMP1,
        kTimerAEvent2TimerBCompare2 = LL_HRTIM_OUTPUTSET_TIMAEV2_TIMBCMP2,
        kTimerAEvent3TimerCCompare2 = LL_HRTIM_OUTPUTSET_TIMAEV3_TIMCCMP2,
        kTimerAEvent4TimerCCompare3 = LL_HRTIM_OUTPUTSET_TIMAEV4_TIMCCMP3,
        kTimerAEvent5TimerDCompare1 = LL_HRTIM_OUTPUTSET_TIMAEV5_TIMDCMP1,
        kTimerAEvent6TimerDCompare2 = LL_HRTIM_OUTPUTSET_TIMAEV6_TIMDCMP2,
        kTimerAEvent7TimerECompare3 = LL_HRTIM_OUTPUTSET_TIMAEV7_TIMECMP3,
        kTimerAEvent8TimerECompare4 = LL_HRTIM_OUTPUTSET_TIMAEV8_TIMECMP4,
        kTimerAEvent9TimerFCompare4 = LL_HRTIM_OUTPUTSET_TIMAEV9_TIMFCMP4,
        kTimerBEvent1TimerACompare1 = LL_HRTIM_OUTPUTSET_TIMBEV1_TIMACMP1,
        kTimerBEvent2TimerACompare2 = LL_HRTIM_OUTPUTSET_TIMBEV2_TIMACMP2,
        kTimerBEvent3TimerCCompare3 = LL_HRTIM_OUTPUTSET_TIMBEV3_TIMCCMP3,
        kTimerBEvent4TimerCCompare4 = LL_HRTIM_OUTPUTSET_TIMBEV4_TIMCCMP4,
        kTimerBEvent5TimerDCompare3 = LL_HRTIM_OUTPUTSET_TIMBEV5_TIMDCMP3,
        kTimerBEvent6TimerDCompare4 = LL_HRTIM_OUTPUTSET_TIMBEV6_TIMDCMP4,
        kTimerBEvent7TimerECompare1 = LL_HRTIM_OUTPUTSET_TIMBEV7_TIMECMP1,
        kTimerBEvent8TimerECompare2 = LL_HRTIM_OUTPUTSET_TIMBEV8_TIMECMP2,
        kTimerBEvent9TimerFCompare3 = LL_HRTIM_OUTPUTSET_TIMBEV9_TIMFCMP3,
        kTimerCEvent1TimerACompare2 = LL_HRTIM_OUTPUTSET_TIMCEV1_TIMACMP2,
        kTimerCEvent2TimerACompare3 = LL_HRTIM_OUTPUTSET_TIMCEV2_TIMACMP3,
        kTimerCEvent3TimerBCompare2 = LL_HRTIM_OUTPUTSET_TIMCEV3_TIMBCMP2,
        kTimerCEvent4TimerBCompare3 = LL_HRTIM_OUTPUTSET_TIMCEV4_TIMBCMP3,
        kTimerCEvent5TimerDCompare2 = LL_HRTIM_OUTPUTSET_TIMCEV5_TIMDCMP2,
        kTimerCEvent6TimerDCompare4 = LL_HRTIM_OUTPUTSET_TIMCEV6_TIMDCMP4,
        kTimerCEvent7TimerECompare3 = LL_HRTIM_OUTPUTSET_TIMCEV7_TIMECMP3,
        kTimerCEvent8TimerECompare4 = LL_HRTIM_OUTPUTSET_TIMCEV8_TIMECMP4,
        kTimerCEvent9TimerFCompare2 = LL_HRTIM_OUTPUTSET_TIMCEV9_TIMFCMP2,
        kTimerDEvent1TimerACompare1 = LL_HRTIM_OUTPUTSET_TIMDEV1_TIMACMP1,
        kTimerDEvent2TimerACompare4 = LL_HRTIM_OUTPUTSET_TIMDEV2_TIMACMP4,
        kTimerDEvent3TimerBCompare2 = LL_HRTIM_OUTPUTSET_TIMDEV3_TIMBCMP2,
        kTimerDEvent4TimerBCompare4 = LL_HRTIM_OUTPUTSET_TIMDEV4_TIMBCMP4,
        kTimerDEvent5TimerCCompare4 = LL_HRTIM_OUTPUTSET_TIMDEV5_TIMCCMP4,
        kTimerDEvent6TimerECompare1 = LL_HRTIM_OUTPUTSET_TIMDEV6_TIMECMP1,
        kTimerDEvent7TimerECompare4 = LL_HRTIM_OUTPUTSET_TIMDEV7_TIMECMP4,
        kTimerDEvent8TimerFCompare1 = LL_HRTIM_OUTPUTSET_TIMDEV8_TIMFCMP1,
        kTimerDEvent9TimerFCompare3 = LL_HRTIM_OUTPUTSET_TIMDEV9_TIMFCMP3,
        kTimerEEvent1TimerACompare4 = LL_HRTIM_OUTPUTSET_TIMEEV1_TIMACMP4,
        kTimerEEvent2TimerBCompare3 = LL_HRTIM_OUTPUTSET_TIMEEV2_TIMBCMP3,
        kTimerEEvent3TimerBCompare4 = LL_HRTIM_OUTPUTSET_TIMEEV3_TIMBCMP4,
        kTimerEEvent4TimerCCompare1 = LL_HRTIM_OUTPUTSET_TIMEEV4_TIMCCMP1,
        kTimerEEvent5TimerCCompare2 = LL_HRTIM_OUTPUTSET_TIMEEV5_TIMCCMP2,
        kTimerEEvent6TimerDCompare1 = LL_HRTIM_OUTPUTSET_TIMEEV6_TIMDCMP1,
        kTimerEEvent7TimerDCompare2 = LL_HRTIM_OUTPUTSET_TIMEEV7_TIMDCMP2,
        kTimerEEvent8TimerFCompare3 = LL_HRTIM_OUTPUTSET_TIMEEV8_TIMFCMP3,
        kTimerEEvent9TimerFCompare4 = LL_HRTIM_OUTPUTSET_TIMEEV9_TIMFCMP4,
        kTimerFEvent1TimerACompare3 = LL_HRTIM_OUTPUTSET_TIMFEV1_TIMACMP3,
        kTimerFEvent2TimerBCompare1 = LL_HRTIM_OUTPUTSET_TIMFEV2_TIMBCMP1,
        kTimerFEvent3TimerBCompare4 = LL_HRTIM_OUTPUTSET_TIMFEV3_TIMBCMP4,
        kTimerFEvent4TimerCCompare1 = LL_HRTIM_OUTPUTSET_TIMFEV4_TIMCCMP1,
        kTimerFEvent5TimerCCompare4 = LL_HRTIM_OUTPUTSET_TIMFEV5_TIMCCMP4,
        kTimerFEvent6TimerDCompare3 = LL_HRTIM_OUTPUTSET_TIMFEV6_TIMDCMP3,
        kTimerFEvent7TimerDCompare4 = LL_HRTIM_OUTPUTSET_TIMFEV7_TIMDCMP4,
        kTimerFEvent8TimerECompare2 = LL_HRTIM_OUTPUTSET_TIMFEV8_TIMECMP2,
        kTimerFEvent9TimerECompare3 = LL_HRTIM_OUTPUTSET_TIMFEV9_TIMECMP3,
        kExternalEvent1             = LL_HRTIM_OUTPUTSET_EEV_1,
        kExternalEvent2             = LL_HRTIM_OUTPUTSET_EEV_2,
        kExternalEvent3             = LL_HRTIM_OUTPUTSET_EEV_3,
        kExternalEvent4             = LL_HRTIM_OUTPUTSET_EEV_4,
        kExternalEvent5             = LL_HRTIM_OUTPUTSET_EEV_5,
        kExternalEvent6             = LL_HRTIM_OUTPUTSET_EEV_6,
        kExternalEvent7             = LL_HRTIM_OUTPUTSET_EEV_7,
        kExternalEvent8             = LL_HRTIM_OUTPUTSET_EEV_8,
        kExternalEvent9             = LL_HRTIM_OUTPUTSET_EEV_9,
        kExternalEvent10            = LL_HRTIM_OUTPUTSET_EEV_10,
        kUpdate                     = LL_HRTIM_OUTPUTSET_UPDATE,
    };

    enum class HRTIMTimerOutputResetSource : uint32_t
    {
        kNone                       = LL_HRTIM_OUTPUTRESET_NONE,
        kResync                     = LL_HRTIM_OUTPUTRESET_RESYNC,
        kTimerPeriod                = LL_HRTIM_OUTPUTRESET_TIMPER,
        kTimerCompare1              = LL_HRTIM_OUTPUTRESET_TIMCMP1,
        kTimerCompare2              = LL_HRTIM_OUTPUTRESET_TIMCMP2,
        kTimerCompare3              = LL_HRTIM_OUTPUTRESET_TIMCMP3,
        kTimerCompare4              = LL_HRTIM_OUTPUTRESET_TIMCMP4,
        kMasterPeriod               = LL_HRTIM_OUTPUTRESET_MASTERPER,
        kMasterCompare1             = LL_HRTIM_OUTPUTRESET_MASTERCMP1,
        kMasterCompare2             = LL_HRTIM_OUTPUTRESET_MASTERCMP2,
        kMasterCompare3             = LL_HRTIM_OUTPUTRESET_MASTERCMP3,
        kMasterCompare4             = LL_HRTIM_OUTPUTRESET_MASTERCMP4,
        kTimerAEvent1TimerBCompare1 = LL_HRTIM_OUTPUTRESET_TIMAEV1_TIMBCMP1,
        kTimerAEvent2TimerBCompare2 = LL_HRTIM_OUTPUTRESET_TIMAEV2_TIMBCMP2,
        kTimerAEvent3TimerCCompare2 = LL_HRTIM_OUTPUTRESET_TIMAEV3_TIMCCMP2,
        kTimerAEvent4TimerCCompare3 = LL_HRTIM_OUTPUTRESET_TIMAEV4_TIMCCMP3,
        kTimerAEvent5TimerDCompare1 = LL_HRTIM_OUTPUTRESET_TIMAEV5_TIMDCMP1,
        kTimerAEvent6TimerDCompare2 = LL_HRTIM_OUTPUTRESET_TIMAEV6_TIMDCMP2,
        kTimerAEvent7TimerECompare3 = LL_HRTIM_OUTPUTRESET_TIMAEV7_TIMECMP3,
        kTimerAEvent8TimerECompare4 = LL_HRTIM_OUTPUTRESET_TIMAEV8_TIMECMP4,
        kTimerAEvent9TimerFCompare4 = LL_HRTIM_OUTPUTRESET_TIMAEV9_TIMFCMP4,
        kTimerBEvent1TimerACompare1 = LL_HRTIM_OUTPUTRESET_TIMBEV1_TIMACMP1,
        kTimerBEvent2TimerACompare2 = LL_HRTIM_OUTPUTRESET_TIMBEV2_TIMACMP2,
        kTimerBEvent3TimerCCompare3 = LL_HRTIM_OUTPUTRESET_TIMBEV3_TIMCCMP3,
        kTimerBEvent4TimerCCompare4 = LL_HRTIM_OUTPUTRESET_TIMBEV4_TIMCCMP4,
        kTimerBEvent5TimerDCompare3 = LL_HRTIM_OUTPUTRESET_TIMBEV5_TIMDCMP3,
        kTimerBEvent6TimerDCompare4 = LL_HRTIM_OUTPUTRESET_TIMBEV6_TIMDCMP4,
        kTimerBEvent7TimerECompare1 = LL_HRTIM_OUTPUTRESET_TIMBEV7_TIMECMP1,
        kTimerBEvent8TimerECompare2 = LL_HRTIM_OUTPUTRESET_TIMBEV8_TIMECMP2,
        kTimerBEvent9TimerFCompare3 = LL_HRTIM_OUTPUTRESET_TIMBEV9_TIMFCMP3,
        kTimerCEvent1TimerACompare2 = LL_HRTIM_OUTPUTRESET_TIMCEV1_TIMACMP2,
        kTimerCEvent2TimerACompare3 = LL_HRTIM_OUTPUTRESET_TIMCEV2_TIMACMP3,
        kTimerCEvent3TimerBCompare2 = LL_HRTIM_OUTPUTRESET_TIMCEV3_TIMBCMP2,
        kTimerCEvent4TimerBCompare3 = LL_HRTIM_OUTPUTRESET_TIMCEV4_TIMBCMP3,
        kTimerCEvent5TimerDCompare2 = LL_HRTIM_OUTPUTRESET_TIMCEV5_TIMDCMP2,
        kTimerCEvent6TimerDCompare4 = LL_HRTIM_OUTPUTRESET_TIMCEV6_TIMDCMP4,
        kTimerCEvent7TimerECompare3 = LL_HRTIM_OUTPUTRESET_TIMCEV7_TIMECMP3,
        kTimerCEvent8TimerECompare4 = LL_HRTIM_OUTPUTRESET_TIMCEV8_TIMECMP4,
        kTimerCEvent9TimerFCompare2 = LL_HRTIM_OUTPUTRESET_TIMCEV9_TIMFCMP2,
        kTimerDEvent1TimerACompare1 = LL_HRTIM_OUTPUTRESET_TIMDEV1_TIMACMP1,
        kTimerDEvent2TimerACompare4 = LL_HRTIM_OUTPUTRESET_TIMDEV2_TIMACMP4,
        kTimerDEvent3TimerBCompare2 = LL_HRTIM_OUTPUTRESET_TIMDEV3_TIMBCMP2,
        kTimerDEvent4TimerBCompare4 = LL_HRTIM_OUTPUTRESET_TIMDEV4_TIMBCMP4,
        kTimerDEvent5TimerCCompare4 = LL_HRTIM_OUTPUTRESET_TIMDEV5_TIMCCMP4,
        kTimerDEvent6TimerECompare1 = LL_HRTIM_OUTPUTRESET_TIMDEV6_TIMECMP1,
        kTimerDEvent7TimerECompare4 = LL_HRTIM_OUTPUTRESET_TIMDEV7_TIMECMP4,
        kTimerDEvent8TimerFCompare1 = LL_HRTIM_OUTPUTRESET_TIMDEV8_TIMFCMP1,
        kTimerDEvent9TimerFCompare3 = LL_HRTIM_OUTPUTRESET_TIMDEV9_TIMFCMP3,
        kTimerEEvent1TimerACompare4 = LL_HRTIM_OUTPUTRESET_TIMEEV1_TIMACMP4,
        kTimerEEvent2TimerBCompare3 = LL_HRTIM_OUTPUTRESET_TIMEEV2_TIMBCMP3,
        kTimerEEvent3TimerBCompare4 = LL_HRTIM_OUTPUTRESET_TIMEEV3_TIMBCMP4,
        kTimerEEvent4TimerCCompare1 = LL_HRTIM_OUTPUTRESET_TIMEEV4_TIMCCMP1,
        kTimerEEvent5TimerCCompare2 = LL_HRTIM_OUTPUTRESET_TIMEEV5_TIMCCMP2,
        kTimerEEvent6TimerDCompare1 = LL_HRTIM_OUTPUTRESET_TIMEEV6_TIMDCMP1,
        kTimerEEvent7TimerDCompare2 = LL_HRTIM_OUTPUTRESET_TIMEEV7_TIMDCMP2,
        kTimerEEvent8TimerFCompare3 = LL_HRTIM_OUTPUTRESET_TIMEEV8_TIMFCMP3,
        kTimerEEvent9TimerFCompare4 = LL_HRTIM_OUTPUTRESET_TIMEEV9_TIMFCMP4,
        kTimerFEvent1TimerACompare3 = LL_HRTIM_OUTPUTRESET_TIMFEV1_TIMACMP3,
        kTimerFEvent2TimerBCompare1 = LL_HRTIM_OUTPUTRESET_TIMFEV2_TIMBCMP1,
        kTimerFEvent3TimerBCompare4 = LL_HRTIM_OUTPUTRESET_TIMFEV3_TIMBCMP4,
        kTimerFEvent4TimerCCompare1 = LL_HRTIM_OUTPUTRESET_TIMFEV4_TIMCCMP1,
        kTimerFEvent5TimerCCompare4 = LL_HRTIM_OUTPUTRESET_TIMFEV5_TIMCCMP4,
        kTimerFEvent6TimerDCompare3 = LL_HRTIM_OUTPUTRESET_TIMFEV6_TIMDCMP3,
        kTimerFEvent7TimerDCompare4 = LL_HRTIM_OUTPUTRESET_TIMFEV7_TIMDCMP4,
        kTimerFEvent8TimerECompare2 = LL_HRTIM_OUTPUTRESET_TIMFEV8_TIMECMP2,
        kTimerFEvent9TimerECompare3 = LL_HRTIM_OUTPUTRESET_TIMFEV9_TIMECMP3,
        kExternalEvent1             = LL_HRTIM_OUTPUTRESET_EEV_1,
        kExternalEvent2             = LL_HRTIM_OUTPUTRESET_EEV_2,
        kExternalEvent3             = LL_HRTIM_OUTPUTRESET_EEV_3,
        kExternalEvent4             = LL_HRTIM_OUTPUTRESET_EEV_4,
        kExternalEvent5             = LL_HRTIM_OUTPUTRESET_EEV_5,
        kExternalEvent6             = LL_HRTIM_OUTPUTRESET_EEV_6,
        kExternalEvent7             = LL_HRTIM_OUTPUTRESET_EEV_7,
        kExternalEvent8             = LL_HRTIM_OUTPUTRESET_EEV_8,
        kExternalEvent9             = LL_HRTIM_OUTPUTRESET_EEV_9,
        kExternalEvent10            = LL_HRTIM_OUTPUTRESET_EEV_10,
        kUpdate                     = LL_HRTIM_OUTPUTRESET_UPDATE,
    };

    enum class HRTIMTimerOutputPolarity : uint32_t
    {
        kPositive = LL_HRTIM_OUT_POSITIVE_POLARITY,  /// Output active high
        kNegative = LL_HRTIM_OUT_NEGATIVE_POLARITY   /// Output active low
    };

    enum class HRTIMTimerOutputIdleMode : uint32_t
    {
        kNoIdle = LL_HRTIM_OUT_NO_IDLE,          /// Never idle
        kBurst  = LL_HRTIM_OUT_IDLE_WHEN_BURST,  /// Idle during burst mode
    };

    enum class HRTIMTimerOutputIdleLevel : uint32_t
    {
        kInactive = LL_HRTIM_OUT_IDLELEVEL_INACTIVE,  /// Inactive when idle
        kActive   = LL_HRTIM_OUT_IDLELEVEL_ACTIVE     /// Active when idle
    };

    enum class HRTIMTimerOutputFaultState : uint32_t
    {
        kInactive = LL_HRTIM_OUT_FAULTSTATE_INACTIVE,   /// Output inactive on fault
        kActive   = LL_HRTIM_OUT_FAULTSTATE_ACTIVE,     /// Output active on fault
        kNoAction = LL_HRTIM_OUT_FAULTSTATE_NO_ACTION,  /// No action on fault
        kHighZ    = LL_HRTIM_OUT_FAULTSTATE_HIGHZ       /// High impedance on fault
    };

    enum class HRTIMTimerOutputChopperMode : uint32_t
    {
        kDisabled = LL_HRTIM_OUT_CHOPPERMODE_DISABLED,
        kEnabled  = LL_HRTIM_OUT_CHOPPERMODE_ENABLED,
    };

    enum class HRTIMTimerOutputBurstModeEntryMode : uint32_t
    {
        kRegular = LL_HRTIM_OUT_BM_ENTRYMODE_REGULAR,
        kDelayed = LL_HRTIM_OUT_BM_ENTRYMODE_DELAYED,
    };

    enum class HRTIMTimerMasterADCTriggerSourceID13 : uint32_t
    {
        kNone     = LL_HRTIM_ADCTRIG_SRC13_NONE,
        kEEV1     = LL_HRTIM_ADCTRIG_SRC13_EEV1,
        kEEV2     = LL_HRTIM_ADCTRIG_SRC13_EEV2,
        kEEV3     = LL_HRTIM_ADCTRIG_SRC13_EEV3,
        kEEV4     = LL_HRTIM_ADCTRIG_SRC13_EEV4,
        kEEV5     = LL_HRTIM_ADCTRIG_SRC13_EEV5,
        kCompare1 = LL_HRTIM_ADCTRIG_SRC13_MCMP1,
        kCompare2 = LL_HRTIM_ADCTRIG_SRC13_MCMP2,
        kCompare3 = LL_HRTIM_ADCTRIG_SRC13_MCMP3,
        kCompare4 = LL_HRTIM_ADCTRIG_SRC13_MCMP4,
        kPeriod   = LL_HRTIM_ADCTRIG_SRC13_MPER,
    };

    enum class HRTIMTimerAADCTriggerSourceID13 : uint32_t
    {
        kNone     = LL_HRTIM_ADCTRIG_SRC13_NONE,
        kEEV1     = LL_HRTIM_ADCTRIG_SRC13_EEV1,
        kEEV2     = LL_HRTIM_ADCTRIG_SRC13_EEV2,
        kEEV3     = LL_HRTIM_ADCTRIG_SRC13_EEV3,
        kEEV4     = LL_HRTIM_ADCTRIG_SRC13_EEV4,
        kEEV5     = LL_HRTIM_ADCTRIG_SRC13_EEV5,
        kCompare3 = LL_HRTIM_ADCTRIG_SRC13_TIMACMP3,
        kCompare4 = LL_HRTIM_ADCTRIG_SRC13_TIMACMP4,
        kPeriod   = LL_HRTIM_ADCTRIG_SRC13_TIMAPER,
        kReset    = LL_HRTIM_ADCTRIG_SRC13_TIMARST,
    };

    enum class HRTIMTimerBADCTriggerSourceID13 : uint32_t
    {
        kNone     = LL_HRTIM_ADCTRIG_SRC13_NONE,
        kEEV1     = LL_HRTIM_ADCTRIG_SRC13_EEV1,
        kEEV2     = LL_HRTIM_ADCTRIG_SRC13_EEV2,
        kEEV3     = LL_HRTIM_ADCTRIG_SRC13_EEV3,
        kEEV4     = LL_HRTIM_ADCTRIG_SRC13_EEV4,
        kEEV5     = LL_HRTIM_ADCTRIG_SRC13_EEV5,
        kCompare3 = LL_HRTIM_ADCTRIG_SRC13_TIMBCMP3,
        kCompare4 = LL_HRTIM_ADCTRIG_SRC13_TIMBCMP4,
        kPeriod   = LL_HRTIM_ADCTRIG_SRC13_TIMBPER,
        kReset    = LL_HRTIM_ADCTRIG_SRC13_TIMBRST,
    };

    enum class HRTIMTimerCADCTriggerSourceID13 : uint32_t
    {
        kNone     = LL_HRTIM_ADCTRIG_SRC13_NONE,
        kEEV1     = LL_HRTIM_ADCTRIG_SRC13_EEV1,
        kEEV2     = LL_HRTIM_ADCTRIG_SRC13_EEV2,
        kEEV3     = LL_HRTIM_ADCTRIG_SRC13_EEV3,
        kEEV4     = LL_HRTIM_ADCTRIG_SRC13_EEV4,
        kEEV5     = LL_HRTIM_ADCTRIG_SRC13_EEV5,
        kCompare3 = LL_HRTIM_ADCTRIG_SRC13_TIMCCMP3,
        kCompare4 = LL_HRTIM_ADCTRIG_SRC13_TIMCCMP4,
        kPeriod   = LL_HRTIM_ADCTRIG_SRC13_TIMCPER,
    };

    enum class HRTIMTimerDADCTriggerSourceID13 : uint32_t
    {
        kNone     = LL_HRTIM_ADCTRIG_SRC13_NONE,
        kEEV1     = LL_HRTIM_ADCTRIG_SRC13_EEV1,
        kEEV2     = LL_HRTIM_ADCTRIG_SRC13_EEV2,
        kEEV3     = LL_HRTIM_ADCTRIG_SRC13_EEV3,
        kEEV4     = LL_HRTIM_ADCTRIG_SRC13_EEV4,
        kEEV5     = LL_HRTIM_ADCTRIG_SRC13_EEV5,
        kCompare3 = LL_HRTIM_ADCTRIG_SRC13_TIMDCMP3,
        kCompare4 = LL_HRTIM_ADCTRIG_SRC13_TIMDCMP4,
        kPeriod   = LL_HRTIM_ADCTRIG_SRC13_TIMDPER,
    };

    enum class HRTIMTimerEADCTriggerSourceID13 : uint32_t
    {
        kNone     = LL_HRTIM_ADCTRIG_SRC13_NONE,
        kEEV1     = LL_HRTIM_ADCTRIG_SRC13_EEV1,
        kEEV2     = LL_HRTIM_ADCTRIG_SRC13_EEV2,
        kEEV3     = LL_HRTIM_ADCTRIG_SRC13_EEV3,
        kEEV4     = LL_HRTIM_ADCTRIG_SRC13_EEV4,
        kEEV5     = LL_HRTIM_ADCTRIG_SRC13_EEV5,
        kCompare3 = LL_HRTIM_ADCTRIG_SRC13_TIMECMP3,
        kCompare4 = LL_HRTIM_ADCTRIG_SRC13_TIMECMP4,
        kPeriod   = LL_HRTIM_ADCTRIG_SRC13_TIMEPER,
    };

    enum class HRTIMTimerFADCTriggerSourceID13 : uint32_t
    {
        kNone     = LL_HRTIM_ADCTRIG_SRC13_NONE,
        kEEV1     = LL_HRTIM_ADCTRIG_SRC13_EEV1,
        kEEV2     = LL_HRTIM_ADCTRIG_SRC13_EEV2,
        kEEV3     = LL_HRTIM_ADCTRIG_SRC13_EEV3,
        kEEV4     = LL_HRTIM_ADCTRIG_SRC13_EEV4,
        kEEV5     = LL_HRTIM_ADCTRIG_SRC13_EEV5,
        kCompare2 = LL_HRTIM_ADCTRIG_SRC13_TIMFCMP2,
        kCompare3 = LL_HRTIM_ADCTRIG_SRC13_TIMFCMP3,
        kCompare4 = LL_HRTIM_ADCTRIG_SRC13_TIMFCMP4,
        kPeriod   = LL_HRTIM_ADCTRIG_SRC13_TIMFPER,
        kReset    = LL_HRTIM_ADCTRIG_SRC13_TIMFRST,
    };

    enum class HRTIMTimerMasterADCTriggerSourceID24 : uint32_t
    {
        kSrcNone            = LL_HRTIM_ADCTRIG_SRC24_NONE,
        kSrcMasterCompare1  = LL_HRTIM_ADCTRIG_SRC24_MCMP1,
        kSrcMasterCompare2  = LL_HRTIM_ADCTRIG_SRC24_MCMP2,
        kSrcMasterCompare3  = LL_HRTIM_ADCTRIG_SRC24_MCMP3,
        kSrcMasterCompare4  = LL_HRTIM_ADCTRIG_SRC24_MCMP4,
        kSrcMasterPeriod    = LL_HRTIM_ADCTRIG_SRC24_MPER,
        kSrcExternalEvent6  = LL_HRTIM_ADCTRIG_SRC24_EEV6,
        kSrcExternalEvent7  = LL_HRTIM_ADCTRIG_SRC24_EEV7,
        kSrcExternalEvent8  = LL_HRTIM_ADCTRIG_SRC24_EEV8,
        kSrcExternalEvent9  = LL_HRTIM_ADCTRIG_SRC24_EEV9,
        kSrcExternalEvent10 = LL_HRTIM_ADCTRIG_SRC24_EEV10,
    };

    enum class HRTIMTimerAADCTriggerSourceID24 : uint32_t
    {
        kSrcNone            = LL_HRTIM_ADCTRIG_SRC24_NONE,
        kSrcExternalEvent6  = LL_HRTIM_ADCTRIG_SRC24_EEV6,
        kSrcExternalEvent7  = LL_HRTIM_ADCTRIG_SRC24_EEV7,
        kSrcExternalEvent8  = LL_HRTIM_ADCTRIG_SRC24_EEV8,
        kSrcExternalEvent9  = LL_HRTIM_ADCTRIG_SRC24_EEV9,
        kSrcExternalEvent10 = LL_HRTIM_ADCTRIG_SRC24_EEV10,
        kSrcTimerACompare2  = LL_HRTIM_ADCTRIG_SRC24_TIMACMP2,
        kSrcTimerACompare4  = LL_HRTIM_ADCTRIG_SRC24_TIMACMP4,
        kSrcTimerAPeriod    = LL_HRTIM_ADCTRIG_SRC24_TIMAPER,
    };

    enum class HRTIMTimerBADCTriggerSourceID24 : uint32_t
    {
        kSrcNone            = LL_HRTIM_ADCTRIG_SRC24_NONE,
        kSrcExternalEvent6  = LL_HRTIM_ADCTRIG_SRC24_EEV6,
        kSrcExternalEvent7  = LL_HRTIM_ADCTRIG_SRC24_EEV7,
        kSrcExternalEvent8  = LL_HRTIM_ADCTRIG_SRC24_EEV8,
        kSrcExternalEvent9  = LL_HRTIM_ADCTRIG_SRC24_EEV9,
        kSrcExternalEvent10 = LL_HRTIM_ADCTRIG_SRC24_EEV10,
        kSrcTimerBCompare2  = LL_HRTIM_ADCTRIG_SRC24_TIMBCMP2,
        kSrcTimerBCompare4  = LL_HRTIM_ADCTRIG_SRC24_TIMBCMP4,
        kSrcTimerBPeriod    = LL_HRTIM_ADCTRIG_SRC24_TIMBPER,
    };

    enum class HRTIMTimerCADCTriggerSourceID24 : uint32_t
    {
        kSrcNone            = LL_HRTIM_ADCTRIG_SRC24_NONE,
        kSrcExternalEvent6  = LL_HRTIM_ADCTRIG_SRC24_EEV6,
        kSrcExternalEvent7  = LL_HRTIM_ADCTRIG_SRC24_EEV7,
        kSrcExternalEvent8  = LL_HRTIM_ADCTRIG_SRC24_EEV8,
        kSrcExternalEvent9  = LL_HRTIM_ADCTRIG_SRC24_EEV9,
        kSrcExternalEvent10 = LL_HRTIM_ADCTRIG_SRC24_EEV10,
        kSrcTimerCCompare2  = LL_HRTIM_ADCTRIG_SRC24_TIMCCMP2,
        kSrcTimerCCompare4  = LL_HRTIM_ADCTRIG_SRC24_TIMCCMP4,
        kSrcTimerCPeriod    = LL_HRTIM_ADCTRIG_SRC24_TIMCPER,
        kSrcTimerCReset     = LL_HRTIM_ADCTRIG_SRC24_TIMCRST,
    };

    enum class HRTIMTimerDADCTriggerSourceID24 : uint32_t
    {
        kSrcNone            = LL_HRTIM_ADCTRIG_SRC24_NONE,
        kSrcExternalEvent6  = LL_HRTIM_ADCTRIG_SRC24_EEV6,
        kSrcExternalEvent7  = LL_HRTIM_ADCTRIG_SRC24_EEV7,
        kSrcExternalEvent8  = LL_HRTIM_ADCTRIG_SRC24_EEV8,
        kSrcExternalEvent9  = LL_HRTIM_ADCTRIG_SRC24_EEV9,
        kSrcExternalEvent10 = LL_HRTIM_ADCTRIG_SRC24_EEV10,
        kSrcTimerDCompare2  = LL_HRTIM_ADCTRIG_SRC24_TIMDCMP2,
        kSrcTimerDCompare4  = LL_HRTIM_ADCTRIG_SRC24_TIMDCMP4,
        kSrcTimerDPeriod    = LL_HRTIM_ADCTRIG_SRC24_TIMDPER,
        kSrcTimerDReset     = LL_HRTIM_ADCTRIG_SRC24_TIMDRST,
    };

    enum class HRTIMTimerEADCTriggerSourceID24 : uint32_t
    {
        kSrcNone            = LL_HRTIM_ADCTRIG_SRC24_NONE,
        kSrcExternalEvent6  = LL_HRTIM_ADCTRIG_SRC24_EEV6,
        kSrcExternalEvent7  = LL_HRTIM_ADCTRIG_SRC24_EEV7,
        kSrcExternalEvent8  = LL_HRTIM_ADCTRIG_SRC24_EEV8,
        kSrcExternalEvent9  = LL_HRTIM_ADCTRIG_SRC24_EEV9,
        kSrcExternalEvent10 = LL_HRTIM_ADCTRIG_SRC24_EEV10,
        kSrcTimerECompare2  = LL_HRTIM_ADCTRIG_SRC24_TIMECMP2,
        kSrcTimerECompare3  = LL_HRTIM_ADCTRIG_SRC24_TIMECMP3,
        kSrcTimerECompare4  = LL_HRTIM_ADCTRIG_SRC24_TIMECMP4,
        kSrcTimerEReset     = LL_HRTIM_ADCTRIG_SRC24_TIMERST,
    };

    enum class HRTIMTimerFADCTriggerSourceID24 : uint32_t
    {
        kSrcNone            = LL_HRTIM_ADCTRIG_SRC24_NONE,
        kSrcExternalEvent6  = LL_HRTIM_ADCTRIG_SRC24_EEV6,
        kSrcExternalEvent7  = LL_HRTIM_ADCTRIG_SRC24_EEV7,
        kSrcExternalEvent8  = LL_HRTIM_ADCTRIG_SRC24_EEV8,
        kSrcExternalEvent9  = LL_HRTIM_ADCTRIG_SRC24_EEV9,
        kSrcExternalEvent10 = LL_HRTIM_ADCTRIG_SRC24_EEV10,
        kSrcTimerFCompare2  = LL_HRTIM_ADCTRIG_SRC24_TIMFCMP2,
        kSrcTimerFCompare3  = LL_HRTIM_ADCTRIG_SRC24_TIMFCMP3,
        kSrcTimerFCompare4  = LL_HRTIM_ADCTRIG_SRC24_TIMFCMP4,
        kSrcTimerFPeriod    = LL_HRTIM_ADCTRIG_SRC24_TIMFPER,
    };

    enum class HRTIMTimerMasterADCTriggerSourceID579 : uint32_t
    {
        kMasterCompare1 = LL_HRTIM_ADCTRIG_SRC579_MCMP1,
        kMasterCompare2 = LL_HRTIM_ADCTRIG_SRC579_MCMP2,
        kMasterCompare3 = LL_HRTIM_ADCTRIG_SRC579_MCMP3,
        kMasterCompare4 = LL_HRTIM_ADCTRIG_SRC579_MCMP4,
        kMasterPeriod   = LL_HRTIM_ADCTRIG_SRC579_MPER,
        kExternalEvent1 = LL_HRTIM_ADCTRIG_SRC579_EEV1,
        kExternalEvent2 = LL_HRTIM_ADCTRIG_SRC579_EEV2,
        kExternalEvent3 = LL_HRTIM_ADCTRIG_SRC579_EEV3,
        kExternalEvent4 = LL_HRTIM_ADCTRIG_SRC579_EEV4,
        kExternalEvent5 = LL_HRTIM_ADCTRIG_SRC579_EEV5,
    };

    enum class HRTIMTimerAADCTriggerSourceID579 : uint32_t
    {
        kExternalEvent1 = LL_HRTIM_ADCTRIG_SRC579_EEV1,
        kExternalEvent2 = LL_HRTIM_ADCTRIG_SRC579_EEV2,
        kExternalEvent3 = LL_HRTIM_ADCTRIG_SRC579_EEV3,
        kExternalEvent4 = LL_HRTIM_ADCTRIG_SRC579_EEV4,
        kExternalEvent5 = LL_HRTIM_ADCTRIG_SRC579_EEV5,
        kTimerACompare3 = LL_HRTIM_ADCTRIG_SRC579_TIMA_CMP3,
        kTimerACompare4 = LL_HRTIM_ADCTRIG_SRC579_TIMA_CMP4,
        kTimerAPeriod   = LL_HRTIM_ADCTRIG_SRC579_TIMA_PER,
        kTimerAReset    = LL_HRTIM_ADCTRIG_SRC579_TIMA_RST,
    };

    enum class HRTIMTimerBADCTriggerSourceID579 : uint32_t
    {
        kExternalEvent1 = LL_HRTIM_ADCTRIG_SRC579_EEV1,
        kExternalEvent2 = LL_HRTIM_ADCTRIG_SRC579_EEV2,
        kExternalEvent3 = LL_HRTIM_ADCTRIG_SRC579_EEV3,
        kExternalEvent4 = LL_HRTIM_ADCTRIG_SRC579_EEV4,
        kExternalEvent5 = LL_HRTIM_ADCTRIG_SRC579_EEV5,
        kTimerBCompare3 = LL_HRTIM_ADCTRIG_SRC579_TIMB_CMP3,
        kTimerBCompare4 = LL_HRTIM_ADCTRIG_SRC579_TIMB_CMP4,
        kTimerBPeriod   = LL_HRTIM_ADCTRIG_SRC579_TIMB_PER,
        kTimerBReset    = LL_HRTIM_ADCTRIG_SRC579_TIMB_RST,
    };

    enum class HRTIMTimerCADCTriggerSourceID579 : uint32_t
    {
        kExternalEvent1 = LL_HRTIM_ADCTRIG_SRC579_EEV1,
        kExternalEvent2 = LL_HRTIM_ADCTRIG_SRC579_EEV2,
        kExternalEvent3 = LL_HRTIM_ADCTRIG_SRC579_EEV3,
        kExternalEvent4 = LL_HRTIM_ADCTRIG_SRC579_EEV4,
        kExternalEvent5 = LL_HRTIM_ADCTRIG_SRC579_EEV5,
        kTimerCCompare3 = LL_HRTIM_ADCTRIG_SRC579_TIMC_CMP3,
        kTimerCCompare4 = LL_HRTIM_ADCTRIG_SRC579_TIMC_CMP4,
        kTimerCPeriod   = LL_HRTIM_ADCTRIG_SRC579_TIMC_PER,
    };

    enum class HRTIMTimerDADCTriggerSourceID579 : uint32_t
    {
        kExternalEvent1 = LL_HRTIM_ADCTRIG_SRC579_EEV1,
        kExternalEvent2 = LL_HRTIM_ADCTRIG_SRC579_EEV2,
        kExternalEvent3 = LL_HRTIM_ADCTRIG_SRC579_EEV3,
        kExternalEvent4 = LL_HRTIM_ADCTRIG_SRC579_EEV4,
        kExternalEvent5 = LL_HRTIM_ADCTRIG_SRC579_EEV5,
        kTimerDCompare3 = LL_HRTIM_ADCTRIG_SRC579_TIMD_CMP3,
        kTimerDCompare4 = LL_HRTIM_ADCTRIG_SRC579_TIMD_CMP4,
        kTimerDPeriod   = LL_HRTIM_ADCTRIG_SRC579_TIMD_PER,
    };

    enum class HRTIMTimerEADCTriggerSourceID579 : uint32_t
    {
        kExternalEvent1 = LL_HRTIM_ADCTRIG_SRC579_EEV1,
        kExternalEvent2 = LL_HRTIM_ADCTRIG_SRC579_EEV2,
        kExternalEvent3 = LL_HRTIM_ADCTRIG_SRC579_EEV3,
        kExternalEvent4 = LL_HRTIM_ADCTRIG_SRC579_EEV4,
        kExternalEvent5 = LL_HRTIM_ADCTRIG_SRC579_EEV5,
        kTimerECompare3 = LL_HRTIM_ADCTRIG_SRC579_TIME_CMP3,
        kTimerECompare4 = LL_HRTIM_ADCTRIG_SRC579_TIME_CMP4,
        kTimerEPeriod   = LL_HRTIM_ADCTRIG_SRC579_TIME_PER,
    };

    enum class HRTIMTimerFADCTriggerSourceID579 : uint32_t
    {
        kExternalEvent1 = LL_HRTIM_ADCTRIG_SRC579_EEV1,
        kExternalEvent2 = LL_HRTIM_ADCTRIG_SRC579_EEV2,
        kExternalEvent3 = LL_HRTIM_ADCTRIG_SRC579_EEV3,
        kExternalEvent4 = LL_HRTIM_ADCTRIG_SRC579_EEV4,
        kExternalEvent5 = LL_HRTIM_ADCTRIG_SRC579_EEV5,
        kTimerFCompare2 = LL_HRTIM_ADCTRIG_SRC579_TIMF_CMP2,
        kTimerFCompare3 = LL_HRTIM_ADCTRIG_SRC579_TIMF_CMP3,
        kTimerFCompare4 = LL_HRTIM_ADCTRIG_SRC579_TIMF_CMP4,
        kTimerFPeriod   = LL_HRTIM_ADCTRIG_SRC579_TIMF_PER,
        kTimerFReset    = LL_HRTIM_ADCTRIG_SRC579_TIMF_RST,
    };

    enum class HRTIMTimerMasterADCTriggerSourceID6810 : uint32_t
    {
        kSrc6810MasterCompare1  = LL_HRTIM_ADCTRIG_SRC6810_MCMP1,
        kSrc6810MasterCompare2  = LL_HRTIM_ADCTRIG_SRC6810_MCMP2,
        kSrc6810MasterCompare3  = LL_HRTIM_ADCTRIG_SRC6810_MCMP3,
        kSrc6810MasterCompare4  = LL_HRTIM_ADCTRIG_SRC6810_MCMP4,
        kSrc6810MasterPeriod    = LL_HRTIM_ADCTRIG_SRC6810_MPER,
        kSrc6810ExternalEvent6  = LL_HRTIM_ADCTRIG_SRC6810_EEV6,
        kSrc6810ExternalEvent7  = LL_HRTIM_ADCTRIG_SRC6810_EEV7,
        kSrc6810ExternalEvent8  = LL_HRTIM_ADCTRIG_SRC6810_EEV8,
        kSrc6810ExternalEvent9  = LL_HRTIM_ADCTRIG_SRC6810_EEV9,
        kSrc6810ExternalEvent10 = LL_HRTIM_ADCTRIG_SRC6810_EEV10,
    };

    enum class HRTIMTimerAADCTriggerSourceID6810 : uint32_t
    {
        kSrc6810ExternalEvent6  = LL_HRTIM_ADCTRIG_SRC6810_EEV6,
        kSrc6810ExternalEvent7  = LL_HRTIM_ADCTRIG_SRC6810_EEV7,
        kSrc6810ExternalEvent8  = LL_HRTIM_ADCTRIG_SRC6810_EEV8,
        kSrc6810ExternalEvent9  = LL_HRTIM_ADCTRIG_SRC6810_EEV9,
        kSrc6810ExternalEvent10 = LL_HRTIM_ADCTRIG_SRC6810_EEV10,
        kSrc6810TimerACompare2  = LL_HRTIM_ADCTRIG_SRC6810_TIMA_CMP2,
        kSrc6810TimerACompare4  = LL_HRTIM_ADCTRIG_SRC6810_TIMA_CMP4,
        kSrc6810TimerAPeriod    = LL_HRTIM_ADCTRIG_SRC6810_TIMA_PER,
    };

    enum class HRTIMTimerBADCTriggerSourceID6810 : uint32_t
    {
        kSrc6810ExternalEvent6  = LL_HRTIM_ADCTRIG_SRC6810_EEV6,
        kSrc6810ExternalEvent7  = LL_HRTIM_ADCTRIG_SRC6810_EEV7,
        kSrc6810ExternalEvent8  = LL_HRTIM_ADCTRIG_SRC6810_EEV8,
        kSrc6810ExternalEvent9  = LL_HRTIM_ADCTRIG_SRC6810_EEV9,
        kSrc6810ExternalEvent10 = LL_HRTIM_ADCTRIG_SRC6810_EEV10,
        kSrc6810TimerBCompare2  = LL_HRTIM_ADCTRIG_SRC6810_TIMB_CMP2,
        kSrc6810TimerBCompare4  = LL_HRTIM_ADCTRIG_SRC6810_TIMB_CMP4,
        kSrc6810TimerBPeriod    = LL_HRTIM_ADCTRIG_SRC6810_TIMB_PER,
    };

    enum class HRTIMTimerCADCTriggerSourceID6810 : uint32_t
    {
        kSrc6810ExternalEvent6  = LL_HRTIM_ADCTRIG_SRC6810_EEV6,
        kSrc6810ExternalEvent7  = LL_HRTIM_ADCTRIG_SRC6810_EEV7,
        kSrc6810ExternalEvent8  = LL_HRTIM_ADCTRIG_SRC6810_EEV8,
        kSrc6810ExternalEvent9  = LL_HRTIM_ADCTRIG_SRC6810_EEV9,
        kSrc6810ExternalEvent10 = LL_HRTIM_ADCTRIG_SRC6810_EEV10,
        kSrc6810TimerCCompare2  = LL_HRTIM_ADCTRIG_SRC6810_TIMC_CMP2,
        kSrc6810TimerCCompare4  = LL_HRTIM_ADCTRIG_SRC6810_TIMC_CMP4,
        kSrc6810TimerCPeriod    = LL_HRTIM_ADCTRIG_SRC6810_TIMC_PER,
        kSrc6810TimerCReset     = LL_HRTIM_ADCTRIG_SRC6810_TIMC_RST,
    };

    enum class HRTIMTimerDADCTriggerSourceID6810 : uint32_t
    {
        kSrc6810ExternalEvent6  = LL_HRTIM_ADCTRIG_SRC6810_EEV6,
        kSrc6810ExternalEvent7  = LL_HRTIM_ADCTRIG_SRC6810_EEV7,
        kSrc6810ExternalEvent8  = LL_HRTIM_ADCTRIG_SRC6810_EEV8,
        kSrc6810ExternalEvent9  = LL_HRTIM_ADCTRIG_SRC6810_EEV9,
        kSrc6810ExternalEvent10 = LL_HRTIM_ADCTRIG_SRC6810_EEV10,
        kSrc6810TimerDCompare2  = LL_HRTIM_ADCTRIG_SRC6810_TIMD_CMP2,
        kSrc6810TimerDCompare4  = LL_HRTIM_ADCTRIG_SRC6810_TIMD_CMP4,
        kSrc6810TimerDPeriod    = LL_HRTIM_ADCTRIG_SRC6810_TIMD_PER,
        kSrc6810TimerDReset     = LL_HRTIM_ADCTRIG_SRC6810_TIMD_RST,
    };

    enum class HRTIMTimerEADCTriggerSourceID6810 : uint32_t
    {
        kSrc6810ExternalEvent6  = LL_HRTIM_ADCTRIG_SRC6810_EEV6,
        kSrc6810ExternalEvent7  = LL_HRTIM_ADCTRIG_SRC6810_EEV7,
        kSrc6810ExternalEvent8  = LL_HRTIM_ADCTRIG_SRC6810_EEV8,
        kSrc6810ExternalEvent9  = LL_HRTIM_ADCTRIG_SRC6810_EEV9,
        kSrc6810ExternalEvent10 = LL_HRTIM_ADCTRIG_SRC6810_EEV10,
        kSrc6810TimerECompare2  = LL_HRTIM_ADCTRIG_SRC6810_TIME_CMP2,
        kSrc6810TimerECompare3  = LL_HRTIM_ADCTRIG_SRC6810_TIME_CMP3,
        kSrc6810TimerECompare4  = LL_HRTIM_ADCTRIG_SRC6810_TIME_CMP4,
        kSrc6810TimerEReset     = LL_HRTIM_ADCTRIG_SRC6810_TIME_RST,
    };

    enum class HRTIMTimerFADCTriggerSourceID6810 : uint32_t
    {
        kSrc6810ExternalEvent6  = LL_HRTIM_ADCTRIG_SRC6810_EEV6,
        kSrc6810ExternalEvent7  = LL_HRTIM_ADCTRIG_SRC6810_EEV7,
        kSrc6810ExternalEvent8  = LL_HRTIM_ADCTRIG_SRC6810_EEV8,
        kSrc6810ExternalEvent9  = LL_HRTIM_ADCTRIG_SRC6810_EEV9,
        kSrc6810ExternalEvent10 = LL_HRTIM_ADCTRIG_SRC6810_EEV10,
        kSrc6810TimerFCompare2  = LL_HRTIM_ADCTRIG_SRC6810_TIMF_CMP2,
        kSrc6810TimerFCompare3  = LL_HRTIM_ADCTRIG_SRC6810_TIMF_CMP3,
        kSrc6810TimerFCompare4  = LL_HRTIM_ADCTRIG_SRC6810_TIMF_CMP4,
        kSrc6810TimerFPeriod    = LL_HRTIM_ADCTRIG_SRC6810_TIMF_PER,
    };

    template <HRTIMTimerID tkTimerID>
    using HRTIMTimerADCTriggerSourceID13 = std::conditional_t<
        tkTimerID == HRTIMTimerID::kA,
        HRTIMTimerAADCTriggerSourceID13,
        std::conditional_t<
            tkTimerID == HRTIMTimerID::kB,
            HRTIMTimerBADCTriggerSourceID13,
            std::conditional_t<tkTimerID == HRTIMTimerID::kC,
                               HRTIMTimerCADCTriggerSourceID13,
                               std::conditional_t<tkTimerID == HRTIMTimerID::kD,
                                                  HRTIMTimerDADCTriggerSourceID13,
                                                  std::conditional_t<tkTimerID == HRTIMTimerID::kE,
                                                                     HRTIMTimerEADCTriggerSourceID13,
                                                                     std::conditional_t<tkTimerID == HRTIMTimerID::kF,
                                                                                        HRTIMTimerFADCTriggerSourceID13,
                                                                                        void>>>>>>;

    template <HRTIMTimerID tkTimerID>
    using HRTIMTimerADCTriggerSourceID24 = std::conditional_t<
        tkTimerID == HRTIMTimerID::kA,
        HRTIMTimerAADCTriggerSourceID24,
        std::conditional_t<
            tkTimerID == HRTIMTimerID::kB,
            HRTIMTimerBADCTriggerSourceID24,
            std::conditional_t<tkTimerID == HRTIMTimerID::kC,
                               HRTIMTimerCADCTriggerSourceID24,
                               std::conditional_t<tkTimerID == HRTIMTimerID::kD,
                                                  HRTIMTimerDADCTriggerSourceID24,
                                                  std::conditional_t<tkTimerID == HRTIMTimerID::kE,
                                                                     HRTIMTimerEADCTriggerSourceID24,
                                                                     std::conditional_t<tkTimerID == HRTIMTimerID::kF,
                                                                                        HRTIMTimerFADCTriggerSourceID24,
                                                                                        void>>>>>>;

    template <HRTIMTimerID tkTimerID>
    using HRTIMTimerADCTriggerSourceID579 = std::conditional_t<
        tkTimerID == HRTIMTimerID::kA,
        HRTIMTimerAADCTriggerSourceID579,
        std::conditional_t<
            tkTimerID == HRTIMTimerID::kB,
            HRTIMTimerBADCTriggerSourceID579,
            std::conditional_t<
                tkTimerID == HRTIMTimerID::kC,
                HRTIMTimerCADCTriggerSourceID579,
                std::conditional_t<
                    tkTimerID == HRTIMTimerID::kD,
                    HRTIMTimerDADCTriggerSourceID579,
                    std::conditional_t<
                        tkTimerID == HRTIMTimerID::kE,
                        HRTIMTimerEADCTriggerSourceID579,
                        std::conditional_t<tkTimerID == HRTIMTimerID::kF, HRTIMTimerFADCTriggerSourceID579, void>>>>>>;

    template <HRTIMTimerID tkTimerID>
    using HRTIMTimerADCTriggerSourceID6810 = std::conditional_t<
        tkTimerID == HRTIMTimerID::kA,
        HRTIMTimerAADCTriggerSourceID6810,
        std::conditional_t<
            tkTimerID == HRTIMTimerID::kB,
            HRTIMTimerBADCTriggerSourceID6810,
            std::conditional_t<
                tkTimerID == HRTIMTimerID::kC,
                HRTIMTimerCADCTriggerSourceID6810,
                std::conditional_t<
                    tkTimerID == HRTIMTimerID::kD,
                    HRTIMTimerDADCTriggerSourceID6810,
                    std::conditional_t<
                        tkTimerID == HRTIMTimerID::kE,
                        HRTIMTimerEADCTriggerSourceID6810,
                        std::conditional_t<tkTimerID == HRTIMTimerID::kF, HRTIMTimerFADCTriggerSourceID6810, void>>>>>>;

    template <HRTIMTimerID tkTimerID>
    using HRTIMTimerADCTriggerSourceID1 = HRTIMTimerADCTriggerSourceID13<tkTimerID>;

    template <HRTIMTimerID tkTimerID>
    using HRTIMTimerADCTriggerSourceID2 = HRTIMTimerADCTriggerSourceID24<tkTimerID>;

    template <HRTIMTimerID tkTimerID>
    using HRTIMTimerADCTriggerSourceID3 = HRTIMTimerADCTriggerSourceID13<tkTimerID>;

    template <HRTIMTimerID tkTimerID>
    using HRTIMTimerADCTriggerSourceID4 = HRTIMTimerADCTriggerSourceID24<tkTimerID>;

    template <HRTIMTimerID tkTimerID>
    using HRTIMTimerADCTriggerSourceID5 = HRTIMTimerADCTriggerSourceID579<tkTimerID>;
    template <HRTIMTimerID tkTimerID>
    using HRTIMTimerADCTriggerSourceID6 = HRTIMTimerADCTriggerSourceID6810<tkTimerID>;
    template <HRTIMTimerID tkTimerID>
    using HRTIMTimerADCTriggerSourceID7 = HRTIMTimerADCTriggerSourceID579<tkTimerID>;

    template <HRTIMTimerID tkTimerID>
    using HRTIMTimerADCTriggerSourceID8 = HRTIMTimerADCTriggerSourceID6810<tkTimerID>;

    template <HRTIMTimerID tkTimerID>
    using HRTIMTimerADCTriggerSourceID9 = HRTIMTimerADCTriggerSourceID579<tkTimerID>;

    template <HRTIMTimerID tkTimerID>
    using HRTIMTimerADCTriggerSourceID10 = HRTIMTimerADCTriggerSourceID6810<tkTimerID>;

    enum class HRTIMTimerADCTriggerID : uint32_t
    {
        kTrig1  = LL_HRTIM_ADCTRIG_1,
        kTrig2  = LL_HRTIM_ADCTRIG_2,
        kTrig3  = LL_HRTIM_ADCTRIG_3,
        kTrig4  = LL_HRTIM_ADCTRIG_4,
        kTrig5  = LL_HRTIM_ADCTRIG_5,
        kTrig6  = LL_HRTIM_ADCTRIG_6,
        kTrig7  = LL_HRTIM_ADCTRIG_7,
        kTrig8  = LL_HRTIM_ADCTRIG_8,
        kTrig9  = LL_HRTIM_ADCTRIG_9,
        kTrig10 = LL_HRTIM_ADCTRIG_10,
    };

    template <HRTIMTimerID tkTimerID, HRTIMTimerADCTriggerID tkTriggerID>
    using HRTIMTimerADCTriggerSourceID = std::conditional_t<
        tkTriggerID == HRTIMTimerADCTriggerID::kTrig1,
        HRTIMTimerADCTriggerSourceID1<tkTimerID>,
        std::conditional_t<
            tkTriggerID == HRTIMTimerADCTriggerID::kTrig2,
            HRTIMTimerADCTriggerSourceID2<tkTimerID>,
            std::conditional_t<
                tkTriggerID == HRTIMTimerADCTriggerID::kTrig3,
                HRTIMTimerADCTriggerSourceID3<tkTimerID>,
                std::conditional_t<
                    tkTriggerID == HRTIMTimerADCTriggerID::kTrig4,
                    HRTIMTimerADCTriggerSourceID4<tkTimerID>,
                    std::conditional_t<
                        tkTriggerID == HRTIMTimerADCTriggerID::kTrig5,
                        HRTIMTimerADCTriggerSourceID5<tkTimerID>,
                        std::conditional_t<
                            tkTriggerID == HRTIMTimerADCTriggerID::kTrig6,
                            HRTIMTimerADCTriggerSourceID6<tkTimerID>,
                            std::conditional_t<
                                tkTriggerID == HRTIMTimerADCTriggerID::kTrig7,
                                HRTIMTimerADCTriggerSourceID7<tkTimerID>,
                                std::conditional_t<
                                    tkTriggerID == HRTIMTimerADCTriggerID::kTrig8,
                                    HRTIMTimerADCTriggerSourceID8<tkTimerID>,
                                    std::conditional_t<
                                        tkTriggerID == HRTIMTimerADCTriggerID::kTrig9,
                                        HRTIMTimerADCTriggerSourceID9<tkTimerID>,
                                        std::conditional_t<tkTriggerID == HRTIMTimerADCTriggerID::kTrig10,
                                                           HRTIMTimerADCTriggerSourceID10<tkTimerID>,
                                                           void>>>>>>>>>>;

    enum class HRTIMTimerADCRolloverMode : uint32_t
    {
        kPeriodReset = LL_HRTIM_ROLLOVER_MODE_BOTH,
        kReset       = LL_HRTIM_ROLLOVER_MODE_RST,
        kPeriod      = LL_HRTIM_ROLLOVER_MODE_PER,
    };

    // ============================================================================
    // HARDWARE TRAITS
    // ============================================================================
    template <HRTIMControllerID tkControllerID>
        requires(kValidHRTIMControllerID<tkControllerID>)
    struct HRTIMControllerTraits;

    template <>
    struct HRTIMControllerTraits<1>
    {
        static inline HRTIM_TypeDef* const skInstance = HRTIM1;

        static void enable_clock()
        {
            LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_HRTIM1);
        }
    };

    // ---------------------------------------------------------------------------
    // HRTIM FAULT
    // ---------------------------------------------------------------------------
    template <HRTIMControllerID tkControllerID, HRTIMFaultID tkFaultID>
    struct HRTIMFaultTraits
    {
    private:
        [[nodiscard]] static constexpr uint32_t get_ll_id()
        {
            if constexpr (tkFaultID == HRTIMFaultID::kFault1)
            {
                return LL_HRTIM_FAULT_1;
            }
            else if constexpr (tkFaultID == HRTIMFaultID::kFault2)
            {
                return LL_HRTIM_FAULT_2;
            }
            else if constexpr (tkFaultID == HRTIMFaultID::kFault3)
            {
                return LL_HRTIM_FAULT_3;
            }
            else if constexpr (tkFaultID == HRTIMFaultID::kFault4)
            {
                return LL_HRTIM_FAULT_4;
            }
            else if constexpr (tkFaultID == HRTIMFaultID::kFault5)
            {
                return LL_HRTIM_FAULT_5;
            }
            else if constexpr (tkFaultID == HRTIMFaultID::kFault6)
            {
                return LL_HRTIM_FAULT_6;
            }
            else
            {
                static_assert(kAlwaysFalse<tkFaultID>, "Invalid HRTIMFaultID");
            }
        }

    public:
        static constexpr uint32_t skLLID = get_ll_id();
    };

    // ---------------------------------------------------------------------------
    // HRTIM EEV
    // ---------------------------------------------------------------------------
    template <HRTIMControllerID tkControllerID, HRTIMEEVID tkEEVID>
    struct HRTIMEEVTraits
    {
    private:
        [[nodiscard]] static constexpr uint32_t get_ll_id()
        {
            if constexpr (tkEEVID == HRTIMEEVID::kEEV1)
            {
                return LL_HRTIM_EVENT_1;
            }
            else if constexpr (tkEEVID == HRTIMEEVID::kEEV2)
            {
                return LL_HRTIM_EVENT_2;
            }
            else if constexpr (tkEEVID == HRTIMEEVID::kEEV3)
            {
                return LL_HRTIM_EVENT_3;
            }
            else if constexpr (tkEEVID == HRTIMEEVID::kEEV4)
            {
                return LL_HRTIM_EVENT_4;
            }
            else if constexpr (tkEEVID == HRTIMEEVID::kEEV5)
            {
                return LL_HRTIM_EVENT_5;
            }
            else if constexpr (tkEEVID == HRTIMEEVID::kEEV6)
            {
                return LL_HRTIM_EVENT_6;
            }
            else if constexpr (tkEEVID == HRTIMEEVID::kEEV7)
            {
                return LL_HRTIM_EVENT_7;
            }
            else if constexpr (tkEEVID == HRTIMEEVID::kEEV8)
            {
                return LL_HRTIM_EVENT_8;
            }
            else if constexpr (tkEEVID == HRTIMEEVID::kEEV9)
            {
                return LL_HRTIM_EVENT_9;
            }
            else if constexpr (tkEEVID == HRTIMEEVID::kEEV10)
            {
                return LL_HRTIM_EVENT_10;
            }
            else
            {
                static_assert(kAlwaysFalse<tkEEVID>, "Invalid HRTIMEEVID");
            }
        }

    public:
        static constexpr uint32_t skLLID = get_ll_id();
    };

    // ----------------------------------------------------------------------------
    // HRTIM TIMER
    // ----------------------------------------------------------------------------
    template <HRTIMControllerID tkControllerID, HRTIMTimerID tkTimerID>
    struct HRTIMTimerTraits;

    // HRTIM1 Timer A (tkHRTIMTimerID 0)
    template <>
    struct HRTIMTimerTraits<1, HRTIMTimerID::kA>
    {
        static constexpr uint32_t  skLLID            = LL_HRTIM_TIMER_A;
        static constexpr uint32_t  skTimerIdx        = HRTIM_TIMERINDEX_TIMER_A;
        static constexpr uint32_t  skOutput1         = HRTIM_OUTPUT_TA1;
        static constexpr uint32_t  skOutput2         = HRTIM_OUTPUT_TA2;
        static constexpr IRQn_Type skIRQn            = HRTIM1_TIMA_IRQn;
        static constexpr uint32_t  skADCTrigUpdateID = LL_HRTIM_ADCTRIG_UPDATE_TIMER_A;
    };

    // HRTIM1 Timer B
    template <>
    struct HRTIMTimerTraits<1, HRTIMTimerID::kB>
    {
        static constexpr uint32_t  skLLID            = LL_HRTIM_TIMER_B;
        static constexpr uint32_t  skTimerIdx        = HRTIM_TIMERINDEX_TIMER_B;
        static constexpr uint32_t  skOutput1         = HRTIM_OUTPUT_TB1;
        static constexpr uint32_t  skOutput2         = HRTIM_OUTPUT_TB2;
        static constexpr IRQn_Type skIRQn            = HRTIM1_TIMB_IRQn;
        static constexpr uint32_t  skADCTrigUpdateID = LL_HRTIM_ADCTRIG_UPDATE_TIMER_B;
    };

    // HRTIM1 Timer C
    template <>
    struct HRTIMTimerTraits<1, HRTIMTimerID::kC>
    {
        static constexpr uint32_t  skLLID            = LL_HRTIM_TIMER_C;
        static constexpr uint32_t  skTimerIdx        = HRTIM_TIMERINDEX_TIMER_C;
        static constexpr uint32_t  skOutput1         = HRTIM_OUTPUT_TC1;
        static constexpr uint32_t  skOutput2         = HRTIM_OUTPUT_TC2;
        static constexpr IRQn_Type skIRQn            = HRTIM1_TIMC_IRQn;
        static constexpr uint32_t  skADCTrigUpdateID = LL_HRTIM_ADCTRIG_UPDATE_TIMER_C;
    };

    // HRTIM1 Timer D
    template <>
    struct HRTIMTimerTraits<1, HRTIMTimerID::kD>
    {
        static constexpr uint32_t  skLLID            = LL_HRTIM_TIMER_D;
        static constexpr uint32_t  skTimerIdx        = HRTIM_TIMERINDEX_TIMER_D;
        static constexpr uint32_t  skOutput1         = HRTIM_OUTPUT_TD1;
        static constexpr uint32_t  skOutput2         = HRTIM_OUTPUT_TD2;
        static constexpr IRQn_Type skIRQn            = HRTIM1_TIMD_IRQn;
        static constexpr uint32_t  skADCTrigUpdateID = LL_HRTIM_ADCTRIG_UPDATE_TIMER_D;
    };

    // HRTIM1 Timer E
    template <>
    struct HRTIMTimerTraits<1, HRTIMTimerID::kE>
    {
        static constexpr uint32_t  skLLID            = LL_HRTIM_TIMER_E;
        static constexpr uint32_t  skTimerIdx        = HRTIM_TIMERINDEX_TIMER_E;
        static constexpr uint32_t  skOutput1         = HRTIM_OUTPUT_TE1;
        static constexpr uint32_t  skOutput2         = HRTIM_OUTPUT_TE2;
        static constexpr IRQn_Type skIRQn            = HRTIM1_TIME_IRQn;
        static constexpr uint32_t  skADCTrigUpdateID = LL_HRTIM_ADCTRIG_UPDATE_TIMER_E;
    };

    // HRTIM1 Timer F
    template <>
    struct HRTIMTimerTraits<1, HRTIMTimerID::kF>
    {
        static constexpr uint32_t  skLLID            = LL_HRTIM_TIMER_F;
        static constexpr uint32_t  skTimerIdx        = HRTIM_TIMERINDEX_TIMER_F;
        static constexpr uint32_t  skOutput1         = HRTIM_OUTPUT_TF1;
        static constexpr uint32_t  skOutput2         = HRTIM_OUTPUT_TF2;
        static constexpr IRQn_Type skIRQn            = HRTIM1_TIMF_IRQn;
        static constexpr uint32_t  skADCTrigUpdateID = LL_HRTIM_ADCTRIG_UPDATE_TIMER_F;
    };

    // ============================================================================
    // Interface
    // ============================================================================

    template <HRTIMControllerID tkControllerID, HRTIMTimerID tkTimerID>
    struct HRTIMTimerInterface
    {
        static constexpr HRTIMControllerID skControllerID = tkControllerID;
        static constexpr HRTIMTimerID      skTimerID      = tkTimerID;

        using ControllerTraitsT = HRTIMControllerTraits<tkControllerID>;
        using TimerTraitsT      = HRTIMTimerTraits<tkControllerID, tkTimerID>;

        static constexpr uint32_t            skMinPrescalerNum      = 0;
        static constexpr uint32_t            skMaxPrescalerNum      = 7;
        static constexpr HRTIMTimerPrescaler skMinPrescaler         = HRTIMTimerPrescaler::kMul32;
        static constexpr HRTIMTimerPrescaler skMaxPrescaler         = HRTIMTimerPrescaler::kDiv4;
        static constexpr uint32_t            skPrescalerScaleFactor = 32;

        static constexpr uint32_t skMaxPeriod = 65535;

        static constexpr uint32_t                    skDeadtimeMinPrescalerNum = 0;
        static constexpr uint32_t                    skDeadtimeMaxPrescalerNum = 7;
        static constexpr HRTIMTimerDeadtimePrescaler skDeadtimeMinPrescaler    = HRTIMTimerDeadtimePrescaler::kMul8;
        static constexpr HRTIMTimerDeadtimePrescaler skDeadtimeMaxPrescaler    = HRTIMTimerDeadtimePrescaler::kDiv16;
        static constexpr uint32_t                    skDeadtimePrescalerScaleFactor = 8;
        static constexpr uint32_t                    skDeadtimeCounterMax           = 511;

        // -------------------------------------------------------------------------
        // TIMING
        // -------------------------------------------------------------------------
        [[nodiscard]] static uint32_t get_freq_hz()
        {
            return get_apb2_timer_clock_freq();
        }

        [[nodiscard]] static HRTIMTimerPrescaler get_timer_prescaler()
        {
            return static_cast<HRTIMTimerPrescaler>(
                LL_HRTIM_TIM_GetPrescaler(ControllerTraitsT::skInstance, TimerTraitsT::skLLID));
        }

        [[nodiscard]] static uint8_t timer_prescaler_to_prescaler_num(HRTIMTimerPrescaler prescaler)
        {
            // NOLINTBEGIN(readability-magic-numbers)
            static_assert(static_cast<uint32_t>(HRTIMTimerPrescaler::kMul32) == 0,
                          "Assumes prescaler enum starts at 0 with Mul32");
            static_assert(static_cast<uint32_t>(HRTIMTimerPrescaler::kMul16) == 1,
                          "Assumes prescaler enum values are sequential");
            static_assert(static_cast<uint32_t>(HRTIMTimerPrescaler::kMul8) == 2,
                          "Assumes prescaler enum values are sequential");
            static_assert(static_cast<uint32_t>(HRTIMTimerPrescaler::kMul4) == 3,
                          "Assumes prescaler enum values are sequential");
            static_assert(static_cast<uint32_t>(HRTIMTimerPrescaler::kMul2) == 4,
                          "Assumes prescaler enum values are sequential");
            static_assert(static_cast<uint32_t>(HRTIMTimerPrescaler::kDiv1) == 5,
                          "Assumes prescaler enum values are sequential");
            static_assert(static_cast<uint32_t>(HRTIMTimerPrescaler::kDiv2) == 6,
                          "Assumes prescaler enum values are sequential");
            static_assert(static_cast<uint32_t>(HRTIMTimerPrescaler::kDiv4) == 7,
                          "Assumes prescaler enum values are sequential");
            // NOLINTEND(readability-magic-numbers)

            return static_cast<uint8_t>(prescaler);
        }

        [[nodiscard]] static constexpr HRTIMTimerPrescaler timer_prescaler_num_to_prescaler(uint8_t prescaler_num)
        {
            // NOLINTBEGIN(readability-magic-numbers)
            static_assert(static_cast<uint32_t>(HRTIMTimerPrescaler::kMul32) == 0,
                          "Assumes prescaler enum starts at 0 with Mul32");
            static_assert(static_cast<uint32_t>(HRTIMTimerPrescaler::kMul16) == 1,
                          "Assumes prescaler enum values are sequential");
            static_assert(static_cast<uint32_t>(HRTIMTimerPrescaler::kMul8) == 2,
                          "Assumes prescaler enum values are sequential");
            static_assert(static_cast<uint32_t>(HRTIMTimerPrescaler::kMul4) == 3,
                          "Assumes prescaler enum values are sequential");
            static_assert(static_cast<uint32_t>(HRTIMTimerPrescaler::kMul2) == 4,
                          "Assumes prescaler enum values are sequential");
            static_assert(static_cast<uint32_t>(HRTIMTimerPrescaler::kDiv1) == 5,
                          "Assumes prescaler enum values are sequential");
            static_assert(static_cast<uint32_t>(HRTIMTimerPrescaler::kDiv2) == 6,
                          "Assumes prescaler enum values are sequential");
            static_assert(static_cast<uint32_t>(HRTIMTimerPrescaler::kDiv4) == 7,
                          "Assumes prescaler enum values are sequential");
            // NOLINTEND(readability-magic-numbers)

            return static_cast<HRTIMTimerPrescaler>(prescaler_num);
        }

        [[nodiscard]] static uint32_t get_period_ticks()
        {
            return LL_HRTIM_TIM_GetPeriod(ControllerTraitsT::skInstance, TimerTraitsT::skLLID);
        }

        [[nodiscard]] static uint32_t get_repetition()
        {
            return LL_HRTIM_TIM_GetRepetition(ControllerTraitsT::skInstance, TimerTraitsT::skLLID);
        }

        [[nodiscard]] static uint64_t calculate_hrck_freq_hz(const uint32_t            f_hrtim_hz,
                                                             const HRTIMTimerPrescaler prescaler)
        {
            const uint8_t prescaler_num = timer_prescaler_to_prescaler_num(prescaler);
            return (static_cast<uint64_t>(f_hrtim_hz) * static_cast<uint64_t>(skPrescalerScaleFactor)) /
                   (1 << prescaler_num);
        }

        [[nodiscard]] static uint64_t get_hrck_freq_hz()
        {
            return calculate_hrck_freq_hz(get_freq_hz(), get_timer_prescaler());
        }

        // NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
        [[nodiscard]] static constexpr uint64_t calculate_timer_freq_hz(const uint64_t f_hrck_hz,
                                                                        const uint32_t period_ticks)
        {
            return f_hrck_hz / (period_ticks + 1);
        }

        [[nodiscard]] static uint64_t get_timer_freq_hz()
        {
            return calculate_timer_freq_hz(get_hrck_freq_hz(), get_period_ticks());
        }

        // NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
        [[nodiscard]] static constexpr uint64_t calculate_int_freq_hz(const uint32_t f_timer_hz,
                                                                      const uint32_t repetition)
        {
            return f_timer_hz / (repetition + 1);
        }

        [[nodiscard]] static uint64_t get_int_freq_hz()
        {
            return calculate_int_freq_hz(get_timer_freq_hz(), get_repetition());
        }

        [[nodiscard]] static constexpr uint64_t calculate_min_freq_hz(const uint32_t f_hrtim_hz)
        {
            // Minimum frequency is achieved with maximum prescaler (7) and maximum period (65535)
            return calculate_timer_freq_hz(calculate_hrck_freq_hz(f_hrtim_hz, HRTIMTimerPrescaler::kDiv4), skMaxPeriod);
        }

        [[nodiscard]] static constexpr uint64_t calculate_max_freq_hz(const uint32_t f_hrtim_hz)
        {
            // Maximum frequency is achieved with minimum prescaler (0) and minimum period (1)
            return calculate_timer_freq_hz(calculate_hrck_freq_hz(f_hrtim_hz, HRTIMTimerPrescaler::kMul32), 1);
        }

        [[nodiscard]] static constexpr HRTIMTimerPrescaler
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
        // DEADTIME
        // -------------------------------------------------------------------------

        [[nodiscard]] static constexpr HRTIMTimerDeadtimePrescaler deadtime_prescaler_num_to_prescaler(
            uint8_t prescaler_num)
        {
            static_assert(static_cast<HRTIMTimerDeadtimePrescaler>(0 << HRTIM_DTR_DTPRSC_Pos) ==
                              HRTIMTimerDeadtimePrescaler::kMul8,
                          "Assumes deadtime prescaler enum values are sequential and start at 0 with Mul8");

            static_assert(static_cast<HRTIMTimerDeadtimePrescaler>(1 << HRTIM_DTR_DTPRSC_Pos) ==
                              HRTIMTimerDeadtimePrescaler::kMul4,
                          "Assumes deadtime prescaler enum values are sequential and start at 0 with Mul8");

            static_assert(static_cast<HRTIMTimerDeadtimePrescaler>(2 << HRTIM_DTR_DTPRSC_Pos) ==
                              HRTIMTimerDeadtimePrescaler::kMul2,
                          "Assumes deadtime prescaler enum values are sequential and start at 0 with Mul8");

            static_assert(static_cast<HRTIMTimerDeadtimePrescaler>(3 << HRTIM_DTR_DTPRSC_Pos) ==
                              HRTIMTimerDeadtimePrescaler::kDiv1,
                          "Assumes deadtime prescaler enum values are sequential and start at 0 with Mul8");

            static_assert(static_cast<HRTIMTimerDeadtimePrescaler>(4 << HRTIM_DTR_DTPRSC_Pos) ==
                              HRTIMTimerDeadtimePrescaler::kDiv2,
                          "Assumes deadtime prescaler enum values are sequential and start at 0 with Mul8");

            static_assert(static_cast<HRTIMTimerDeadtimePrescaler>(5 << HRTIM_DTR_DTPRSC_Pos) ==
                              HRTIMTimerDeadtimePrescaler::kDiv4,
                          "Assumes deadtime prescaler enum values are sequential and start at 0 with Mul8");

            static_assert(static_cast<HRTIMTimerDeadtimePrescaler>(6 << HRTIM_DTR_DTPRSC_Pos) ==
                              HRTIMTimerDeadtimePrescaler::kDiv8,
                          "Assumes deadtime prescaler enum values are sequential and start at 0 with Mul8");

            static_assert(static_cast<HRTIMTimerDeadtimePrescaler>(7 << HRTIM_DTR_DTPRSC_Pos) ==
                              HRTIMTimerDeadtimePrescaler::kDiv16,
                          "Assumes deadtime prescaler enum values are sequential and start at 0 with Mul8");

            uint8_t clamped_num =
                std::clamp<uint8_t>(prescaler_num, skDeadtimeMinPrescalerNum, skDeadtimeMaxPrescalerNum);

            // Deadtime prescaler value is offset by bit 0 of the DTRRSC register (HRTIM_DTR_DTPRSC_Pos)
            return static_cast<HRTIMTimerDeadtimePrescaler>(clamped_num << HRTIM_DTR_DTPRSC_Pos);
        }

        [[nodiscard]] static constexpr uint8_t deadtime_prescaler_to_prescaler_num(
            const HRTIMTimerDeadtimePrescaler prescaler)
        {
            return (static_cast<uint32_t>(prescaler) >> HRTIM_DTR_DTPRSC_Pos) & 0x7;
        }

        [[nodiscard]] static constexpr float calculate_deadtime_period(const uint32_t                    f_hrtim_hz,
                                                                       const HRTIMTimerDeadtimePrescaler prescaler)
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
            return skDeadtimeCounterMax * calculate_deadtime_period(f_hrtim_hz, HRTIMTimerDeadtimePrescaler::kDiv16);
        }

        [[nodiscard]] static constexpr float calculate_min_deadtime_ns(const uint32_t f_hrtim_hz)
        {
            return calculate_deadtime_period(f_hrtim_hz, HRTIMTimerDeadtimePrescaler::kMul8);
        }

        [[nodiscard]] static constexpr HRTIMTimerDeadtimePrescaler
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
                                                                         const HRTIMTimerDeadtimePrescaler prescaler,
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
        static void enable_counter()
        {
            LL_HRTIM_TIM_CounterEnable(ControllerTraitsT::skInstance, TimerTraitsT::skLLID);
        }

        static void disable_counter()
        {
            LL_HRTIM_TIM_CounterDisable(ControllerTraitsT::skInstance, TimerTraitsT::skLLID);
        }

        // ----------------------------------------------------------------------------
        // OUTPUTS
        // ----------------------------------------------------------------------------
        static void enable_output1()
        {
            LL_HRTIM_TIM_EnableOutput(ControllerTraitsT::skInstance, TimerTraitsT::skOutput1);
        }

        static void disable_output1()
        {
            LL_HRTIM_TIM_DisableOutput(ControllerTraitsT::skInstance, TimerTraitsT::skOutput1);
        }

        static void enable_output2()
        {
            LL_HRTIM_TIM_EnableOutput(ControllerTraitsT::skInstance, TimerTraitsT::skOutput2);
        }

        static void disable_output2()
        {
            LL_HRTIM_TIM_DisableOutput(ControllerTraitsT::skInstance, TimerTraitsT::skOutput2);
        }

        static void enable_outputs()
        {
            LL_HRTIM_EnableOutput(ControllerTraitsT::skInstance, TimerTraitsT::skOutput1 | TimerTraitsT::skOutput2);
        }

        static void disable_outputs()
        {
            LL_HRTIM_DisableOutput(ControllerTraitsT::skInstance, TimerTraitsT::skOutput1 | TimerTraitsT::skOutput2);
        }

        // ----------------------------------------------------------------------------
        // COMPARE
        // ----------------------------------------------------------------------------

        template <HRTIMTimerCompareUnit tkCompareUnit>
        static constexpr void set_compare(uint32_t compare_value)
        {
            if constexpr (tkCompareUnit == HRTIMTimerCompareUnit::kCompare1)
            {
                LL_HRTIM_TIM_SetCompare1(ControllerTraitsT::skInstance, TimerTraitsT::skLLID, compare_value);
            }
            else if constexpr (tkCompareUnit == HRTIMTimerCompareUnit::kCompare2)
            {
                LL_HRTIM_TIM_SetCompare2(ControllerTraitsT::skInstance, TimerTraitsT::skLLID, compare_value);
            }
            else if constexpr (tkCompareUnit == HRTIMTimerCompareUnit::kCompare3)
            {
                LL_HRTIM_TIM_SetCompare3(ControllerTraitsT::skInstance, TimerTraitsT::skLLID, compare_value);
            }
            else if constexpr (tkCompareUnit == HRTIMTimerCompareUnit::kCompare4)
            {
                LL_HRTIM_TIM_SetCompare4(ControllerTraitsT::skInstance, TimerTraitsT::skLLID, compare_value);
            }
            else
            {
                static_assert(kAlwaysFalse<tkCompareUnit>, "Invalid HRTIMTimerCompareUnit");
            }
        }

        static constexpr void set_compare(HRTIMTimerCompareUnit compare_unit, uint32_t compare_value)
        {
            switch (compare_unit)
            {
                case HRTIMTimerCompareUnit::kCompare1:
                    set_compare<HRTIMTimerCompareUnit::kCompare1>(compare_value);
                    break;
                case HRTIMTimerCompareUnit::kCompare2:
                    set_compare<HRTIMTimerCompareUnit::kCompare2>(compare_value);
                    break;
                case HRTIMTimerCompareUnit::kCompare3:
                    set_compare<HRTIMTimerCompareUnit::kCompare3>(compare_value);
                    break;
                case HRTIMTimerCompareUnit::kCompare4:
                    set_compare<HRTIMTimerCompareUnit::kCompare4>(compare_value);
                    break;
                default:
                    // Invalid compare unit, default to Compare1
                    set_compare<HRTIMTimerCompareUnit::kCompare1>(compare_value);
                    break;
            }
        }

        template <HRTIMTimerCompareUnit tkCompareUnit>
        [[nodiscard]] static constexpr HRTIMTimerOutputSetSource set_source_for_compare_unit()
        {
            if constexpr (tkCompareUnit == HRTIMTimerCompareUnit::kCompare1)
            {
                return HRTIMTimerOutputSetSource::kTimerCompare1;
            }
            else if constexpr (tkCompareUnit == HRTIMTimerCompareUnit::kCompare2)
            {
                return HRTIMTimerOutputSetSource::kTimerCompare2;
            }
            else if constexpr (tkCompareUnit == HRTIMTimerCompareUnit::kCompare3)
            {
                return HRTIMTimerOutputSetSource::kTimerCompare3;
            }
            else if constexpr (tkCompareUnit == HRTIMTimerCompareUnit::kCompare4)
            {
                return HRTIMTimerOutputSetSource::kTimerCompare4;
            }
            else
            {
                static_assert(kAlwaysFalse<tkCompareUnit>, "Invalid HRTIMTimerCompareUnit");
            }
        }

        [[nodiscard]] static constexpr HRTIMTimerOutputSetSource set_source_for_compare_unit(
            HRTIMTimerCompareUnit compare_unit)
        {
            switch (compare_unit)
            {
                case HRTIMTimerCompareUnit::kCompare1:
                    return set_source_for_compare_unit<HRTIMTimerCompareUnit::kCompare1>();
                case HRTIMTimerCompareUnit::kCompare2:
                    return set_source_for_compare_unit<HRTIMTimerCompareUnit::kCompare2>();
                case HRTIMTimerCompareUnit::kCompare3:
                    return set_source_for_compare_unit<HRTIMTimerCompareUnit::kCompare3>();
                case HRTIMTimerCompareUnit::kCompare4:
                    return set_source_for_compare_unit<HRTIMTimerCompareUnit::kCompare4>();
                default:
                    // Invalid compare unit, default to Compare1
                    return set_source_for_compare_unit<HRTIMTimerCompareUnit::kCompare1>();
            }
        }

        template <HRTIMTimerCompareUnit tkCompareUnit>
        [[nodiscard]] static constexpr HRTIMTimerOutputResetSource reset_source_for_compare_unit()
        {
            if constexpr (tkCompareUnit == HRTIMTimerCompareUnit::kCompare1)
            {
                return HRTIMTimerOutputResetSource::kTimerCompare1;
            }
            else if constexpr (tkCompareUnit == HRTIMTimerCompareUnit::kCompare2)
            {
                return HRTIMTimerOutputResetSource::kTimerCompare2;
            }
            else if constexpr (tkCompareUnit == HRTIMTimerCompareUnit::kCompare3)
            {
                return HRTIMTimerOutputResetSource::kTimerCompare3;
            }
            else if constexpr (tkCompareUnit == HRTIMTimerCompareUnit::kCompare4)
            {
                return HRTIMTimerOutputResetSource::kTimerCompare4;
            }
            else
            {
                static_assert(kAlwaysFalse<tkCompareUnit>, "Invalid HRTIMTimerCompareUnit");
            }
        }

        [[nodiscard]] static constexpr HRTIMTimerOutputResetSource reset_source_for_compare_unit(
            HRTIMTimerCompareUnit compare_unit)
        {
            switch (compare_unit)
            {
                case HRTIMTimerCompareUnit::kCompare1:
                    return reset_source_for_compare_unit<HRTIMTimerCompareUnit::kCompare1>();
                case HRTIMTimerCompareUnit::kCompare2:
                    return reset_source_for_compare_unit<HRTIMTimerCompareUnit::kCompare2>();
                case HRTIMTimerCompareUnit::kCompare3:
                    return reset_source_for_compare_unit<HRTIMTimerCompareUnit::kCompare3>();
                case HRTIMTimerCompareUnit::kCompare4:
                    return reset_source_for_compare_unit<HRTIMTimerCompareUnit::kCompare4>();
                default:
                    // Invalid compare unit, default to Compare1
                    return reset_source_for_compare_unit<HRTIMTimerCompareUnit::kCompare1>();
            }
        }
    };

}  // namespace valle