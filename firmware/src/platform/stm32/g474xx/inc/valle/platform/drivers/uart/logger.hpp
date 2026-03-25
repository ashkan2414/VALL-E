#pragma once

#include "valle/base/logging/config_base.hpp"
#include "valle/platform/devices/uart.hpp"

namespace valle::platform
{

    template <typename TUartControllerDevice>
    class UartLogger
    {
    public:
        using ControllerT   = TUartControllerDevice;
        using InjectDevices = TypeList<ControllerT>;

    private:
        [[no_unique_address]] DeviceRef<ControllerT> m_uart;

    public:
        UartLogger() = delete;

        UartLogger(DeviceRef<ControllerT>&& uart_controller) : m_uart(std::move(uart_controller))
        {
        }

        [[nodiscard]] inline bool init(const UartControllerConfig& config)
        {
            return m_uart->init(config);
        }

        void output_sync(const std::string_view message)
        {
            m_uart->transmit(std::span(reinterpret_cast<const std::byte*>(message.data()), message.size()),
                             TimingContext::TimeoutMillisT(100));
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

}  // namespace valle::platform

// Put this in app_log_config.hpp to bind the logger instance
#define VALLE_DEFINE_UART_LOGGER_HANDLER(instance)              \
    namespace valle                                             \
    {                                                           \
        void log_output_handler(const std::string_view message) \
        {                                                       \
            (instance).output_sync(message);                    \
        }                                                       \
    }

// Put these in app_isr_bindings.hpp to bind your instance
#define VALLE_BIND_UART_LOGGER_DMA_ISR_ROUTER(instance)                                            \
    namespace valle::platform                                                                      \
    {                                                                                              \
        static_assert(std::remove_cvref_t<decltype((instance))>::ControllerT::skHasDmaTx,          \
                      "Instance does not have DMA TX capability");                                 \
        template <>                                                                                \
        struct DmaGlobalIsrRouter<                                                                 \
            std::remove_cvref_t<decltype((instance))>::ControllerT::DmaChannelTxT::skPeripheralId, \
            std::remove_cvref_t<decltype((instance))>::ControllerT::DmaChannelTxT::skChannelId>    \
        {                                                                                          \
            static void handle()                                                                   \
            {                                                                                      \
                (instance).handle_dma_irq();                                                       \
            }                                                                                      \
        };                                                                                         \
    }

#define VALLE_BIND_UART_LOGGER_UART_ISR_ROUTER(instance)                                                   \
    namespace valle::platform                                                                              \
    {                                                                                                      \
        template <>                                                                                        \
        struct UartGlobalIsrRouter<std::remove_cvref_t<decltype((instance))>::ControllerT::skPeripheralId> \
        {                                                                                                  \
            static void handle()                                                                           \
            {                                                                                              \
                (instance).handle_uart_irq();                                                              \
            }                                                                                              \
        };                                                                                                 \
    }

#define VALLE_BIND_UART_LOGGER_ISR_ROUTERS(instance) \
    VALLE_BIND_UART_LOGGER_DMA_ISR_ROUTER(instance)  \
    VALLE_BIND_UART_LOGGER_UART_ISR_ROUTER(instance)
