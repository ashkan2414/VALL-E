#include "app.hpp"

// Bind UART Logger ISR
VALLE_BIND_UART_LOGGER_ISR_ROUTERS(valle::app::g_drivers.uart_logger);
VALLE_BIND_VCA_CURRENT_LOOP_DRIVER_ISR_ROUTERS(valle::app::g_drivers.intake_vca_current_loop_driver);
VALLE_BIND_VCA_CURRENT_LOOP_DRIVER_ISR_ROUTERS(valle::app::g_drivers.exhaust_vca_current_loop_driver);
VALLE_BIND_I2C_COMMAND_BUFFER_DRIVER_ISR(valle::app::g_drivers.root.i2c1());
VALLE_BIND_LDC161X_VALVE_POSITION_CONTROLLER_ISR_ROUTERS(valle::app::g_drivers.position_loop_driver);