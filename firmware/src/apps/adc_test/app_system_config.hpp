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
    constexpr platform::AdcPeripheralId kTestAdcPeripheralId = platform::AdcPeripheralId::kAdc1;
    constexpr platform::AdcChannelId    kTestAdcChannelId    = platform::AdcChannelId::kChannel1;
    constexpr bool                      kTestAdcUseInject    = false;

    using TestAdcDmaChannelT = platform::Dma1Channel2Device;
    struct AdcControllerCTConfig : public platform::AdcControllerCTDefaultConfig
    {
        using DmaChannelT = TestAdcDmaChannelT;
    };

}  // namespace valle::app

// Bind compile-time configurations
VALLE_DEFINE_ADC_CONTROLLER_CT_CONFIG(valle::app::kTestAdcPeripheralId, valle::app::AdcControllerCTConfig{});

namespace valle
{
    namespace app
    {

        // ============================================================================
        // Drivers
        // ============================================================================
        using TestAdcChannelT =
            std::conditional_t<kTestAdcUseInject,
                               platform::AdcInjectChannelRank1Device<kTestAdcPeripheralId, kTestAdcChannelId>,
                               platform::AdcRegularChannelRank1Device<kTestAdcPeripheralId, kTestAdcChannelId>>;
        using TestAdcConverterT = platform::AdcVoltageConverter<IdentityConverter<float>>;
        using TestAdcDriverT    = platform::AdcAnalogSensorDriver<TestAdcChannelT, TestAdcConverterT>;

        // Declare Main Driver List
        using MainDriversT = TypeList<platform::CoreSystemDriver, UartLoggerT, TestAdcDriverT>;

        // ============================================================================
        // Root Driver
        // ============================================================================
        using RootDevicesT = system_build::RootDevicesFromDriverList<MainDriversT>;
        struct RootDriver : PackedDriverBase<RootDevicesT>
        {
            using BaseT = PackedDriverBase<RootDevicesT>;
            using BaseT::BaseT;

            VALLE_DEFINE_PACKED_DEVICE_DRIVER_ACCESSOR(adc1, platform::Adc1ControllerDevice);
        };

        // ============================================================================
        // Drivers Container
        // ============================================================================
        struct Drivers
        {
            using DriversT = typename TypeListAddFront<RootDriver, MainDriversT>::type;

            RootDriver                 root;
            platform::CoreSystemDriver core;
            UartLoggerT                uart_logger;
            TestAdcDriverT             test_adc;
        };

    }  // namespace app

    struct AppSystemBuildConfig : SystemBuildConfigBase<app::Drivers>
    {
    };

}  // namespace valle
