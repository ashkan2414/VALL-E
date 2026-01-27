#pragma once

#include "Valle/Device/device.hpp"
#include "Valle/Modules/acs724.hpp"
#include "Valle/Modules/vca.hpp"
#include "Valle/System/config.hpp"

namespace valle::app
{
    constexpr ADCControllerID kCurrentSensorADCID        = 1;
    constexpr ADCChannelID    kCurrentSensorADCChannelId = ADCChannelID::kChannel3;
    constexpr auto            kCurrentSensorADCConfig    = ADCControllerCTConfig<DMA1Channel1Device>{};

}  // namespace valle::app

VALLE_DEFINE_ADC_CONTROLLER_CT_CONFIG(valle::app::kCurrentSensorADCID, valle::app::kCurrentSensorADCConfig);

namespace valle::app
{

    // ============================================================================
    // Device Configurations
    // ============================================================================

    using CurrentSensorADCChannelT    = ADCInjectChannelDevice1<kCurrentSensorADCID, kCurrentSensorADCChannelId>;
    using CurrentSensorADCControllerT = CurrentSensorADCChannelT::ChannelT::ControllerT;
    using CurrentSensorT              = ACS724Module<CurrentSensorADCChannelT, ACS724Model::k2P5ABi>;

    using VCAControllerT = VCAControllerModule<HRTIM1TimerADevice>;

    struct Drivers
    {
        app::CurrentSensorT current_sensor;
        app::VCAControllerT vca;
    };

}  // namespace valle::app

namespace valle::system
{
    struct Config : ConfigBase<TypeList<app::CurrentSensorT, app::VCAControllerT>, app::Drivers>
    {
    };

}  // namespace valle::system