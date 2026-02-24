#pragma once

#include "Valle/Board/Device/device.hpp"
#include "Valle/Board/Drivers/uart_logger.hpp"
#include "Valle/Modules/acs724.hpp"
#include "Valle/Modules/ldc1612.hpp"
#include "Valle/Modules/vca.hpp"
#include "Valle/System/config.hpp"

namespace valle::app
{
    constexpr UARTControllerID kLoggerUARTID = UARTControllerID::kLPUART1;
    struct UARTControllerCTConfig : UARTControllerCTConfigDefaults<kLoggerUARTID>
    {
        using DMAChannelTxT = DMA1Channel1Device;
    };

    // Current Sensor ADC Config
    constexpr ADCControllerID kCurrentSensorADCID        = 3;
    constexpr ADCChannelID    kCurrentSensorADCChannelId = ADCChannelID::kChannel1;
    struct ADCControllerCTConfig : ADCControllerCTConfigDefaults
    {
    };

    // HRTIM PWM Config
    constexpr HRTIMControllerID kPWMHRTIMControllerID = 1;
    constexpr HRTIMTimerID      kPWMHRTIMTimerID      = HRTIMTimerID::kA;

    struct HRTIMControllerCTConfig : HRTIMControllerCTConfigDefaults
    {
    };

    struct HRTIMTimerCTConfig : HRTIMTimerCTConfigDefaults
    {
        using Output1PinT = HRTIMTimerDefaultOutput1PinDevice<kPWMHRTIMControllerID, kPWMHRTIMTimerID>;
        using Output2PinT = HRTIMTimerDefaultOutput2PinDevice<kPWMHRTIMControllerID, kPWMHRTIMTimerID>;
    };

    // Position Sensor I2C Controller Config
    constexpr I2CControllerID kPositionSensorI2CID             = 1;
    constexpr uint16_t        kPositionSensorI2CAddress        = 0x2A;
    constexpr bool            kPositionSensorI2CAddressIs10Bit = false;

    struct I2CControllerCTConfig : I2CControllerCTConfigDefaults
    {
        using DMAChannelRxT = DMA2Channel1Device;
        using DMAChannelTxT = DMA2Channel2Device;
    };

}  // namespace valle::app

VALLE_DEFINE_UART_CONTROLLER_CT_CONFIG(app::kLoggerUARTID, app::UARTControllerCTConfig{});
VALLE_DEFINE_ADC_CONTROLLER_CT_CONFIG(app::kCurrentSensorADCID, app::ADCControllerCTConfig{});
VALLE_DEFINE_HRTIM_CONTROLLER_CT_CONFIG(app::kPWMHRTIMControllerID, app::HRTIMControllerCTConfig{});
VALLE_DEFINE_HRTIM_TIMER_CT_CONFIG(app::kPWMHRTIMControllerID, app::kPWMHRTIMTimerID, app::HRTIMTimerCTConfig{});
VALLE_DEFINE_I2C_CONTROLLER_CT_CONFIG(app::kPositionSensorI2CID, app::I2CControllerCTConfig{});

namespace valle::app
{

    // ============================================================================
    // Device Configurations
    // ============================================================================
    using LoggerUARTControllerT = UARTControllerDevice<app::kLoggerUARTID>;
    using UARTLoggerT           = UARTLogger<LoggerUARTControllerT>;

    using CurrentSensorADCChannelT    = ADCInjectChannelRank1Device<kCurrentSensorADCID, kCurrentSensorADCChannelId>;
    using CurrentSensorADCControllerT = CurrentSensorADCChannelT::ChannelT::ControllerT;
    using CurrentSensorT              = ACS724Module<CurrentSensorADCChannelT, ACS724Model::k2P5ABi>;

    using VCAControllerT = VCAControllerModule<HRTIMTimerDevice<kPWMHRTIMControllerID, kPWMHRTIMTimerID>>;

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
        using DevicesT = TypeList<ADC345ClockDevice,
                                  ADC3ControllerDevice,
                                  HRTIM1ControllerDevice,
                                  I2C1CommandBufferDevice<>,
                                  DMAMux1ControllerDevice,
                                  DMA1ControllerDevice,
                                  DMA2ControllerDevice,
                                  GPIOPortADevice,
                                  GPIOPortBDevice>;

        [[no_unique_address]] DeviceRef<ADC345ClockDevice>         adc345_clk;
        [[no_unique_address]] DeviceRef<ADC3ControllerDevice>      adc3;
        [[no_unique_address]] DeviceRef<HRTIM1ControllerDevice>    hrtim1;
        [[no_unique_address]] DeviceRef<I2C1CommandBufferDevice<>> i2c1;
        [[no_unique_address]] DeviceRef<DMAMux1ControllerDevice>   dmamux1;
        [[no_unique_address]] DeviceRef<DMA1ControllerDevice>      dma1;
        [[no_unique_address]] DeviceRef<DMA2ControllerDevice>      dma2;
        [[no_unique_address]] DeviceRef<GPIOPortADevice>           gpioa;
        [[no_unique_address]] DeviceRef<GPIOPortBDevice>           gpiob;
    };

}  // namespace valle::app

namespace valle::system
{
    struct Config : ConfigBase<app::Drivers, app::Devices>
    {
    };

}  // namespace valle::system