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
        using DMAChannelTxT = DMA1Channel1Device;
    };
    static constexpr UARTControllerCTConfig kLoggerUARTCTConfig{};

    constexpr ADCControllerID kTestADCControllerID = 1;
    constexpr ADCChannelID    kTestADCChannelId    = ADCChannelID::kChannel1;
    constexpr bool            kTestADCUseInject    = false;
    using TestADCDMAChannelT                       = DMA2Channel1Device;
    struct ADCControllerCTConfig : ADCControllerCTConfigDefaults
    {
        using DMAChannelT = TestADCDMAChannelT;
    };
    constexpr auto kTestADCCTConfig = ADCControllerCTConfig{};

}  // namespace valle::app

VALLE_DEFINE_UART_CONTROLLER_CT_CONFIG(valle::app::kLoggerUARTID, valle::app::kLoggerUARTCTConfig);
VALLE_DEFINE_ADC_CONTROLLER_CT_CONFIG(valle::app::kTestADCControllerID, valle::app::kTestADCCTConfig);

namespace valle::app
{

    // ============================================================================
    // Device Configurations
    // ============================================================================
    using LoggerUARTControllerT = UARTControllerDevice<valle::app::kLoggerUARTID>;
    using UARTLoggerT           = UARTLogger<LoggerUARTControllerT>;

    using TestADCChannelT    = std::conditional_t<kTestADCUseInject,
                                                  ADCInjectChannelDevice1<kTestADCControllerID, kTestADCChannelId>,
                                                  ADCRegularChannelDevice1<kTestADCControllerID, kTestADCChannelId>>;
    using TestADCControllerT = TestADCChannelT::ChannelT::ControllerT;
    using TestADCConverterT  = ADCVoltageConverter<IdentityConverter<float>>;
    using TestADCDriverT     = ADCSensorDriver<TestADCChannelT, TestADCConverterT>;

    struct Drivers
    {
        using DriversT = TypeList<UARTLoggerT, TestADCDriverT>;

        UARTLoggerT    uart_logger;
        TestADCDriverT test_adc;
    };

    struct Devices
    {
        using DevicesT = TypeList<ADC12ClockDevice,
                                  TestADCControllerT,
                                  DMAMux1ControllerDevice,
                                  DMA1ControllerDevice,
                                  DMA2ControllerDevice,
                                  GPIOPortADevice>;

        [[no_unique_address]] DeviceRef<ADC12ClockDevice>        adc12_clk;
        [[no_unique_address]] DeviceRef<TestADCControllerT>      adc1;
        [[no_unique_address]] DeviceRef<DMAMux1ControllerDevice> dmamux1;
        [[no_unique_address]] DeviceRef<DMA1ControllerDevice>    dma1;
        [[no_unique_address]] DeviceRef<DMA2ControllerDevice>    dma2;
        [[no_unique_address]] DeviceRef<GPIOPortADevice>         gpioa;
    };

}  // namespace valle::app

namespace valle::system
{
    struct Config : ConfigBase<app::Drivers, app::Devices>
    {
    };

}  // namespace valle::system