#include <random>
 
#include "app.hpp"
#include "valle/core/app_bridge.hpp"
 
namespace valle
{
    void app::main()
    {
        delay_ms(3000);
 
        app::init();
        if constexpr (kCurrentSensorUseInject)
        {
            app::g_devices.adc1->start_inject();
        }
        else
        {
            app::g_devices.adc1->start_regular();
        }
        app::g_drivers.vca_controller.enable();
 
        VALLE_LOG_INFO("Initialized!");
        delay_ms(1000);
 
        constexpr float kCmdBaselineA = 0.0F;
        constexpr float kCmdStepA     = 0.15F;
 
        uint32_t counter = 0;
        while (true)
        {
            //VALLE_LOG_INFO("Starting step response capture #{}", ++counter);
            //VALLE_LOG_INFO("Settling to baseline...");
            app::g_drivers.vca_controller.set_target_current(kCmdBaselineA);
            app::g_drivers.test_gpio.write(false);
            //delay_ms(1000);
 
            VALLE_LOG_INFO("Starting step response capture...");
 
            app::g_current_response_collector.start_capture();
            for (size_t i = 0; i < app::kCaptureSteps / 2; ++i)
            {
                app::g_drivers.vca_controller.set_target_current(kCmdStepA);
                app::g_drivers.test_gpio.write(true);
                delay(app::kTargetSettleTime);
                app::g_drivers.vca_controller.set_target_current(kCmdBaselineA);
                app::g_drivers.test_gpio.write(false);
                delay(app::kTargetSettleTime);
            }
            app::g_current_response_collector.stop_capture();
 
            VALLE_LOG_INFO("Capture complete! Processing data...");
 
            auto captured_data = app::g_current_response_collector.get_captured_data();
 
            VALLE_LOG_INFO("t_us,cmd_A,meas_A");
            if (!captured_data.empty())
            {
                const auto first_timestamp = captured_data.front().timestamp;
 
                while (auto data = captured_data.pop())
                {
                    const auto relative_timestamp =
                        std::chrono::duration_cast<DurationMicros>(data->timestamp - first_timestamp);
 
                    VALLE_LOG_INFO(
                        "{},{:.6f},{:.6f}", relative_timestamp.count(), data->target_current, data->measured_current);
                }
            }
            else
            {
                VALLE_LOG_INFO("No data captured.");
            }
 
            app::g_drivers.vca_controller.set_target_current(0.0F);
            app::g_drivers.test_gpio.write(false);
            //VALLE_LOG_INFO("Step response capture complete. Restarting in 3 seconds...");
            //delay_ms(3000);
        }
 
        app::g_drivers.vca_controller.disable();
 
        while (true)
        {
        }
    }
 
}  // namespace valle