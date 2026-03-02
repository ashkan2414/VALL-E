#include "app.hpp"
#include "valle/core.hpp"

namespace valle
{
    inline float g_last_duty_cycle = 0.5F;
    inline bool  g_sample_on_reset = false;  // true = sample on RESET, false = PERIOD

    inline Atomic<bool> g_reset_triggered = false;

    constexpr float kSampleSwitchLower = 0.4F;  // switch to PERIOD only if duty < 0.4
    constexpr float kSampleSwitchUpper = 0.6F;  // switch to RESET only if duty > 0.6

    template <>
    struct ADCISRRouter<app::CurrentSensorADCChannelT::skControllerID, ADCInterruptType::kInjectEndOfSequence>
    {
        static void handle()
        {
            bool current_event_reset = false;
            if (g_reset_triggered.load(std::memory_order_acquire))
            {
                current_event_reset = true;
                g_reset_triggered.store(false, std::memory_order_release);
            }

            // --- update edge choice with hysteresis ---
            if (g_last_duty_cycle > kSampleSwitchUpper)
            {
                g_sample_on_reset = true;
            }
            else if (g_last_duty_cycle < kSampleSwitchLower)
            {
                g_sample_on_reset = false;
            }

            // only process sample if event matches chosen edge
            if (g_sample_on_reset != current_event_reset)
            {
                return;
            }

            // Toggle test GPIO on each sample event (for oscilloscope debugging)
            app::g_drivers.test_gpio.toggle();

            // First read the sampled current sensor and convert
            app::g_drivers.current_sensor.get_adc().on_data_available();

            // Then run VCA control loop
            g_last_duty_cycle = app::g_drivers.vca_controller.run_ctrl_loop();
        }
    };

    template <>
    struct HRTIMTimerISRRouter<app::kVCAPWMHRTIMControllerID,
                               app::kVCAPWMHRTIMTimerID,
                               HRTIMTimerInterruptType::kRepetition>
    {
        static void handle()
        {
            g_reset_triggered.store(true, std::memory_order_release);
        }
    };

}  // namespace valle

// Bind UART Logger ISR
VALLE_BIND_UART_LOGGER_ISR_ROUTERS(app::g_drivers.uart_logger);
