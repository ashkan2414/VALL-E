#pragma once

#include "valle/app/vca_current_controller_config.hpp"
#include "valle/base/system_build/config_base.hpp"
#include "valle/platform/drivers/core_system.hpp"
#include "valle/platform/drivers/gpio/digital_out.hpp"
#include "valle/platform/drivers/uart/logger.hpp"
#include "valle/platform/modules/acs724.hpp"
#include "valle/platform/modules/ldc1612.hpp"
#include "valle/platform/modules/vca.hpp"

namespace valle::app
{
    // Logger
    constexpr UARTControllerID kLoggerUARTControllerID = UARTControllerID::kLPUART1;
    struct UARTControllerCTConfig : UARTControllerCTDefaultConfig<kLoggerUARTControllerID>
    {
        using DMAChannelTxT = DMA1Channel1Device;
    };

    // Position Sensor I2C Controller Config
    constexpr I2CControllerID kPositionSensorI2CID             = 1;
    constexpr uint16_t        kPositionSensorI2CAddress        = 0x2A;
    constexpr bool            kPositionSensorI2CAddressIs10Bit = false;

    struct I2CControllerCTConfig : I2CControllerCTDefaultConfig
    {
        using DMAChannelRxT = DMA2Channel1Device;
        using DMAChannelTxT = DMA2Channel2Device;
    };

}  // namespace valle::app

// Bind compile-time configurations
VALLE_DEFINE_UART_CONTROLLER_CT_CONFIG(app::kLoggerUARTControllerID, app::UARTControllerCTConfig{});
VALLE_DEFINE_I2C_CONTROLLER_CT_CONFIG(app::kPositionSensorI2CID, app::I2CControllerCTConfig{});

namespace valle
{
    namespace app
    {
        // ============================================================================
        // Drivers
        // ============================================================================
        using UARTLoggerT = UARTLogger<UARTControllerDevice<kLoggerUARTControllerID>>;

        using PositionSensorI2CControllerT  = I2CCommandBufferDevice<kPositionSensorI2CID>;
        using PositionSensorI2CSlaveDeviceT = I2CCommandBufferSlaveDevice<kPositionSensorI2CID,
                                                                          kPositionSensorI2CAddress,
                                                                          kPositionSensorI2CAddressIs10Bit>;
        using PositionSensorT               = LDC161XSensorModule<PositionSensorI2CSlaveDeviceT, 1>;

        using TestGPIODriverT = GPIODigitalOutDriver<GPIOPinB6Device>;

        // Declare Main Driver List
        using MainDriversT =
            TypeList<CoreSystemDriver, UARTLoggerT, VCAControllerT, CurrentSensorT, PositionSensorT, TestGPIODriverT>;

        // ============================================================================
        // Root Driver
        // ============================================================================
        using RootDevicesT = RootDevicesFromDriverList<MainDriversT>;
        struct RootDriver : PackedDriverBase<RootDevicesT>
        {
            using BaseT = PackedDriverBase<RootDevicesT>;
            using BaseT::BaseT;

            VALLE_DEFINE_PACKED_DEVICE_DRIVER_ACCESSOR(adc1, ADC1ControllerDevice);
            VALLE_DEFINE_PACKED_DEVICE_DRIVER_ACCESSOR(i2c1, I2C1CommandBufferDevice<>);
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
            PositionSensorT  position_sensor;
            TestGPIODriverT  test_gpio;
        };

    }  // namespace app

    struct AppSystemBuildConfig : SystemBuildConfigBase<app::Drivers>
    {
    };

}  // namespace valle
