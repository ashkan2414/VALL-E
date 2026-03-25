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
     * @tparam tkPeripheralId I2C Peripheral ID.
     */
    template <I2cPeripheralId tkPeripheralId>
    [[nodiscard]] consteval static inline bool i2c_has_any_granular_isr_handler()
    {
        constexpr auto values = magic_enum::enum_values<I2cInterruptType>();

        return [values]<std::size_t... Is>(std::index_sequence<Is...>)
        {
            return (CBoundIsrRouter<I2cIsrRouter<tkPeripheralId, values[Is]>> || ...);
        }(std::make_index_sequence<values.size()>{});
    }

    /**
     * @brief I2C Event Interrupt Handler Router
     *
     * @tparam tkPeripheralId I2C Peripheral ID.
     */
    template <I2cPeripheralId tkPeripheralId>
    static inline void i2c_event_irq_handler()
    {
        using GlobalRouterT               = I2cGlobalEventIsrRouter<tkPeripheralId>;
        constexpr bool kHasGlobalRouter   = CBoundIsrRouter<GlobalRouterT>;
        constexpr bool kHasGranularRouter = i2c_has_any_granular_isr_handler<tkPeripheralId>();
        static_assert(!(kHasGlobalRouter && kHasGranularRouter), "VALLE CONFLICT: Global and Granular ISRs detected.");

        if constexpr (kHasGlobalRouter)
        {
            GlobalRouterT::handle();
            return;
        }

#define HANDLE_I2C_INT(tkIntType)                                                   \
    {                                                                               \
        using RouterT = I2cIsrRouter<tkPeripheralId, (tkIntType)>;                  \
        using TraitsT = I2cInterruptTraits<tkPeripheralId, tkIntType>;              \
        if constexpr (CBoundIsrRouter<RouterT>)                                     \
        {                                                                           \
            if (TraitsT::is_pending())                                              \
            {                                                                       \
                /* Note: Some I2C flags (like TXIS/RXNE) are cleared by hardware */ \
                /* via register access, but we call ack() per traits definition. */ \
                if constexpr (kIsrRouterConfigAck<RouterT>)                         \
                {                                                                   \
                    TraitsT::ack();                                                 \
                }                                                                   \
                RouterT::handle();                                                  \
            }                                                                       \
        }                                                                           \
    }

        HANDLE_I2C_INT(I2cInterruptType::kTx);
        HANDLE_I2C_INT(I2cInterruptType::kRx);
        HANDLE_I2C_INT(I2cInterruptType::kAddressMatch);
        HANDLE_I2C_INT(I2cInterruptType::kNACKReceived);
        HANDLE_I2C_INT(I2cInterruptType::kStopDetection);
        HANDLE_I2C_INT(I2cInterruptType::kTransferComplete);

#undef HANDLE_I2C_INT
    }

    /**
     * @brief I2C Error Interrupt Handler Router
     *
     * @tparam tkPeripheralId I2C Index (1-4)
     */
    template <I2cPeripheralId tkPeripheralId>
    static inline void i2c_error_irq_handler()
    {
        using GlobalRouterT               = I2cGlobalErrorIsrRouter<tkPeripheralId>;
        constexpr bool kHasGlobalRouter   = CBoundIsrRouter<GlobalRouterT>;
        constexpr bool kHasGranularRouter = i2c_has_any_granular_isr_handler<tkPeripheralId>();
        static_assert(!(kHasGlobalRouter && kHasGranularRouter), "VALLE CONFLICT: Global and Granular ISRs detected.");

        if constexpr (kHasGlobalRouter)
        {
            GlobalRouterT::handle();
            return;
        }

#define HANDLE_I2C_ERR_INT(tkIntType)                                                 \
    {                                                                                 \
        using RouterT = I2cIsrRouter<tkPeripheralId, I2cInterruptType::kError>;       \
        using TraitsT = I2cInterruptTraits<tkPeripheralId, I2cInterruptType::kError>; \
        if constexpr (CBoundIsrRouter<RouterT>)                                       \
        {                                                                             \
            if (TraitsT::template is_pending<tkIntType>())                            \
            {                                                                         \
                if constexpr (kIsrRouterConfigAck<RouterT>)                           \
                {                                                                     \
                    TraitsT::template ack<tkIntType>();                               \
                }                                                                     \
                RouterT::handle(tkIntType);                                           \
            }                                                                         \
        }                                                                             \
    }

        HANDLE_I2C_ERR_INT(I2cErrorInterruptType::kArbitrationLoss);
        HANDLE_I2C_ERR_INT(I2cErrorInterruptType::kBusError);
        HANDLE_I2C_ERR_INT(I2cErrorInterruptType::kOverrunUnderrun);
        HANDLE_I2C_ERR_INT(I2cErrorInterruptType::kSMBusTimeout);
        HANDLE_I2C_ERR_INT(I2cErrorInterruptType::kSMBusPecError);
        HANDLE_I2C_ERR_INT(I2cErrorInterruptType::kSMBusAlert);
    }

    extern "C"
    {
        // --- I2C 1 ---
        void I2C1_EV_IRQHandler(void)
        {
            i2c_event_irq_handler<I2cPeripheralId::kI2c1>();
        }
        void I2C1_ER_IRQHandler(void)
        {
            i2c_error_irq_handler<I2cPeripheralId::kI2c1>();
        }

        // --- I2C 2 ---
        void I2C2_EV_IRQHandler(void)
        {
            i2c_event_irq_handler<I2cPeripheralId::kI2c2>();
        }
        void I2C2_ER_IRQHandler(void)
        {
            i2c_error_irq_handler<I2cPeripheralId::kI2c2>();
        }

        // --- I2C 3 ---
        void I2C3_EV_IRQHandler(void)
        {
            i2c_event_irq_handler<I2cPeripheralId::kI2c3>();
        }
        void I2C3_ER_IRQHandler(void)
        {
            i2c_error_irq_handler<I2cPeripheralId::kI2c3>();
        }

        // --- I2C 4 ---
        void I2C4_EV_IRQHandler(void)
        {
            i2c_event_irq_handler<I2cPeripheralId::kI2c4>();
        }
        void I2C4_ER_IRQHandler(void)
        {
            i2c_error_irq_handler<I2cPeripheralId::kI2c4>();
        }
    }
}  // namespace valle::platform
