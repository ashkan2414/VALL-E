#include "app.hpp"

// Bind UART Logger ISR
VALLE_BIND_UART_LOGGER_ISR_ROUTERS(valle::app::g_drivers.uart_logger);
VALLE_BIND_I2C_COMMAND_BUFFER_DRIVER_ISR(valle::app::g_drivers.root.i2c1());