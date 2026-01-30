#include "Valle/System/hardware.hpp"

// Bind UART Logger ISR
VALLE_BIND_UART_LOGGER_DMA_ISR_ROUTER(app::g_drivers.uart_logger);
VALLE_BIND_UART_LOGGER_ISR_ROUTER(app::g_drivers.uart_logger);
