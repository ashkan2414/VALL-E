#include "app.hpp"

namespace valle::app
{
    void on_current_sensor_adc_reading();
    void on_pwm_repetition();
}  // namespace valle::app

namespace valle::platform
{

    template <>
    struct ADCISRRouter<valle::app::CurrentSensorADCChannelT::skControllerID, ADCInterruptType::kInjectEndOfSequence>
    {
        static void handle()
        {
            app::on_current_sensor_adc_reading();
        }
    };

    template <>
    struct DMAISRRouter<valle::app::CurrentSensorADCDMAChannelT::skControllerID,
                        valle::app::CurrentSensorADCDMAChannelT::skChannelID,
                        DMAChannelInterruptType::kTransferComplete>
    {
        static void handle()
        {
            app::on_current_sensor_adc_reading();
        }
    };

    template <>
    struct HRTIMTimerISRRouter<valle::app::kVCAPWMHRTIMControllerID,
                               valle::app::kVCAPWMHRTIMTimerID,
                               HRTIMTimerInterruptType::kRepetition>
    {
        static void handle()
        {
            app::on_pwm_repetition();
        }
    };

}  // namespace valle::platform

// Bind UART Logger ISR
VALLE_BIND_UART_LOGGER_ISR_ROUTERS(valle::app::g_drivers.uart_logger);
