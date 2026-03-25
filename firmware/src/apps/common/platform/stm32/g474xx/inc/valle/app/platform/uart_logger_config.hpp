#pragma once

#include "valle/platform/drivers/uart/logger.hpp"

namespace valle::app
{
    constexpr auto kLoggerUartId = valle::platform::UartPeripheralId::kLPUart1;
    struct UartControllerCTConfig : valle::platform::UartControllerCTDefaultConfig<kLoggerUartId>
    {
        using DmaChannelTxT = valle::platform::Dma1Channel1Device;
    };
    static constexpr UartControllerCTConfig kLoggerUartCTConfig{};

}  // namespace valle::app

VALLE_DEFINE_UART_CONTROLLER_CT_CONFIG(valle::app::kLoggerUartId, valle::app::kLoggerUartCTConfig);

namespace valle::app
{
    using LoggerUartControllerT = valle::platform::UartControllerDevice<kLoggerUartId>;
    using UartLoggerT           = valle::platform::UartLogger<LoggerUartControllerT>;
}  // namespace valle::app