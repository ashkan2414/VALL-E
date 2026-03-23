#include <random>

#include "app.hpp"
#include "valle/app_bridge.hpp"
#include "valle/system/timing.hpp"

namespace valle::app
{
    void main()
    {
        using namespace system;

        init();
        VALLE_LOG_INFO("Initialized!");
        TimingContext::delay_ms(3000);

        start();

        constexpr float kCmdBaselinePositionMM = 3.0F;
        constexpr float kCmdStepPositionMM     = 5.0F;

        VALLE_LOG_INFO("Settling to baseline...");
        g_drivers.position_loop_driver.set_target_position_mm<kResponseTargetValveChannel>(kCmdBaselinePositionMM);
        TimingContext::delay_ms(1000);

        while (true)
        {
            g_position_response_collector.start_capture();
            for (size_t i = 0; i < kCaptureSteps / 2; ++i)
            {
                g_drivers.position_loop_driver.set_target_position_mm<kResponseTargetValveChannel>(
                    kCmdBaselinePositionMM);
                TimingContext::delay(kTargetSettleTime);
                g_drivers.position_loop_driver.set_target_position_mm<kResponseTargetValveChannel>(kCmdStepPositionMM);
                TimingContext::delay(kTargetSettleTime);
            }
            g_position_response_collector.stop_capture();

            auto captured_data = g_position_response_collector.get_captured_data();

            VALLE_LOG_INFO("t_ms,cmd_mm,meas_mm");
            if (!captured_data.empty())
            {
                const auto first_timestamp = captured_data.front().timestamp;

                while (auto data = captured_data.pop())
                {
                    const auto relative_timestamp =
                        std::chrono::duration_cast<DurationMillis>(data->timestamp - first_timestamp);

                    VALLE_LOG_INFO(
                        "{},{:.6f},{:.6f}", relative_timestamp.count(), data->target_position, data->measured_position);
                }
            }
            else
            {
                VALLE_LOG_INFO("No data captured.");
            }
        }

        stop();

        while (true)
        {
        }
    }

}  // namespace valle::app