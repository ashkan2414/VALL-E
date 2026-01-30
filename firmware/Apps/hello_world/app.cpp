#include "app.hpp"

VALLE_DEFINE_UART_LOGGER_HANDLER(app::g_drivers.uart_logger);

namespace valle::app
{
    DriverBuilderReturnT install_drivers(DriverBuilderT&& builder)
    {
        return std::move(builder).template install<UARTLoggerT>().yield();
    }

    /**
     * @brief Initialize shared devices.
     *
     */
    static void init_shared()
    {
        g_ref_registry.foreach_shared(Overloaded{
            [](DMA1ControllerDevice& dev) { dev.init(); },
            [](GPIOPortADevice& dev) { dev.init(); },
        }  // namespace valle
        );
    }

    /**
     * @brief Initialize drivers.
     *
     */
    static void init_drivers()
    {
        g_drivers.uart_logger.init(UARTControllerConfig{
            .baud_rate         = UARTBaudRate::kBaud115200,
            .word_length       = UARTWordLength::kBits8,
            .stop_bits         = UARTStopBits::kBits1,
            .parity            = UARTParity::kNone,
            .transfer_mode     = UARTTransferMode::kTxRx,
            .hw_flow_ctrl      = UARTHardwareFlowControl::kNone,
            .dma_priority      = DMAPriority::kHigh,
            .dma_int_priority  = 5,
            .uart_int_priority = 5,
        });
    }

    /**
     * @brief Post-initialize shared devices.
     *
     */
    static void post_init_shared()
    {
        g_ref_registry.foreach_shared(Overloaded{
            [](DMA1ControllerDevice& dev) { dev.post_init(); },
            [](GPIOPortADevice& dev) { dev.post_init(); },
        });
    }

    /**
     * @brief Initialize the system hardware.
     *
     */
    void init()
    {
        init_shared();
        init_drivers();
        post_init_shared();
    }

}  // namespace valle::app