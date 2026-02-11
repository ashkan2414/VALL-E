#include "app.hpp"

namespace valle
{
    template <>
    struct HRTIMTimerISRRouter<app::HRTIMTimerDeviceT::skControllerID,
                               app::HRTIMTimerDeviceT::skTimerID,
                               HRTIMTimerInterruptType::kRepetition>
    {
        static void handle()
        {
            app::g_rep_counter.fetch_add(1, std::memory_order_relaxed);
        }
    };

}  // namespace valle

// Bind UART Logger ISR
VALLE_BIND_UART_LOGGER_ISR_ROUTERS(app::g_drivers.uart_logger);
