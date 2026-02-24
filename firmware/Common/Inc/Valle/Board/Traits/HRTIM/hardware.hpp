#pragma once

#include "Valle/Board/Traits/HRTIM/id.hpp"
#include "Valle/Utils/template_utils.hpp"
#include "stm32g4xx_ll_bus.h"
#include "stm32g4xx_ll_hrtim.h"

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

    enum class HRTIMTimerOutputSetSource : uint32_t
    {
        kNone                       = LL_HRTIM_OUTPUTSET_NONE,
        kResync                     = LL_HRTIM_OUTPUTSET_RESYNC,
        kTimerPeriod                = LL_HRTIM_OUTPUTSET_TIMPER,
        kTimerCompare1              = LL_HRTIM_OUTPUTSET_TIMCMP1,
        kTimerCompare2              = LL_HRTIM_OUTPUTSET_TIMCMP2,
        kTimerCompare3              = LL_HRTIM_OUTPUTSET_TIMCMP3,
        kTimerCompare4              = LL_HRTIM_OUTPUTSET_TIMCMP4,
        kMasterPer                  = LL_HRTIM_OUTPUTSET_MASTERPER,
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
        static constexpr uint32_t  skLLID     = LL_HRTIM_TIMER_A;
        static constexpr uint32_t  skTimerIdx = HRTIM_TIMERINDEX_TIMER_A;
        static constexpr uint32_t  skOutput1  = HRTIM_OUTPUT_TA1;
        static constexpr uint32_t  skOutput2  = HRTIM_OUTPUT_TA2;
        static constexpr IRQn_Type skIRQn     = HRTIM1_TIMA_IRQn;
    };

    // HRTIM1 Timer B
    template <>
    struct HRTIMTimerTraits<1, HRTIMTimerID::kB>
    {
        static constexpr uint32_t  skLLID     = LL_HRTIM_TIMER_B;
        static constexpr uint32_t  skTimerIdx = HRTIM_TIMERINDEX_TIMER_B;
        static constexpr uint32_t  skOutput1  = HRTIM_OUTPUT_TB1;
        static constexpr uint32_t  skOutput2  = HRTIM_OUTPUT_TB2;
        static constexpr IRQn_Type skIRQn     = HRTIM1_TIMB_IRQn;
    };

    // HRTIM1 Timer C
    template <>
    struct HRTIMTimerTraits<1, HRTIMTimerID::kC>
    {
        static constexpr uint32_t  skLLID     = LL_HRTIM_TIMER_C;
        static constexpr uint32_t  skTimerIdx = HRTIM_TIMERINDEX_TIMER_C;
        static constexpr uint32_t  skOutput1  = HRTIM_OUTPUT_TC1;
        static constexpr uint32_t  skOutput2  = HRTIM_OUTPUT_TC2;
        static constexpr IRQn_Type skIRQn     = HRTIM1_TIMC_IRQn;
    };

    // HRTIM1 Timer D
    template <>
    struct HRTIMTimerTraits<1, HRTIMTimerID::kD>
    {
        static constexpr uint32_t  skLLID     = LL_HRTIM_TIMER_D;
        static constexpr uint32_t  skTimerIdx = HRTIM_TIMERINDEX_TIMER_D;
        static constexpr uint32_t  skOutput1  = HRTIM_OUTPUT_TD1;
        static constexpr uint32_t  skOutput2  = HRTIM_OUTPUT_TD2;
        static constexpr IRQn_Type skIRQn     = HRTIM1_TIMD_IRQn;
    };

    // HRTIM1 Timer E
    template <>
    struct HRTIMTimerTraits<1, HRTIMTimerID::kE>
    {
        static constexpr uint32_t  skLLID     = LL_HRTIM_TIMER_E;
        static constexpr uint32_t  skTimerIdx = HRTIM_TIMERINDEX_TIMER_E;
        static constexpr uint32_t  skOutput1  = HRTIM_OUTPUT_TE1;
        static constexpr uint32_t  skOutput2  = HRTIM_OUTPUT_TE2;
        static constexpr IRQn_Type skIRQn     = HRTIM1_TIME_IRQn;
    };

    // HRTIM1 Timer F
    template <>
    struct HRTIMTimerTraits<1, HRTIMTimerID::kF>
    {
        static constexpr uint32_t  skLLID     = LL_HRTIM_TIMER_F;
        static constexpr uint32_t  skTimerIdx = HRTIM_TIMERINDEX_TIMER_F;
        static constexpr uint32_t  skOutput1  = HRTIM_OUTPUT_TF1;
        static constexpr uint32_t  skOutput2  = HRTIM_OUTPUT_TF2;
        static constexpr IRQn_Type skIRQn     = HRTIM1_TIMF_IRQn;
    };

}  // namespace valle