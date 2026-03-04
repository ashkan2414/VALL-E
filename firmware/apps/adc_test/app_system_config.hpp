#pragma once

#include "valle/core/device/device.hpp"
#include "valle/core/system/config.hpp"
#include "valle/platform/drivers/uart/logger.hpp"
#include "valle/platform/modules/acs724.hpp"
#include "valle/platform/modules/ldc1612.hpp"
#include "valle/platform/modules/vca.hpp"

namespace valle::app
{
    // Logger
    constexpr UARTControllerID kLoggerUARTControllerID = UARTControllerID::kLPUART1;
    struct UARTControllerCTConfig : UARTControllerCTConfigDefaults<kLoggerUARTControllerID>
    {
        using DMAChannelTxT = DMA1Channel1Device;
    };

    // ADC Channel
    constexpr ADCControllerID kTestADCControllerID = 1;
    constexpr ADCChannelID    kTestADCChannelID    = ADCChannelID::kChannel1;
    constexpr bool            kTestADCUseInject    = false;

    using TestADCDMAChannelT = DMA2Channel1Device;
    struct ADCControllerCTConfig : ADCControllerCTConfigDefaults
    {
        using DMAChannelT = TestADCDMAChannelT;
    };

}  // namespace valle::app

// Bind compile-time configurations
VALLE_DEFINE_UART_CONTROLLER_CT_CONFIG(app::kLoggerUARTControllerID, app::UARTControllerCTConfig{});
VALLE_DEFINE_ADC_CONTROLLER_CT_CONFIG(app::kTestADCControllerID, app::ADCControllerCTConfig{});

namespace valle::app
{

    // ============================================================================
    // Driver Configurations
    // ============================================================================
    using UARTLoggerT = UARTLogger<UARTControllerDevice<kLoggerUARTControllerID>>;

    using TestADCChannelT   = std::conditional_t<kTestADCUseInject,
                                                 ADCInjectChannelRank1Device<kTestADCControllerID, kTestADCChannelID>,
                                                 ADCRegularChannelRank1Device<kTestADCControllerID, kTestADCChannelID>>;
    using TestADCConverterT = ADCVoltageConverter<IdentityConverter<float>>;
    using TestADCDriverT    = ADCAnalogSensorDriver<TestADCChannelT, TestADCConverterT>;

    struct Drivers
    {
        using DriversT = TypeList<UARTLoggerT, TestADCDriverT>;

        UARTLoggerT    uart_logger;
        TestADCDriverT test_adc;
    };

    struct Devices
    {
        using DevicesT = TypeList<DMAMux1ControllerDevice,
                                  DMA1ControllerDevice,
                                  DMA2ControllerDevice,
                                  GPIOPortADevice,
                                  ADC12ClockDevice,
                                  ADC1ControllerDevice>;

        [[no_unique_address]] DeviceRef<DMAMux1ControllerDevice> dmamux1;
        [[no_unique_address]] DeviceRef<DMA1ControllerDevice>    dma1;
        [[no_unique_address]] DeviceRef<DMA2ControllerDevice>    dma2;
        [[no_unique_address]] DeviceRef<GPIOPortADevice>         gpioa;
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