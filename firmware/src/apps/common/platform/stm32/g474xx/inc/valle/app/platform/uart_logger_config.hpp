#pragma once

#include "valle/platform/drivers/uart/logger.hpp"

namespace valle::app
{
    constexpr auto kLoggerUARTID = valle::platform::UARTControllerID::kLPUART1;
    struct UARTControllerCTConfig : valle::platform::UARTControllerCTDefaultConfig<kLoggerUARTID>
    {
        using DMAChannelTxT = valle::platform::DMA1Channel1Device;
    };
    static constexpr UARTControllerCTConfig kLoggerUARTCTConfig{};

}  // namespace valle::app

VALLE_DEFINE_UART_CONTROLLER_CT_CONFIG(valle::app::kLoggerUARTID, valle::app::kLoggerUARTCTConfig);

namespace valle::app
{
    using LoggerUARTControllerT = valle::platform::UARTControllerDevice<kLoggerUARTID>;
    using UARTLoggerT           = valle::platform::UARTLogger<LoggerUARTControllerT>;
}  // namespace valle::app