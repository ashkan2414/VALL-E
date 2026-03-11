#pragma once

#include "valle/app/vca_current_controller_config.hpp"
#include "valle/base/hardware_build/config_base.hpp"
#include "valle/platform/drivers/core_system.hpp"
#include "valle/platform/drivers/gpio/digital_out.hpp"
#include "valle/platform/drivers/uart/logger.hpp"
#include "valle/platform/modules/acs724.hpp"
#include "valle/platform/modules/vca.hpp"

namespace valle::app
{
    // Logger
    constexpr UARTControllerID kLoggerUARTControllerID = UARTControllerID::kLPUART1;
    struct UARTControllerCTConfig : UARTControllerCTDefaultConfig<kLoggerUARTControllerID>
    {
        using DMAChannelTxT = DMA1Channel1Device;
    };

}  // namespace valle::app

// Bind compile-time configurations
VALLE_DEFINE_UART_CONTROLLER_CT_CONFIG(app::kLoggerUARTControllerID, app::UARTControllerCTConfig{});

namespace valle
{
    namespace app
    {
        // ============================================================================
        // Drivers
        // ============================================================================
        using UARTLoggerT     = UARTLogger<UARTControllerDevice<kLoggerUARTControllerID>>;
        using TestGPIODriverT = GPIODigitalOutDriver<GPIOPinB6Device>;

        // Declare Main Driver List
        using MainDriversT = TypeList<CoreSystemDriver, UARTLoggerT, VCAControllerT, CurrentSensorT, TestGPIODriverT>;

        // ============================================================================
        // Root Driver
        // ============================================================================
        using RootDevicesT = RootDevicesFromDriverList<MainDriversT>;
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
            VCAControllerT   vca_controller;
            CurrentSensorT   current_sensor;
            TestGPIODriverT  test_gpio;
        };

    }  // namespace app

    struct AppHardwareBuildConfig : SystemConfigBase<app::Drivers>
    {
    };

}  // namespace valle
