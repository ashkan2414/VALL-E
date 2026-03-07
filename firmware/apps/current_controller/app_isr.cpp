#include "app_isr_bindings.hpp"
#include "valle/core.hpp"

namespace valle
{

    constexpr float kSampleSwitchLower = 0.4F;  // switch to PERIOD only if duty < 0.4
    constexpr float kSampleSwitchUpper = 0.6F;  // switch to RESET only if duty > 0.6

    volatile inline float        g_last_duty_cycle = 0.5F;
    volatile inline bool         g_sample_on_reset = false;  // true = sample on RESET, false = PERIOD
    volatile inline Atomic<bool> g_reset_triggered = false;

    void on_current_sensor_adc_reading()
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

        app::g_current_response_collector.push_data(app::CurrentResponseData{
            .timestamp        = CycleClock::now(),
            .target_current   = app::g_drivers.vca_controller.get_target_current(),
            .measured_current = app::g_drivers.current_sensor.read_amps(),
        });

        // Then run VCA control loop
        g_last_duty_cycle = app::g_drivers.vca_controller.run_ctrl_loop();
    }

    void on_pwm_repetition()
    {
        g_reset_triggered.store(true, std::memory_order_release);
    }

}  // namespace valle