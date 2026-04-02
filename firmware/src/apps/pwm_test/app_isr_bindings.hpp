#include "app.hpp"

namespace valle::platform
{
    template <>
    struct HrtimTimerIsrRouter<valle::app::HrtimTimerDeviceT::skControllerId,
                               valle::app::HrtimTimerDeviceT::skTimerId,
                               HrtimTimerInterruptSource::kRepetition>
    {
        static void handle()
        {
        }
    };

}  // namespace valle::platform

// Bind UART Logger ISR
VALLE_BIND_UART_LOGGER_ISR_ROUTERS(valle::app::g_drivers.uart_logger);
