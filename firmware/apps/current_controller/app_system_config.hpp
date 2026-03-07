#pragma once

#include "valle/core/device/device.hpp"
#include "valle/core/system/config.hpp"
#include "valle/platform/drivers/gpio/digital_out.hpp"
#include "valle/platform/drivers/uart/logger.hpp"
#include "valle/platform/modules/acs724.hpp"
#include "valle/platform/modules/vca.hpp"

namespace valle::app
{
    // Logger
    constexpr UARTControllerID kLoggerUARTControllerID = UARTControllerID::kLPUART1;
    struct UARTControllerCTConfig : UARTControllerCTConfigDefaults<kLoggerUARTControllerID>
    {
        using DMAChannelTxT = DMA1Channel1Device;
    };

    // HRTIM Controller
    constexpr HRTIMControllerID kVCAPWMHRTIMControllerID = 1;
    struct HRTIMControllerCTConfig : HRTIMControllerCTConfigDefaults
    {
    };

    // HRTIM Timer
    constexpr HRTIMTimerID kVCAPWMHRTIMTimerID = HRTIMTimerID::kA;
    struct HRTIMTimerCTConfig : HRTIMTimerCTConfigDefaults
    {
        using Output1PinT = GPIOPinA8Device;
        using Output2PinT = GPIOPinA9Device;
    };

    constexpr HRTIMTimerADCTriggerID kVCAPWMHRTIMTimerCurrentSensorADCTriggerID = HRTIMTimerADCTriggerID::kTrig1;

    // Current Sensor ADC Channel (pin PA0, A0)
    constexpr ADCControllerID kCurrentSensorADCID        = 1;
    constexpr ADCChannelID    kCurrentSensorADCChannelId = ADCChannelID::kChannel1;

    using CurrentSensorADCDMAChannelT = DMA1Channel2Device;
    struct ADCControllerCTConfig : ADCControllerCTConfigDefaults
    {
        using DMAChannelT = CurrentSensorADCDMAChannelT;
    };

}  // namespace valle::app

// Bind compile-time configurations
VALLE_DEFINE_UART_CONTROLLER_CT_CONFIG(app::kLoggerUARTControllerID, app::UARTControllerCTConfig{});
VALLE_DEFINE_HRTIM_CONTROLLER_CT_CONFIG(app::kVCAPWMHRTIMControllerID, app::HRTIMControllerCTConfig{});
VALLE_DEFINE_HRTIM_TIMER_CT_CONFIG(app::kVCAPWMHRTIMControllerID, app::kVCAPWMHRTIMTimerID, app::HRTIMTimerCTConfig{});
VALLE_DEFINE_ADC_CONTROLLER_CT_CONFIG(app::kCurrentSensorADCID, app::ADCControllerCTConfig{});

namespace valle::app
{

    // ============================================================================
    // Driver Configurations
    // ============================================================================
    using UARTLoggerT = UARTLogger<UARTControllerDevice<kLoggerUARTControllerID>>;

    using VCAPWMHRTIMTimerDeviceT = HRTIMTimerDevice<kVCAPWMHRTIMControllerID, kVCAPWMHRTIMTimerID>;

    using VCAControllerSystemControllerT       = VCAClosedLoopCurrentFeedbackController<float>;
    using VCAControllerSystemControllerConfigT = typename VCAControllerSystemControllerT::ConfigT;

    using VCAControllerT       = VCAControllerHRTIMModule<VCAPWMHRTIMTimerDeviceT, VCAControllerSystemControllerT>;
    using VCAControllerConfigT = typename VCAControllerT::ConfigT;

    // Set to false to use regular channel instead (not recommended, less accurate timing)
    static constexpr bool kCurrentSensorUseInject = true;
    using CurrentSensorADCChannelT =
        std::conditional_t<kCurrentSensorUseInject,
                           ADCInjectChannelRank1Device<kCurrentSensorADCID, kCurrentSensorADCChannelId>,
                           ADCRegularChannelRank1Device<kCurrentSensorADCID, kCurrentSensorADCChannelId>>;
    using CurrentSensorADCControllerT = CurrentSensorADCChannelT::ChannelT::ControllerT;
    using CurrentSensorT              = ACS724Module<CurrentSensorADCChannelT, ACS724Model::k2P5ABi>;

    using TestGPIODriverT = GPIODigitalOutDriver<GPIOPinB6Device>;

    struct Drivers
    {
        using DriversT = TypeList<UARTLoggerT, VCAControllerT, CurrentSensorT, TestGPIODriverT>;

        UARTLoggerT     uart_logger;
        VCAControllerT  vca_controller;
        CurrentSensorT  current_sensor;
        TestGPIODriverT test_gpio;
    };

    struct Devices
    {
        using DevicesT = TypeList<GPIOPortADevice,
                                  GPIOPortBDevice,
                                  DMAMux1ControllerDevice,
                                  DMA1ControllerDevice,
                                  HRTIM1ControllerDevice,
                                  ADC12ClockDevice,
                                  ADC1ControllerDevice>;

        [[no_unique_address]] DeviceRef<GPIOPortADevice>         gpioa;
        [[no_unique_address]] DeviceRef<GPIOPortBDevice>         gpiob;
        [[no_unique_address]] DeviceRef<DMAMux1ControllerDevice> dmamux1;
        [[no_unique_address]] DeviceRef<DMA1ControllerDevice>    dma1;
        [[no_unique_address]] DeviceRef<HRTIM1ControllerDevice>  hrtim1;
        [[no_unique_address]] DeviceRef<ADC12ClockDevice>        adc12_clk;
        [[no_unique_address]] DeviceRef<ADC1ControllerDevice>    adc1;
    };

}  // namespace valle::app

namespace valle::system
{
    struct Config : ConfigBase<app::Drivers, app::Devices>
    {
    };

}  // namespace valle::system