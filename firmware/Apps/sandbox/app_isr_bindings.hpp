#include "Valle/System/hardware.hpp"

namespace valle
{

    template <>
    struct ADCISRRouter<app::CurrentSensorT::ChannelT::skControllerID, ADCInterruptType::kInjectEndOfSequence>
    {
        static_assert(app::CurrentSensorT::ChannelT::skGroup == ADCChannelGroup::kInject,
                      "ADCISRRouter specialization used for non-inject channel");

        static void handle()
        {
            // First read the current sensor and convert
            app::g_drivers.current_sensor.on_data_available();

            // Then run VCA control loop
            app::g_drivers.vca.run_ctrl_loop();
        }
    };

}  // namespace valle

// Bind UART Logger ISR
VALLE_BIND_UART_LOGGER_ISR_ROUTERS(app::g_drivers.uart_logger);

// Bind I2C Command Buffer Device ISRs
VALLE_BIND_I2C_COMMAND_BUFFER_DRIVER_ISR(app::g_devices.i2c1.get());
