#pragma once

#include "valle/app/platform/modules/acs724.hpp"
#include "valle/app/platform/modules/vca.hpp"
#include "valle/app/platform/uart_logger_config.hpp"
#include "valle/app/platform/vca_current_controller_config.hpp"
#include "valle/base/system_build/config_base.hpp"
#include "valle/base/system_build/traits.hpp"
#include "valle/platform/drivers/core_system.hpp"
#include "valle/platform/drivers/gpio/digital_out.hpp"
#include "valle/platform/drivers/uart/logger.hpp"

namespace valle
{
    namespace app
    {
        // ============================================================================
        // Drivers
        // ============================================================================
        using TestGPIODriverT = platform::GPIODigitalOutDriver<platform::GPIOPinB6Device>;

        // Declare Main Driver List
        using MainDriversT =
            TypeList<platform::CoreSystemDriver, UARTLoggerT, VCAControllerT, CurrentSensorT, TestGPIODriverT>;

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
            VCAControllerT             vca_controller;
            CurrentSensorT             current_sensor;
            TestGPIODriverT            test_gpio;
        };

    }  // namespace app

    struct AppSystemBuildConfig : SystemBuildConfigBase<app::Drivers>
    {
    };

}  // namespace valle
