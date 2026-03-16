#include <random>

#include "app.hpp"
#include "valle/app_bridge.hpp"

namespace valle::app
{
    // Sets the current controller based on a normalized angle input [0, 1] (maps to 0 to 2pi along the motor cycle).
    void set_current(const float normalized_angle)
    {
    }

    void main()
    {
        using namespace system;

        TimingContext::delay_ms(3000);

        init();
        g_drivers.vca_current_loop_driver.start();
        g_drivers.motor_encoder.enable();

        VALLE_LOG_INFO("Initialized!");
        TimingContext::delay_ms(1000);

        while (true)
        {
            VALLE_LOG_INFO("Encoder Deg: {}", g_drivers.motor_encoder.get_position_deg_abs());
        }
    }
}  // namespace valle::app