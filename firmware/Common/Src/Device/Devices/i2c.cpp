#include "Valle/Device/Devices/i2c.hpp"

#if __has_include("app_isr_bindings.hpp")
#include "app_isr_bindings.hpp"
#endif

namespace valle
{

    /**
     * @brief I2C Event Interrupt Handler Router
     *
     * @tparam tkControllerID I2C Index (1-4)
     */
    template <I2CControllerID tkControllerID>
        requires(kValidI2CControllerID<tkControllerID>)
    static inline void i2c_event_irq_handler()
    {
#define HANDLE_I2C_INT(tkIntType)                                               \
    if constexpr (CBoundIsrHandler<I2CIsrRouter<tkControllerID, (tkIntType)>>)  \
    {                                                                           \
        if (I2CInterruptTraits<tkControllerID, tkIntType>::is_pending())        \
        {                                                                       \
            /* Note: Some I2C flags (like TXIS/RXNE) are cleared by hardware */ \
            /* via register access, but we call ack() per traits definition. */ \
            I2CInterruptTraits<tkControllerID, tkIntType>::ack();               \
            I2CIsrRouter<tkControllerID, tkIntType>::handle();                  \
        }                                                                       \
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
#define HANDLE_I2C_ERR_INT(tkIntType)                                                                       \
    if constexpr (CBoundIsrHandler<I2CIsrRouter<tkControllerID, I2CInterruptType::kError>>)                 \
    {                                                                                                       \
        if (I2CInterruptTraits<tkControllerID, I2CInterruptType::kError>::template is_pending<tkIntType>()) \
        {                                                                                                   \
            I2CInterruptTraits<tkControllerID, I2CInterruptType::kError>::template ack<tkIntType>();        \
            I2CIsrRouter<tkControllerID, I2CInterruptType::kError>::handle(tkIntType);                      \
        }                                                                                                   \
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