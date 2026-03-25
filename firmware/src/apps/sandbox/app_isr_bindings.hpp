#include "app.hpp"

namespace valle::platform
{

    template <>
    struct AdcIsrRouter<valle::app::VCACurrentLoopDriverT::skCurrentSensorAdcPeripheralId,
                        AdcInterruptType::kInjectEndOfSequence>
    {
        static void handle()
        {
            if (valle::app::g_drivers.vca_current_loop_driver.on_current_sensor_adc_reading_isr())
            {
                // Toggle test GPIO on each sample event (for oscilloscope debugging)
                valle::app::g_drivers.test_gpio.toggle();
            }
        }
    };

    template <>
    struct HrtimTimerIsrRouter<valle::app::VCACurrentLoopDriverT::skVcaHrtimPwmPeripheralId,
                               valle::app::VCACurrentLoopDriverT::skVcaHrtimPwmTimerId,
                               HrtimTimerInterruptType::kRepetition>
    {
        static void handle()
        {
            valle::app::g_drivers.vca_current_loop_driver.on_pwm_repetition_isr();
        }
    };

}  // namespace valle::platform

// Bind UART Logger ISR
VALLE_BIND_UART_LOGGER_ISR_ROUTERS(valle::app::g_drivers.uart_logger);
