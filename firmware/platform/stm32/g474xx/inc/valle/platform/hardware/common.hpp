#pragma once

#include "valle/platform/hardware/adc.hpp"
#include "valle/platform/hardware/hrtim.hpp"

namespace valle
{
    constexpr HRTIMTimerADCTriggerID adc_trigger_to_hrtim_trigger(ADCInjectGroupTriggerSource trigger)
    {
        switch (trigger)
        {
            case ADCInjectGroupTriggerSource::kExtHrtimTRG1:
                return HRTIMTimerADCTriggerID::kTrig1;
            case ADCInjectGroupTriggerSource::kExtHrtimTRG2:
                return HRTIMTimerADCTriggerID::kTrig2;
            case ADCInjectGroupTriggerSource::kExtHrtimTRG3:
                return HRTIMTimerADCTriggerID::kTrig3;
            case ADCInjectGroupTriggerSource::kExtHrtimTRG4:
                return HRTIMTimerADCTriggerID::kTrig4;
            case ADCInjectGroupTriggerSource::kExtHrtimTRG5:
                return HRTIMTimerADCTriggerID::kTrig5;
            case ADCInjectGroupTriggerSource::kExtHrtimTRG6:
                return HRTIMTimerADCTriggerID::kTrig6;
            case ADCInjectGroupTriggerSource::kExtHrtimTRG7:
                return HRTIMTimerADCTriggerID::kTrig7;
            case ADCInjectGroupTriggerSource::kExtHrtimTRG8:
                return HRTIMTimerADCTriggerID::kTrig8;
            case ADCInjectGroupTriggerSource::kExtHrtimTRG9:
                return HRTIMTimerADCTriggerID::kTrig9;
            case ADCInjectGroupTriggerSource::kExtHrtimTRG10:
                return HRTIMTimerADCTriggerID::kTrig10;
            default:
                return HRTIMTimerADCTriggerID::kTrig1;  // Default to kTrig1 for unsupported triggers
        }
    }

    constexpr HRTIMTimerADCTriggerID adc_trigger_to_hrtim_trigger(ADCRegularGroupTriggerSource trigger)
    {
        switch (trigger)
        {
            case ADCRegularGroupTriggerSource::kExtHrtimTRG1:
                return HRTIMTimerADCTriggerID::kTrig1;
            case ADCRegularGroupTriggerSource::kExtHrtimTRG2:
                return HRTIMTimerADCTriggerID::kTrig2;
            case ADCRegularGroupTriggerSource::kExtHrtimTRG3:
                return HRTIMTimerADCTriggerID::kTrig3;
            case ADCRegularGroupTriggerSource::kExtHrtimTRG4:
                return HRTIMTimerADCTriggerID::kTrig4;
            case ADCRegularGroupTriggerSource::kExtHrtimTRG5:
                return HRTIMTimerADCTriggerID::kTrig5;
            case ADCRegularGroupTriggerSource::kExtHrtimTRG6:
                return HRTIMTimerADCTriggerID::kTrig6;
            case ADCRegularGroupTriggerSource::kExtHrtimTRG7:
                return HRTIMTimerADCTriggerID::kTrig7;
            case ADCRegularGroupTriggerSource::kExtHrtimTRG8:
                return HRTIMTimerADCTriggerID::kTrig8;
            case ADCRegularGroupTriggerSource::kExtHrtimTRG9:
                return HRTIMTimerADCTriggerID::kTrig9;
            case ADCRegularGroupTriggerSource::kExtHrtimTRG10:
                return HRTIMTimerADCTriggerID::kTrig10;
            default:
                return HRTIMTimerADCTriggerID::kTrig1;  // Default to kTrig1 for unsupported triggers
        }
    }

    constexpr ADCInjectGroupTriggerSource hrtim_trigger_to_adc_inject_trigger(HRTIMTimerADCTriggerID trigger)
    {
        switch (trigger)
        {
            case HRTIMTimerADCTriggerID::kTrig1:
                return ADCInjectGroupTriggerSource::kExtHrtimTRG1;
            case HRTIMTimerADCTriggerID::kTrig2:
                return ADCInjectGroupTriggerSource::kExtHrtimTRG2;
            case HRTIMTimerADCTriggerID::kTrig3:
                return ADCInjectGroupTriggerSource::kExtHrtimTRG3;
            case HRTIMTimerADCTriggerID::kTrig4:
                return ADCInjectGroupTriggerSource::kExtHrtimTRG4;
            case HRTIMTimerADCTriggerID::kTrig5:
                return ADCInjectGroupTriggerSource::kExtHrtimTRG5;
            case HRTIMTimerADCTriggerID::kTrig6:
                return ADCInjectGroupTriggerSource::kExtHrtimTRG6;
            case HRTIMTimerADCTriggerID::kTrig7:
                return ADCInjectGroupTriggerSource::kExtHrtimTRG7;
            case HRTIMTimerADCTriggerID::kTrig8:
                return ADCInjectGroupTriggerSource::kExtHrtimTRG8;
            case HRTIMTimerADCTriggerID::kTrig9:
                return ADCInjectGroupTriggerSource::kExtHrtimTRG9;
            case HRTIMTimerADCTriggerID::kTrig10:
                return ADCInjectGroupTriggerSource::kExtHrtimTRG10;
            default:
                return ADCInjectGroupTriggerSource::kExtHrtimTRG1;  // Default to kExtHrtimTRG1 for unsupported triggers
        }
    }

    constexpr ADCRegularGroupTriggerSource hrtim_trigger_to_adc_regular_trigger(HRTIMTimerADCTriggerID trigger)
    {
        switch (trigger)
        {
            case HRTIMTimerADCTriggerID::kTrig1:
                return ADCRegularGroupTriggerSource::kExtHrtimTRG1;
            case HRTIMTimerADCTriggerID::kTrig2:
                return ADCRegularGroupTriggerSource::kExtHrtimTRG2;
            case HRTIMTimerADCTriggerID::kTrig3:
                return ADCRegularGroupTriggerSource::kExtHrtimTRG3;
            case HRTIMTimerADCTriggerID::kTrig4:
                return ADCRegularGroupTriggerSource::kExtHrtimTRG4;
            case HRTIMTimerADCTriggerID::kTrig5:
                return ADCRegularGroupTriggerSource::kExtHrtimTRG5;
            case HRTIMTimerADCTriggerID::kTrig6:
                return ADCRegularGroupTriggerSource::kExtHrtimTRG6;
            case HRTIMTimerADCTriggerID::kTrig7:
                return ADCRegularGroupTriggerSource::kExtHrtimTRG7;
            case HRTIMTimerADCTriggerID::kTrig8:
                return ADCRegularGroupTriggerSource::kExtHrtimTRG8;
            case HRTIMTimerADCTriggerID::kTrig9:
                return ADCRegularGroupTriggerSource::kExtHrtimTRG9;
            case HRTIMTimerADCTriggerID::kTrig10:
                return ADCRegularGroupTriggerSource::kExtHrtimTRG10;
            default:
                return ADCRegularGroupTriggerSource::kExtHrtimTRG1;  // Default to kExtHrtimTRG1 for unsupported
                                                                     // triggers
        }
    }

}  // namespace valle