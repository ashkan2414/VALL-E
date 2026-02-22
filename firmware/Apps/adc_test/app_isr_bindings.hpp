#include "Valle/System/hardware.hpp"

namespace valle
{
    static void on_data_available()
    {
        // First read the current sensor and convert
        app::g_drivers.test_adc.on_data_available();

        VALLE_LOG_INFO("ADC Voltage: {} V", app::g_drivers.test_adc.read());
    }

    template <>
    struct ADCISRRouter<app::TestADCChannelT::skControllerID, ADCInterruptType::kInjectEndOfSequence>
    {
        static void handle()
        {
            on_data_available();
        }
    };

    template <>
    struct DMAISRRouter<app::TestADCDMAChannelT::skControllerID,
                        app::TestADCDMAChannelT::skChannelID,
                        DMAChannelInterruptType::kTransferComplete>
    {
        static void handle()
        {
            on_data_available();
        }
    };

}  // namespace valle

// Bind UART Logger ISR
VALLE_BIND_UART_LOGGER_ISR_ROUTERS(app::g_drivers.uart_logger);
