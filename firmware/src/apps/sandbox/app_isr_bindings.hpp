#include "valle/base/system_build/build.hpp"

namespace valle::platform
{

    template <>
    struct ADCISRRouter<valle::app::CurrentSensorT::ADCChannelInterfaceT::ChannelT::skControllerID,
                        ADCInterruptType::kInjectEndOfSequence>
    {
        static_assert(valle::app::CurrentSensorT::ADCChannelInterfaceT::ChannelT::skGroup == ADCChannelGroup::kInject,
                      "ADCISRRouter specialization used for non-inject channel");

        static void handle()
        {
            // First read the current sensor and convert
            valle::app::g_drivers.current_sensor.get_adc().on_data_available();

            // Then run VCA control loop
            valle::app::g_drivers.vca_controller.run_ctrl_loop(valle::app::g_drivers.current_sensor.read_amps());
        }
    };

}  // namespace valle::platform

// Bind UART Logger ISR
VALLE_BIND_UART_LOGGER_ISR_ROUTERS(valle::app::g_drivers.uart_logger);

// Bind I2C Command Buffer Device ISRs
VALLE_BIND_I2C_COMMAND_BUFFER_DRIVER_ISR(valle::app::g_drivers.root.i2c1());
