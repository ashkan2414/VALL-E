#pragma once

#include "valle/platform/hardware/adc.hpp"
#include "valle/platform/hardware/hrtim.hpp"

namespace valle::platform
{
    constexpr HrtimTimerAdcTriggerId adc_trigger_to_hrtim_trigger(AdcInjectGroupTriggerSource trigger)
    {
        switch (trigger)
        {
            case AdcInjectGroupTriggerSource::kExtHrtimTRG1:
                return HrtimTimerAdcTriggerId::kTrig1;
            case AdcInjectGroupTriggerSource::kExtHrtimTRG2:
                return HrtimTimerAdcTriggerId::kTrig2;
            case AdcInjectGroupTriggerSource::kExtHrtimTRG3:
                return HrtimTimerAdcTriggerId::kTrig3;
            case AdcInjectGroupTriggerSource::kExtHrtimTRG4:
                return HrtimTimerAdcTriggerId::kTrig4;
            case AdcInjectGroupTriggerSource::kExtHrtimTRG5:
                return HrtimTimerAdcTriggerId::kTrig5;
            case AdcInjectGroupTriggerSource::kExtHrtimTRG6:
                return HrtimTimerAdcTriggerId::kTrig6;
            case AdcInjectGroupTriggerSource::kExtHrtimTRG7:
                return HrtimTimerAdcTriggerId::kTrig7;
            case AdcInjectGroupTriggerSource::kExtHrtimTRG8:
                return HrtimTimerAdcTriggerId::kTrig8;
            case AdcInjectGroupTriggerSource::kExtHrtimTRG9:
                return HrtimTimerAdcTriggerId::kTrig9;
            case AdcInjectGroupTriggerSource::kExtHrtimTRG10:
                return HrtimTimerAdcTriggerId::kTrig10;
            default:
                return HrtimTimerAdcTriggerId::kTrig1;  // Default to kTrig1 for unsupported triggers
        }
    }

    constexpr HrtimTimerAdcTriggerId adc_trigger_to_hrtim_trigger(AdcRegularGroupTriggerSource trigger)
    {
        switch (trigger)
        {
            case AdcRegularGroupTriggerSource::kExtHrtimTRG1:
                return HrtimTimerAdcTriggerId::kTrig1;
            case AdcRegularGroupTriggerSource::kExtHrtimTRG2:
                return HrtimTimerAdcTriggerId::kTrig2;
            case AdcRegularGroupTriggerSource::kExtHrtimTRG3:
                return HrtimTimerAdcTriggerId::kTrig3;
            case AdcRegularGroupTriggerSource::kExtHrtimTRG4:
                return HrtimTimerAdcTriggerId::kTrig4;
            case AdcRegularGroupTriggerSource::kExtHrtimTRG5:
                return HrtimTimerAdcTriggerId::kTrig5;
            case AdcRegularGroupTriggerSource::kExtHrtimTRG6:
                return HrtimTimerAdcTriggerId::kTrig6;
            case AdcRegularGroupTriggerSource::kExtHrtimTRG7:
                return HrtimTimerAdcTriggerId::kTrig7;
            case AdcRegularGroupTriggerSource::kExtHrtimTRG8:
                return HrtimTimerAdcTriggerId::kTrig8;
            case AdcRegularGroupTriggerSource::kExtHrtimTRG9:
                return HrtimTimerAdcTriggerId::kTrig9;
            case AdcRegularGroupTriggerSource::kExtHrtimTRG10:
                return HrtimTimerAdcTriggerId::kTrig10;
            default:
                return HrtimTimerAdcTriggerId::kTrig1;  // Default to kTrig1 for unsupported triggers
        }
    }

    constexpr AdcInjectGroupTriggerSource hrtim_trigger_to_adc_inject_trigger(HrtimTimerAdcTriggerId trigger)
    {
        switch (trigger)
        {
            case HrtimTimerAdcTriggerId::kTrig1:
                return AdcInjectGroupTriggerSource::kExtHrtimTRG1;
            case HrtimTimerAdcTriggerId::kTrig2:
                return AdcInjectGroupTriggerSource::kExtHrtimTRG2;
            case HrtimTimerAdcTriggerId::kTrig3:
                return AdcInjectGroupTriggerSource::kExtHrtimTRG3;
            case HrtimTimerAdcTriggerId::kTrig4:
                return AdcInjectGroupTriggerSource::kExtHrtimTRG4;
            case HrtimTimerAdcTriggerId::kTrig5:
                return AdcInjectGroupTriggerSource::kExtHrtimTRG5;
            case HrtimTimerAdcTriggerId::kTrig6:
                return AdcInjectGroupTriggerSource::kExtHrtimTRG6;
            case HrtimTimerAdcTriggerId::kTrig7:
                return AdcInjectGroupTriggerSource::kExtHrtimTRG7;
            case HrtimTimerAdcTriggerId::kTrig8:
                return AdcInjectGroupTriggerSource::kExtHrtimTRG8;
            case HrtimTimerAdcTriggerId::kTrig9:
                return AdcInjectGroupTriggerSource::kExtHrtimTRG9;
            case HrtimTimerAdcTriggerId::kTrig10:
                return AdcInjectGroupTriggerSource::kExtHrtimTRG10;
            default:
                return AdcInjectGroupTriggerSource::kExtHrtimTRG1;  // Default to kExtHrtimTRG1 for unsupported triggers
        }
    }

    constexpr AdcRegularGroupTriggerSource hrtim_trigger_to_adc_regular_trigger(HrtimTimerAdcTriggerId trigger)
    {
        switch (trigger)
        {
            case HrtimTimerAdcTriggerId::kTrig1:
                return AdcRegularGroupTriggerSource::kExtHrtimTRG1;
            case HrtimTimerAdcTriggerId::kTrig2:
                return AdcRegularGroupTriggerSource::kExtHrtimTRG2;
            case HrtimTimerAdcTriggerId::kTrig3:
                return AdcRegularGroupTriggerSource::kExtHrtimTRG3;
            case HrtimTimerAdcTriggerId::kTrig4:
                return AdcRegularGroupTriggerSource::kExtHrtimTRG4;
            case HrtimTimerAdcTriggerId::kTrig5:
                return AdcRegularGroupTriggerSource::kExtHrtimTRG5;
            case HrtimTimerAdcTriggerId::kTrig6:
                return AdcRegularGroupTriggerSource::kExtHrtimTRG6;
            case HrtimTimerAdcTriggerId::kTrig7:
                return AdcRegularGroupTriggerSource::kExtHrtimTRG7;
            case HrtimTimerAdcTriggerId::kTrig8:
                return AdcRegularGroupTriggerSource::kExtHrtimTRG8;
            case HrtimTimerAdcTriggerId::kTrig9:
                return AdcRegularGroupTriggerSource::kExtHrtimTRG9;
            case HrtimTimerAdcTriggerId::kTrig10:
                return AdcRegularGroupTriggerSource::kExtHrtimTRG10;
            default:
                return AdcRegularGroupTriggerSource::kExtHrtimTRG1;  // Default to kExtHrtimTRG1 for unsupported
                                                                     // triggers
        }
    }

}  // namespace valle::platform