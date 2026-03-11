#pragma once

#include "valle/core/device/device.hpp"
#include "valle/core/system/config.hpp"
#include "valle/platform/drivers/adc/analog_sensor.hpp"
#include "valle/platform/drivers/core_system.hpp"
#include "valle/platform/drivers/uart/logger.hpp"

namespace valle::app
{
    // Logger
    constexpr UARTControllerID kLoggerUARTControllerID = UARTControllerID::kLPUART1;
    struct UARTControllerCTConfig : UARTControllerCTDefaultConfig<kLoggerUARTControllerID>
    {
        using DMAChannelTxT = DMA1Channel1Device;
    };

    // ADC Channel
    constexpr ADCControllerID kTestADCControllerID = 1;
    constexpr ADCChannelID    kTestADCChannelID    = ADCChannelID::kChannel1;
    constexpr bool            kTestADCUseInject    = false;

    using TestADCDMAChannelT = DMA1Channel2Device;
    struct ADCControllerCTConfig : ADCControllerCTDefaultConfig
    {
        using DMAChannelT = TestADCDMAChannelT;
    };

}  // namespace valle::app

// Bind compile-time configurations
VALLE_DEFINE_UART_CONTROLLER_CT_CONFIG(app::kLoggerUARTControllerID, app::UARTControllerCTConfig{});
VALLE_DEFINE_ADC_CONTROLLER_CT_CONFIG(app::kTestADCControllerID, app::ADCControllerCTConfig{});

namespace valle
{
    namespace app
    {

        // ============================================================================
        // Drivers
        // ============================================================================
        using UARTLoggerT = UARTLogger<UARTControllerDevice<kLoggerUARTControllerID>>;

        using TestADCChannelT =
            std::conditional_t<kTestADCUseInject,
                               ADCInjectChannelRank1Device<kTestADCControllerID, kTestADCChannelID>,
                               ADCRegularChannelRank1Device<kTestADCControllerID, kTestADCChannelID>>;
        using TestADCConverterT = ADCVoltageConverter<IdentityConverter<float>>;
        using TestADCDriverT    = ADCAnalogSensorDriver<TestADCChannelT, TestADCConverterT>;

        // Declare Main Driver List
        using MainDriversT = TypeList<CoreSystemDriver, UARTLoggerT, TestADCDriverT>;

        // ============================================================================
        // Root Driver
        // ============================================================================
        using RootDevicesT = RootDevicesFromDrivers<MainDriversT>;
        struct RootDriver : PackedDriverBase<RootDevicesT>
        {
            using BaseT = PackedDriverBase<RootDevicesT>;
            using BaseT::BaseT;

            VALLE_DEFINE_PACKED_DEVICE_DRIVER_ACCESSOR(adc1, ADC1ControllerDevice);
        };

        // ============================================================================
        // Drivers Container
        // ============================================================================
        struct Drivers
        {
            using DriversT = typename TypeListAddFront<RootDriver, MainDriversT>::type;

            RootDriver       root;
            CoreSystemDriver core;
            UARTLoggerT      uart_logger;
            TestADCDriverT   test_adc;
        };

    }  // namespace app

    struct AppSystemConfig : SystemConfigBase<app::Drivers>
    {
    };

}  // namespace valle
