#include "Valle/Device/Devices/i2c.hpp"

#include "magic_enum/magic_enum.hpp"

#if __has_include("app_isr_bindings.hpp")
#include "app_isr_bindings.hpp"
#endif

namespace valle
{
    /**
     * @brief Check if any granular ISR handler is bound for this I2C.
     *
     * @tparam tkControllerID I2C Controller ID.
     */
    template <I2CControllerID tkControllerID>
        requires(kValidI2CControllerID<tkControllerID>)
    [[nodiscard]] consteval static inline bool i2c_has_any_granular_isr_handler()
    {
        constexpr auto values = magic_enum::enum_values<I2CInterruptType>();

        return [values]<std::size_t... Is>(std::index_sequence<Is...>)
        {
            return (CBoundISRRouter<I2CISRRouter<tkControllerID, values[Is]>> || ...);
        }(std::make_index_sequence<values.size()>{});
    }

    /**
     * @brief I2C Event Interrupt Handler Router
     *
     * @tparam tkControllerID I2C Index (1-4)
     */
    template <I2CControllerID tkControllerID>
        requires(kValidI2CControllerID<tkControllerID>)
    static inline void i2c_event_irq_handler()
    {
        using GlobalRouterT               = I2CGlobalEventISRRouter<tkControllerID>;
        constexpr bool kHasGlobalRouter   = CBoundISRRouter<GlobalRouterT>;
        constexpr bool kHasGranularRouter = i2c_has_any_granular_isr_handler<tkControllerID>();
        static_assert(!(kHasGlobalRouter && kHasGranularRouter), "VALLE CONFLICT: Global and Granular ISRs detected.");

        if constexpr (kHasGlobalRouter)
        {
            GlobalRouterT::handle();
            return;
        }

#define HANDLE_I2C_INT(tkIntType)                                                   \
    {                                                                               \
        using RouterT = I2CISRRouter<tkControllerID, (tkIntType)>;                  \
        using TraitsT = I2CInterruptTraits<tkControllerID, tkIntType>;              \
        if constexpr (CBoundISRRouter<RouterT>)                                     \
        {                                                                           \
            if (TraitsT::is_pending())                                              \
            {                                                                       \
                /* Note: Some I2C flags (like TXIS/RXNE) are cleared by hardware */ \
                /* via register access, but we call ack() per traits definition. */ \
                if constexpr (kISRRouterConfigAck<RouterT>)                         \
                {                                                                   \
                    TraitsT::ack();                                                 \
                }                                                                   \
                RouterT::handle();                                                  \
            }                                                                       \
        }                                                                           \
    }

        HANDLE_I2C_INT(I2CInterruptType::kTx);
        HANDLE_I2C_INT(I2CInterruptType::kRx);
        HANDLE_I2C_INT(I2CInterruptType::kAddressMatch);
        HANDLE_I2C_INT(I2CInterruptType::kNACKReceived);
        HANDLE_I2C_INT(I2CInterruptType::kStopDetection);
        HANDLE_I2C_INT(I2CInterruptType::kTransferComplete);

#undef HANDLE_I2C_INT
    }

    /**
 * @brief I2C Error Interrupt Handler Router
 *
 * @tparam tkControllerID I2C Index (1-4)
 */
    template <I2CControllerID tkControllerID>
        requires(kValidI2CControllerID<tkControllerID>)
    static inline void i2c_error_irq_handler()
    {
        using GlobalRouterT               = I2CGlobalErrorISRRouter<tkControllerID>;
        constexpr bool kHasGlobalRouter   = CBoundISRRouter<GlobalRouterT>;
        constexpr bool kHasGranularRouter = i2c_has_any_granular_isr_handler<tkControllerID>();
        static_assert(!(kHasGlobalRouter && kHasGranularRouter), "VALLE CONFLICT: Global and Granular ISRs detected.");

        if constexpr (kHasGlobalRouter)
        {
            GlobalRouterT::handle();
            return;
        }

#define HANDLE_I2C_ERR_INT(tkIntType)                                                 \
    {                                                                                 \
        using RouterT = I2CISRRouter<tkControllerID, I2CInterruptType::kError>;       \
        using TraitsT = I2CInterruptTraits<tkControllerID, I2CInterruptType::kError>; \
        if constexpr (CBoundISRRouter<RouterT>)                                       \
        {                                                                             \
            if (TraitsT::template is_pending<tkIntType>())                            \
            {                                                                         \
                if constexpr (kISRRouterConfigAck<RouterT>)                           \
                {                                                                     \
                    TraitsT::template ack<tkIntType>();                               \
                }                                                                     \
                RouterT::handle(tkIntType);                                           \
            }                                                                         \
        }                                                                             \
    }

        HANDLE_I2C_ERR_INT(I2CErrorInterruptType::kArbitrationLoss);
        HANDLE_I2C_ERR_INT(I2CErrorInterruptType::kBusError);
        HANDLE_I2C_ERR_INT(I2CErrorInterruptType::kOverrunUnderrun);
        HANDLE_I2C_ERR_INT(I2CErrorInterruptType::kSMBusTimeout);
        HANDLE_I2C_ERR_INT(I2CErrorInterruptType::kSMBusPecError);
        HANDLE_I2C_ERR_INT(I2CErrorInterruptType::kSMBusAlert);
    }

    extern "C"
    {
        // --- I2C 1 ---
        void I2C1_EV_IRQHandler(void)
        {
            i2c_event_irq_handler<1>();
        }
        void I2C1_ER_IRQHandler(void)
        {
            i2c_error_irq_handler<1>();
        }

        // --- I2C 2 ---
        void I2C2_EV_IRQHandler(void)
        {
            i2c_event_irq_handler<2>();
        }
        void I2C2_ER_IRQHandler(void)
        {
            i2c_error_irq_handler<2>();
        }

        // --- I2C 3 ---
        void I2C3_EV_IRQHandler(void)
        {
            i2c_event_irq_handler<3>();
        }
        void I2C3_ER_IRQHandler(void)
        {
            i2c_error_irq_handler<3>();
        }

        // --- I2C 4 ---
        void I2C4_EV_IRQHandler(void)
        {
            i2c_event_irq_handler<4>();
        }
        void I2C4_ER_IRQHandler(void)
        {
            i2c_error_irq_handler<4>();
        }
    }
}  // namespace valle