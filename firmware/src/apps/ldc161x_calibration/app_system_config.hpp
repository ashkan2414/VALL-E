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
    constexpr platform::I2cControllerId kPositionSensorI2cId             = platform::I2cControllerId::kI2c1;
    constexpr uint16_t                  kPositionSensorI2cAddress        = 0x2B;
    constexpr bool                      kPositionSensorI2cAddressIs10Bit = false;

    struct PositionSensorI2cControllerCTConfig : public platform::I2cControllerCTDefaultConfig
    {
        using DmaChannelRxT = platform::Dma1Channel3;
        using DmaChannelTxT = platform::Dma1Channel4;
        using SCLPinT       = platform::GpioPinB8;
        using SDAPinT       = platform::GpioPinB9;
    };
}  // namespace valle::app

VALLE_DEFINE_I2C_CONTROLLER_CT_CONFIG(valle::app::kPositionSensorI2cId,
                                      valle::app::PositionSensorI2cControllerCTConfig{});

namespace valle
{
    namespace app
    {
        // ============================================================================
        // Drivers
        // ============================================================================
        using PositionSensorI2cControllerT  = platform::I2cCommandBuffer<kPositionSensorI2cId>;
        using PositionSensorI2cSlaveDeviceT = platform::
            I2cCommandBufferSlave<kPositionSensorI2cId, kPositionSensorI2cAddress, kPositionSensorI2cAddressIs10Bit>;
        using PositionSensorModuleT       = platform::app::LDC161XSensorModule<PositionSensorI2cSlaveDeviceT, 2>;
        using PositionSensorModuleConfigT = typename PositionSensorModuleT::ConfigT;
        using PositionSensorConfigT       = typename PositionSensorModuleT::SensorConfigT;

        // Declare Main Driver List
        using MainDriversT = TypeList<platform::CoreSystemDriver, UartLoggerT, PositionSensorModuleT>;

        // ============================================================================
        // Root Driver
        // ============================================================================
        using RootDevicesT = system_build::RootDevicesFromDriverList<MainDriversT>;
        struct RootDriver : PackedDriverBase<RootDevicesT>
        {
            using BaseT = PackedDriverBase<RootDevicesT>;
            using BaseT::BaseT;

            VALLE_DEFINE_PACKED_DEVICE_DRIVER_ACCESSOR(i2c1, platform::I2c1CommandBuffer<>);
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
            PositionSensorModuleT      position_sensor;
        };

    }  // namespace app

    struct AppSystemBuildConfig : SystemBuildConfigBase<app::Drivers>
    {
    };

}  // namespace valle
