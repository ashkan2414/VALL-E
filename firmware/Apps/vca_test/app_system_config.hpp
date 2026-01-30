#pragma once

#include "Valle/Device/device.hpp"
#include "Valle/Drivers/uart_logger.hpp"
#include "Valle/Modules/acs724.hpp"
#include "Valle/Modules/ldc1612.hpp"
#include "Valle/Modules/vca.hpp"
#include "Valle/System/config.hpp"

namespace valle::app
{
    constexpr UARTControllerID kLoggerUARTID = UARTControllerID::kLPUART1;
    struct UARTControllerCTConfig : UARTControllerCTConfigDefaults<kLoggerUARTID>
    {
        using DMAChannelTxT = DMA1Channel2Device;
    };
    static constexpr UARTControllerCTConfig kLoggerUARTCTConfig{};

    constexpr ADCControllerID kCurrentSensorADCID        = 1;
    constexpr ADCChannelID    kCurrentSensorADCChannelId = ADCChannelID::kChannel1;
    struct ADCControllerCTConfig : ADCControllerCTConfigDefaults
    {
        using DMAChannelT = DMA1Channel1Device;
    };
    constexpr auto kCurrentSensorADCConfig = ADCControllerCTConfig{};

    constexpr I2CControllerID kPositionSensorI2CID             = 1;
    constexpr uint16_t        kPositionSensorI2CAddress        = 0x2A;
    constexpr bool            kPositionSensorI2CAddressIs10Bit = false;

    struct I2CControllerCTConfig : I2CControllerCTConfigDefaults
    {
        using DMAChannelRxT = DMA2Channel1Device;
        using DMAChannelTxT = DMA2Channel2Device;
    };
    constexpr auto kPositionSensorI2CConfig = I2CControllerCTConfig{};

}  // namespace valle::app

VALLE_DEFINE_UART_CONTROLLER_CT_CONFIG(valle::app::kLoggerUARTID, valle::app::kLoggerUARTCTConfig);
VALLE_DEFINE_ADC_CONTROLLER_CT_CONFIG(valle::app::kCurrentSensorADCID, valle::app::kCurrentSensorADCConfig);
VALLE_DEFINE_I2C_CONTROLLER_CT_CONFIG(valle::app::kPositionSensorI2CID, valle::app::kPositionSensorI2CConfig);

namespace valle::app
{

    // ============================================================================
    // Device Configurations
    // ============================================================================
    using LoggerUARTControllerT = UARTControllerDevice<valle::app::kLoggerUARTID>;
    using UARTLoggerT           = UARTLogger<LoggerUARTControllerT>;

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
        using DriversT = TypeList<UARTLoggerT, CurrentSensorT, VCAControllerT, PositionSensorT>;

        UARTLoggerT     uart_logger;
        CurrentSensorT  current_sensor;
        VCAControllerT  vca;
        PositionSensorT position_sensor;
    };

    struct Devices
    {
        using DevicesT = TypeList<ADCControllerDevice<1>,
                                  HRTIMControllerDevice<1>,
                                  I2CCommandBufferDevice<1>,
                                  DMAControllerDevice<1>,
                                  DMAControllerDevice<2>,
                                  GPIOPortADevice>;

        [[no_unique_address]] DeviceRef<ADCControllerDevice<1>>    adc1;
        [[no_unique_address]] DeviceRef<HRTIMControllerDevice<1>>  hrtim1;
        [[no_unique_address]] DeviceRef<I2CCommandBufferDevice<1>> i2c1;
        [[no_unique_address]] DeviceRef<DMAControllerDevice<1>>    dma1;
        [[no_unique_address]] DeviceRef<DMAControllerDevice<2>>    dma2;
        [[no_unique_address]] DeviceRef<GPIOPortADevice>           gpioa;
    };

}  // namespace valle::app

namespace valle::system
{
    struct Config : ConfigBase<app::Drivers, app::Devices>
    {
    };

}  // namespace valle::system