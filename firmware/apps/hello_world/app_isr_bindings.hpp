#include "valle/core/system/hardware.hpp"

// Bind UART Logger ISR
VALLE_BIND_UART_LOGGER_ISR_ROUTERS(app::g_drivers.uart_logger);
