#include "app.hpp"

namespace valle
{
    void on_current_sensor_adc_reading();
    void on_pwm_repetition();

    template <>
    struct ADCISRRouter<app::CurrentSensorADCChannelT::skControllerID, ADCInterruptType::kInjectEndOfSequence>
    {
        static void handle()
        {
            on_current_sensor_adc_reading();
        }
    };

    template <>
    struct DMAISRRouter<app::CurrentSensorADCDMAChannelT::skControllerID,
                        app::CurrentSensorADCDMAChannelT::skChannelID,
                        DMAChannelInterruptType::kTransferComplete>
    {
        static void handle()
        {
            on_current_sensor_adc_reading();
        }
    };

    template <>
    struct HRTIMTimerISRRouter<app::kVCAPWMHRTIMControllerID,
                               app::kVCAPWMHRTIMTimerID,
                               HRTIMTimerInterruptType::kRepetition>
    {
        static void handle()
        {
            on_pwm_repetition();
        }
    };

}  // namespace valle

// Bind UART Logger ISR
VALLE_BIND_UART_LOGGER_ISR_ROUTERS(app::g_drivers.uart_logger);
