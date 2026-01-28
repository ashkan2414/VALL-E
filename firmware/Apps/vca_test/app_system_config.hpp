#pragma once

#include "Valle/Device/device.hpp"
#include "Valle/Modules/acs724.hpp"
#include "Valle/Modules/ldc1612.hpp"
#include "Valle/Modules/vca.hpp"
#include "Valle/System/config.hpp"

namespace valle::app
{
    constexpr ADCControllerID kCurrentSensorADCID        = 1;
    constexpr ADCChannelID    kCurrentSensorADCChannelId = ADCChannelID::kChannel3;
    constexpr auto            kCurrentSensorADCConfig    = ADCControllerCTConfig<DMA1Channel1Device>{};

    constexpr I2CControllerID kPositionSensorI2CID = 1;
    constexpr auto     kPositionSensorI2CConfig    = I2CControllerCTConfig<DMA2Channel1Device, DMA2Channel2Device>{};
    constexpr uint16_t kPositionSensorI2CAddress   = 0x2A;
    constexpr bool     kPositionSensorI2CAddressIs10Bit = false;

}  // namespace valle::app

VALLE_DEFINE_ADC_CONTROLLER_CT_CONFIG(valle::app::kCurrentSensorADCID, valle::app::kCurrentSensorADCConfig);
VALLE_DEFINE_I2C_CONTROLLER_CT_CONFIG(valle::app::kPositionSensorI2CID, valle::app::kPositionSensorI2CConfig);

namespace valle::app
{

    // ============================================================================
    // Device Configurations
    // ============================================================================

    using CurrentSensorADCChannelT    = ADCInjectChannelDevice1<kCurrentSensorADCID, kCurrentSensorADCChannelId>;
    using CurrentSensorADCControllerT = CurrentSensorADCChannelT::ChannelT::ControllerT;
    using CurrentSensorT              = ACS724Module<CurrentSensorADCChannelT, ACS724Model::k2P5ABi>;

    using VCAControllerT = VCAControllerModule<HRTIM1TimerADevice>;

    using PositionSensorI2CControllerT = I2CCommandBufferDevice<kPositionSensorI2CID>;
    using PositionSensorI2CSlaveDeviceT =
        I2CCommandBufferSlaveDevice<kPositionSensorI2CID, kPositionSensorI2CAddress, kPositionSensorI2CAddressIs10Bit>;
    using PositionSensorT = LDC161XSensorDriver<PositionSensorI2CSlaveDeviceT, 1>;

    struct Drivers
    {
        CurrentSensorT  current_sensor;
        VCAControllerT  vca;
        PositionSensorT position_sensor;
    };

}  // namespace valle::app

namespace valle::system
{
    struct Config : ConfigBase<TypeList<app::CurrentSensorT, app::VCAControllerT, app::PositionSensorT>, app::Drivers>
    {
    };

}  // namespace valle::system