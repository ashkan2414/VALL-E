#pragma once

#include "valle/app/platform/uart_logger_config.hpp"
#include "valle/base/system_build/config_base.hpp"
#include "valle/base/system_build/traits.hpp"
#include "valle/platform/drivers/adc/analog_sensor.hpp"
#include "valle/platform/drivers/core_system.hpp"
#include "valle/platform/drivers/uart/logger.hpp"

namespace valle::app
{
    // ADC Channel
    constexpr platform::ADCControllerID kTestADCControllerID = 1;
    constexpr platform::ADCChannelID    kTestADCChannelID    = platform::ADCChannelID::kChannel1;
    constexpr bool                      kTestADCUseInject    = false;

    using TestADCDMAChannelT = platform::DMA1Channel2Device;
    struct ADCControllerCTConfig : platform::ADCControllerCTDefaultConfig
    {
        using DMAChannelT = TestADCDMAChannelT;
    };

}  // namespace valle::app

// Bind compile-time configurations
VALLE_DEFINE_ADC_CONTROLLER_CT_CONFIG(valle::app::kTestADCControllerID, valle::app::ADCControllerCTConfig{});

namespace valle
{
    namespace app
    {

        // ============================================================================
        // Drivers
        // ============================================================================
        using TestADCChannelT =
            std::conditional_t<kTestADCUseInject,
                               platform::ADCInjectChannelRank1Device<kTestADCControllerID, kTestADCChannelID>,
                               platform::ADCRegularChannelRank1Device<kTestADCControllerID, kTestADCChannelID>>;
        using TestADCConverterT = platform::ADCVoltageConverter<IdentityConverter<float>>;
        using TestADCDriverT    = platform::ADCAnalogSensorDriver<TestADCChannelT, TestADCConverterT>;

        // Declare Main Driver List
        using MainDriversT = TypeList<platform::CoreSystemDriver, UARTLoggerT, TestADCDriverT>;

        // ============================================================================
        // Root Driver
        // ============================================================================
        using RootDevicesT = system_build::RootDevicesFromDriverList<MainDriversT>;
        struct RootDriver : PackedDriverBase<RootDevicesT>
        {
            using BaseT = PackedDriverBase<RootDevicesT>;
            using BaseT::BaseT;

            VALLE_DEFINE_PACKED_DEVICE_DRIVER_ACCESSOR(adc1, platform::ADC1ControllerDevice);
        };

        // ============================================================================
        // Drivers Container
        // ============================================================================
        struct Drivers
        {
            using DriversT = typename TypeListAddFront<RootDriver, MainDriversT>::type;

            RootDriver                 root;
            platform::CoreSystemDriver core;
            UARTLoggerT                uart_logger;
            TestADCDriverT             test_adc;
        };

    }  // namespace app

    struct AppSystemBuildConfig : SystemBuildConfigBase<app::Drivers>
    {
    };

}  // namespace valle
