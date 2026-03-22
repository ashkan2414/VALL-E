#pragma once

#include "valle/app/platform/modules/ldc161x.hpp"
#include "valle/app/platform/uart_logger_config.hpp"
#include "valle/base/system_build/config_base.hpp"
#include "valle/base/system_build/traits.hpp"
#include "valle/platform/drivers/core_system.hpp"
#include "valle/platform/drivers/uart/logger.hpp"

namespace valle::app
{
    // Position Sensor I2C Controller Config
    constexpr platform::I2CControllerID kPositionSensorI2CID             = 1;
    constexpr uint16_t                  kPositionSensorI2CAddress        = 0x2B;
    constexpr bool                      kPositionSensorI2CAddressIs10Bit = false;

    struct PositionSensorI2CControllerCTConfig : public platform::I2CControllerCTDefaultConfig
    {
        using DMAChannelRxT = platform::DMA1Channel3Device;
        using DMAChannelTxT = platform::DMA1Channel4Device;
        using SCLPinT       = platform::GPIOPinB8Device;
        using SDAPinT       = platform::GPIOPinB9Device;
    };
}  // namespace valle::app

VALLE_DEFINE_I2C_CONTROLLER_CT_CONFIG(valle::app::kPositionSensorI2CID,
                                      valle::app::PositionSensorI2CControllerCTConfig{});

namespace valle
{
    namespace app
    {
        // ============================================================================
        // Drivers
        // ============================================================================
        using PositionSensorI2CControllerT  = platform::I2CCommandBufferDevice<kPositionSensorI2CID>;
        using PositionSensorI2CSlaveDeviceT = platform::I2CCommandBufferSlaveDevice<kPositionSensorI2CID,
                                                                                    kPositionSensorI2CAddress,
                                                                                    kPositionSensorI2CAddressIs10Bit>;
        using PositionSensorModuleT         = platform::app::LDC161XSensorModule<PositionSensorI2CSlaveDeviceT, 2>;
        using PositionSensorModuleConfigT   = typename PositionSensorModuleT::ConfigT;
        using PositionSensorConfigT         = typename PositionSensorModuleT::SensorConfigT;

        // Declare Main Driver List
        using MainDriversT = TypeList<platform::CoreSystemDriver, UARTLoggerT, PositionSensorModuleT>;

        // ============================================================================
        // Root Driver
        // ============================================================================
        using RootDevicesT = system_build::RootDevicesFromDriverList<MainDriversT>;
        struct RootDriver : PackedDriverBase<RootDevicesT>
        {
            using BaseT = PackedDriverBase<RootDevicesT>;
            using BaseT::BaseT;

            VALLE_DEFINE_PACKED_DEVICE_DRIVER_ACCESSOR(i2c1, platform::I2C1CommandBufferDevice<>);
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
            PositionSensorModuleT      position_sensor;
        };

    }  // namespace app

    struct AppSystemBuildConfig : SystemBuildConfigBase<app::Drivers>
    {
    };

}  // namespace valle
