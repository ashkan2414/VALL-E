#include <random>

#include "app.hpp"
#include "valle/app_bridge.hpp"
#include "valle/system/timing.hpp"

namespace valle::app
{
    void main()
    {
        using namespace system;

        TimingContext::delay_ms(3000);

        init();
        g_drivers.vca_current_loop_driver.start();

        VALLE_LOG_INFO("Initialized!");
        constexpr float kCmdBaselineA = 0.0F;
        constexpr float kCmdStepA     = 0.15F;

        VALLE_LOG_INFO("Settling to baseline...");
        g_drivers.vca_current_loop_driver.set_target_current_amps(kCmdBaselineA);
        TimingContext::delay_ms(1000);

        while (true)
        {
            g_current_response_collector.start_capture();
            for (size_t i = 0; i < kCaptureSteps / 2; ++i)
            {
                g_drivers.vca_current_loop_driver.set_target_current_amps(kCmdBaselineA);
                TimingContext::delay(kTargetSettleTime);
                g_drivers.vca_current_loop_driver.set_target_current_amps(kCmdStepA);
                TimingContext::delay(kTargetSettleTime);
            }
            g_current_response_collector.stop_capture();

            auto captured_data = g_current_response_collector.get_captured_data();

            VALLE_LOG_INFO("t_us,cmd_A,meas_A,duty_cycle");
            if (!captured_data.empty())
            {
                const auto first_timestamp = captured_data.front().timestamp;

                while (auto data = captured_data.pop())
                {
                    const auto relative_timestamp =
                        std::chrono::duration_cast<DurationMicros>(data->timestamp - first_timestamp);

                    VALLE_LOG_INFO("{},{:.6f},{:.6f},{:.6f}",
                                   relative_timestamp.count(),
                                   data->target_current,
                                   data->measured_current,
                                   data->duty_cycle);
                }
            }
            else
            {
                VALLE_LOG_INFO("No data captured.");
            }
        }

        g_drivers.vca_current_loop_driver.stop();

        while (true)
        {
        }
    }

}  // namespace valle::app