#pragma once

#include "Valle/Board/Device/device.hpp"
#include "Valle/Board/Drivers/uart_logger.hpp"
#include "Valle/Modules/acs724.hpp"
#include "Valle/Modules/ldc1612.hpp"
#include "Valle/Modules/vca.hpp"
#include "Valle/System/config.hpp"

namespace valle::app
{
    constexpr UARTControllerID kLoggerUARTID = UARTControllerID::kLPUART1;
    struct UARTControllerCTConfig : UARTControllerCTConfigDefaults<kLoggerUARTID>
    {
        using DMAChannelTxT = DMA1Channel2Device;
    };
    static constexpr UARTControllerCTConfig kLoggerUARTCTConfig{};

}  // namespace valle::app

VALLE_DEFINE_UART_CONTROLLER_CT_CONFIG(valle::app::kLoggerUARTID, valle::app::kLoggerUARTCTConfig);

namespace valle::app
{

    // ============================================================================
    // Device Configurations
    // ============================================================================
    using LoggerUARTControllerT = UARTControllerDevice<valle::app::kLoggerUARTID>;
    using UARTLoggerT           = UARTLogger<LoggerUARTControllerT>;

    struct Drivers
    {
        using DriversT = TypeList<UARTLoggerT>;

        UARTLoggerT uart_logger;
    };

    struct Devices
    {
        using DevicesT = TypeList<DMAMux1ControllerDevice, DMA1ControllerDevice, GPIOPortADevice>;

        [[no_unique_address]] DeviceRef<DMAMux1ControllerDevice> dmamux1;
        [[no_unique_address]] DeviceRef<DMA1ControllerDevice>    dma1;
        [[no_unique_address]] DeviceRef<GPIOPortADevice>         gpioa;
    };

}  // namespace valle::app

namespace valle::system
{
    struct Config : ConfigBase<app::Drivers, app::Devices>
    {
    };

}  // namespace valle::system