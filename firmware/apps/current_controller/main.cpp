#include <random>

#include "app.hpp"
#include "valle/core/app_bridge.hpp"

namespace valle
{
    void app::main()
    {
        delay_ms(1000);

        // Init and start
        app::init();
        app::g_devices.adc3->start_inject();
        app::g_drivers.vca_controller.enable();

        delay_ms(1000);  // Let things stabilize before starting capture

        // Step 1: Collect Data
        app::g_current_response_collector.start_capture();
        delay_us(kCaptureDurationMicros);
        app::g_current_response_collector.stop_capture();

        // Step 2: Log Data
        auto captured_data   = app::g_current_response_collector.get_captured_data();
        auto first_timestamp = captured_data.front().timestamp;

        while (auto data = captured_data.pop())
        {
            const auto relative_timestamp =
                std::chrono::duration_cast<std::chrono::microseconds>(data->timestamp - first_timestamp);

            // Log format: timestamp (us), target current (A), measured current (A)
            VALLE_LOG_INFO("{},{},{}", relative_timestamp.count(), data->target_current, data->measured_current);
        }

        uint32_t counter = 0;
        while (true)
        {
            VALLE_LOG_INFO("Hello World #{}! Current: {} A", ++counter, app::g_drivers.current_sensor.read_amps());
            delay_ms(1000);
        }
    }

}  // namespace valle