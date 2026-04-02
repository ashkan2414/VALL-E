#include "valle/platform/devices/i2c.hpp"

#include "magic_enum/magic_enum.hpp"

#if __has_include("app_isr_bindings.hpp")
#include "app_isr_bindings.hpp"
#endif

namespace valle::platform
{
    /**
     * @brief Check if any granular ISR handler is bound for this I2C.
     *
     * @tparam tkControllerId I2C Controller ID.
     */
    template <I2cControllerId tkControllerId>
    [[nodiscard]] consteval static inline bool i2c_has_any_granular_isr_handler()
    {
        constexpr auto values = magic_enum::enum_values<I2cInterruptSource>();

        return [values]<std::size_t... Is>(std::index_sequence<Is...>)
        {
            return (CBoundIsrRouter<I2cIsrRouter<tkControllerId, values[Is]>> || ...);
        }(std::make_index_sequence<values.size()>{});
    }

    /**
     * @brief I2C Event Interrupt Handler Router
     *
     * @tparam tkControllerId I2C Controller ID.
     */
    template <I2cControllerId tkControllerId>
    static inline void i2c_event_irq_handler()
    {
        using IrqRouterT                   = I2cGlobalEventIsrRouter<tkControllerId>;
        constexpr bool has_irq_handler     = CBoundIsrRouter<IrqRouterT>;
        constexpr bool has_source_handlers = i2c_has_any_granular_isr_handler<tkControllerId>();
        static_assert(!(has_irq_handler && has_source_handlers),
                      "VALLE CONFLICT: IRQ and Source ISR handlers detected.");

        if constexpr (has_irq_handler)
        {
            IrqRouterT::handle();
            return;
        }

#define HANDLE_I2C_INT(tkIntSource)                                                   \
    {                                                                                 \
        using RouterT = I2cIsrRouter<tkControllerId, (tkIntSource)>;                  \
        using TraitsT = I2cInterruptSourceInterface<tkControllerId, tkIntSource>;     \
        if constexpr (CBoundIsrRouter<RouterT>)                                       \
        {                                                                             \
            if (TraitsT::is_pending())                                                \
            {                                                                         \
                /* Note: Some I2C flags (like TXIS/RXNE) are cleared by hardware */   \
                /* via register access, but we call clear() per traits definition. */ \
                if constexpr (kIsrRouterConfigAck<RouterT>)                           \
                {                                                                     \
                    TraitsT::clear();                                                 \
                }                                                                     \
                RouterT::handle();                                                    \
            }                                                                         \
        }                                                                             \
    }

        HANDLE_I2C_INT(I2cInterruptSource::kTx);
        HANDLE_I2C_INT(I2cInterruptSource::kRx);
        HANDLE_I2C_INT(I2cInterruptSource::kAddressMatch);
        HANDLE_I2C_INT(I2cInterruptSource::kNACKReceived);
        HANDLE_I2C_INT(I2cInterruptSource::kStopDetection);
        HANDLE_I2C_INT(I2cInterruptSource::kTransferComplete);

#undef HANDLE_I2C_INT
    }

    /**
     * @brief I2C Error Interrupt Handler Router
     *
     * @tparam tkControllerId I2C Index (1-4)
     */
    template <I2cControllerId tkControllerId>
    static inline void i2c_error_irq_handler()
    {
        using IrqRouterT                   = I2cGlobalErrorIsrRouter<tkControllerId>;
        constexpr bool has_irq_handler     = CBoundIsrRouter<IrqRouterT>;
        constexpr bool has_source_handlers = i2c_has_any_granular_isr_handler<tkControllerId>();
        static_assert(!(has_irq_handler && has_source_handlers),
                      "VALLE CONFLICT: IRQ and Source ISR handlers detected.");

        if constexpr (has_irq_handler)
        {
            IrqRouterT::handle();
            return;
        }

#define HANDLE_I2C_ERR_INT(tkIntSource)                                                          \
    {                                                                                            \
        using RouterT = I2cIsrRouter<tkControllerId, I2cInterruptSource::kError>;                \
        using TraitsT = I2cInterruptSourceInterface<tkControllerId, I2cInterruptSource::kError>; \
        if constexpr (CBoundIsrRouter<RouterT>)                                                  \
        {                                                                                        \
            if (TraitsT::template is_pending<tkIntSource>())                                     \
            {                                                                                    \
                if constexpr (kIsrRouterConfigAck<RouterT>)                                      \
                {                                                                                \
                    TraitsT::template ack<tkIntSource>();                                        \
                }                                                                                \
                RouterT::handle(tkIntSource);                                                    \
            }                                                                                    \
        }                                                                                        \
    }

        HANDLE_I2C_ERR_INT(I2cErrorInterruptSource::kArbitrationLoss);
        HANDLE_I2C_ERR_INT(I2cErrorInterruptSource::kBusError);
        HANDLE_I2C_ERR_INT(I2cErrorInterruptSource::kOverrunUnderrun);
        HANDLE_I2C_ERR_INT(I2cErrorInterruptSource::kSMBusTimeout);
        HANDLE_I2C_ERR_INT(I2cErrorInterruptSource::kSMBusPecError);
        HANDLE_I2C_ERR_INT(I2cErrorInterruptSource::kSMBusAlert);
    }

    extern "C"
    {
        // --- I2C 1 ---
        void I2C1_EV_IRQHandler(void)
        {
            i2c_event_irq_handler<I2cControllerId::kI2c1>();
        }
        void I2C1_ER_IRQHandler(void)
        {
            i2c_error_irq_handler<I2cControllerId::kI2c1>();
        }

        // --- I2C 2 ---
        void I2C2_EV_IRQHandler(void)
        {
            i2c_event_irq_handler<I2cControllerId::kI2c2>();
        }
        void I2C2_ER_IRQHandler(void)
        {
            i2c_error_irq_handler<I2cControllerId::kI2c2>();
        }

        // --- I2C 3 ---
        void I2C3_EV_IRQHandler(void)
        {
            i2c_event_irq_handler<I2cControllerId::kI2c3>();
        }
        void I2C3_ER_IRQHandler(void)
        {
            i2c_error_irq_handler<I2cControllerId::kI2c3>();
        }

        // --- I2C 4 ---
        void I2C4_EV_IRQHandler(void)
        {
            i2c_event_irq_handler<I2cControllerId::kI2c4>();
        }
        void I2C4_ER_IRQHandler(void)
        {
            i2c_error_irq_handler<I2cControllerId::kI2c4>();
        }
    }
}  // namespace valle::platform
