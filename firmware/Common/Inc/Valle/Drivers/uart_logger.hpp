#pragma once

#include "Valle/Core/Logging/core.hpp"
#include "Valle/Device/Devices/uart.hpp"

namespace valle
{

    template <typename TUARTControllerDevice>
    class UARTLogger
    {
    public:
        using ControllerT   = TUARTControllerDevice;
        using InjectDevices = TypeList<ControllerT>;

    private:
        [[no_unique_address]] DeviceRef<ControllerT> m_uart;

    public:
        UARTLogger(DeviceRef<ControllerT>&& uart_controller) : m_uart(std::move(uart_controller))
        {
        }

        inline void init(const UARTControllerConfig& config)
        {
            m_uart->init(config);
        }

        void output(const std::string_view message)
        {
            m_uart->transmit(std::span(reinterpret_cast<const std::byte*>(message.data()), message.size()),
                             std::chrono::milliseconds(100));
        }

        void output_async(const std::string_view message)
        {
            m_uart->transmit_async(std::span(reinterpret_cast<const std::byte*>(message.data()), message.size()));
        }

        void handle_dma_irq()
        {
            m_uart->handle_dma_irq();
        }

        void handle_uart_irq()
        {
            m_uart->handle_uart_irq();
        }
    };

}  // namespace valle

// Put this in app_log_config.hpp to bind the logger instance
#define VALLE_BIND_UART_LOGGER_HANDLER(instance)               \
    namespace valle                                            \
    {                                                          \
        template <>                                            \
        struct LoggingOutputHandler<void>                      \
        {                                                      \
            static void output(const std::string_view message) \
            {                                                  \
                (instance).output(message);                    \
            }                                                  \
        };                                                     \
    }

// Put these in app_isr_bindings.hpp to bind your instance
#define VALLE_BIND_UART_LOGGER_DMA_ISR_ROUTER(instance)                                            \
    namespace valle                                                                                \
    {                                                                                              \
        static_assert(std::remove_cvref_t<decltype((instance))>::ControllerT::skHasDMATx,          \
                      "Instance does not have DMA TX capability");                                 \
        template <>                                                                                \
        struct DMAGlobalISRRouter<                                                                 \
            std::remove_cvref_t<decltype((instance))>::ControllerT::DMAChannelTxT::skControllerID, \
            std::remove_cvref_t<decltype((instance))>::ControllerT::DMAChannelTxT::skChannelID>    \
        {                                                                                          \
            static void handle()                                                                   \
            {                                                                                      \
                (instance).handle_dma_irq();                                                       \
            }                                                                                      \
        };                                                                                         \
    }

#define VALLE_BIND_UART_LOGGER_ISR_ROUTER(instance)                                                        \
    namespace valle                                                                                        \
    {                                                                                                      \
        template <>                                                                                        \
        struct UARTGlobalISRRouter<std::remove_cvref_t<decltype((instance))>::ControllerT::skControllerID> \
        {                                                                                                  \
            static void handle()                                                                           \
            {                                                                                              \
                (instance).handle_uart_irq();                                                              \
            }                                                                                              \
        };                                                                                                 \
    }
