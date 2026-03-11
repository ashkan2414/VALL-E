#include <random>

#include "app.hpp"
#include "valle/app_bridge.hpp"
#include "valle/timing.hpp"

namespace valle
{
    void app::main()
    {
        TimingContext::delay_ms(3000);

        app::init();
        app::start_vca_controller();

        VALLE_LOG_INFO("Initialized!");
        TimingContext::delay_ms(1000);

        constexpr float kCmdBaselineA = 0.0F;
        constexpr float kCmdStepA     = 0.06F;

        uint32_t counter = 0;
        while (true)
        {
            VALLE_LOG_INFO("Starting step response capture #{}", ++counter);
            VALLE_LOG_INFO("Settling to baseline...");
            app::g_drivers.vca_controller.set_target_current(kCmdBaselineA);
            TimingContext::delay_ms(1000);

            VALLE_LOG_INFO("Starting step response capture...");

            app::g_current_response_collector.start_capture();
            for (size_t i = 0; i < app::kCaptureSteps / 2; ++i)
            {
                app::g_drivers.vca_controller.set_target_current(kCmdStepA);
                TimingContext::delay(app::kTargetSettleTime);
                app::g_drivers.vca_controller.set_target_current(kCmdBaselineA);
                TimingContext::delay(app::kTargetSettleTime);
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
            VALLE_LOG_INFO("Step response capture complete. Restarting in 3 seconds...");
            TimingContext::delay_ms(3000);
        }

        app::stop_vca_controller();

        while (true)
        {
        }
    }

}  // namespace valle