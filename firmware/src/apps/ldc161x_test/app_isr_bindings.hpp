#include "app.hpp"

namespace valle::platform
{
    template <>
    struct GpioPinIsrRouter<valle::app::PositionSensorINTBPinT::skPortId, valle::app::PositionSensorINTBPinT::skPinId>
    {
        static void handle()
        {
            valle::app::intb_asserted_isr();
        }
    };
}  // namespace valle::platform

// Bind UART Logger ISR
VALLE_BIND_UART_LOGGER_ISR_ROUTERS(valle::app::g_drivers.uart_logger);
VALLE_BIND_I2C_COMMAND_BUFFER_DRIVER_ISR(valle::app::g_drivers.root.i2c1());