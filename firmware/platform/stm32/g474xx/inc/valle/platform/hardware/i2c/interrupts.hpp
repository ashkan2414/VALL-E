#pragma once

#include "valle/platform/hardware/i2c/interface.hpp"

namespace valle::platform
{
    // ============================================================================
    // I2C INTERRUPT TRAITS
    // ===========================================================================

    enum class I2CInterruptType : uint8_t
    {
        kTx = 0,
        kRx,
        kAddressMatch,
        kNACKReceived,
        kStopDetection,
        kTransferComplete,
        kError
    };

    template <I2CControllerID tkControllerID, I2CInterruptType tkIntType>
        requires(kValidI2CControllerID<tkControllerID>)
    struct I2CInterruptTraits;

    // ----------------------------------------------------------------------------
    // I2C ERROR INTERRUPT TRAITS
    // ----------------------------------------------------------------------------

    enum class I2CErrorInterruptType : uint8_t
    {
        kArbitrationLoss = 0,
        kBusError,
        kOverrunUnderrun,
        kSMBusTimeout,
        kSMBusPecError,
        kSMBusAlert
    };

    template <I2CControllerID tkControllerID, I2CErrorInterruptType tkIntType>
        requires(kValidI2CControllerID<tkControllerID>)
    struct I2CErrorInterruptTraits;

#define DEFINE_I2C_ERROR_INT_TRAIT(tkIntType, flag_id, ll_name)                                                 \
    template <I2CControllerID tkControllerID>                                                                   \
        requires(kValidI2CControllerID<tkControllerID>)                                                         \
    struct I2CErrorInterruptTraits<tkControllerID, (tkIntType)>                                                 \
    {                                                                                                           \
        static inline bool flag_active()                                                                        \
        {                                                                                                       \
            return LL_I2C_IsActive##flag_id##_##ll_name(I2CControllerTraits<tkControllerID>::skInstance);       \
        }                                                                                                       \
                                                                                                                \
        static inline bool is_pending()                                                                         \
        {                                                                                                       \
            return flag_active() && I2CInterruptTraits<tkControllerID, I2CInterruptType::kError>::is_enabled(); \
        }                                                                                                       \
        static inline void ack()                                                                                \
        {                                                                                                       \
            LL_I2C_Clear##flag_id##_##ll_name(I2CControllerTraits<tkControllerID>::skInstance);                 \
        }                                                                                                       \
    };

    DEFINE_I2C_ERROR_INT_TRAIT(I2CErrorInterruptType::kArbitrationLoss, Flag, ARLO);
    DEFINE_I2C_ERROR_INT_TRAIT(I2CErrorInterruptType::kBusError, Flag, BERR);
    DEFINE_I2C_ERROR_INT_TRAIT(I2CErrorInterruptType::kOverrunUnderrun, Flag, OVR);
    DEFINE_I2C_ERROR_INT_TRAIT(I2CErrorInterruptType::kSMBusTimeout, SMBusFlag, TIMEOUT);
    DEFINE_I2C_ERROR_INT_TRAIT(I2CErrorInterruptType::kSMBusPecError, SMBusFlag, PECERR);
    DEFINE_I2C_ERROR_INT_TRAIT(I2CErrorInterruptType::kSMBusAlert, SMBusFlag, ALERT);

#undef DEFINE_I2C_ERROR_INT_TRAIT

#define DEFINE_I2C_INT_TRAIT(tkIntType, ll_name)                                                   \
    template <I2CControllerID tkControllerID>                                                      \
        requires(kValidI2CControllerID<tkControllerID>)                                            \
    struct I2CInterruptTraits<tkControllerID, (tkIntType)>                                         \
    {                                                                                              \
        static inline void enable()                                                                \
        {                                                                                          \
            LL_I2C_EnableIT_##ll_name(I2CControllerTraits<tkControllerID>::skInstance);            \
        }                                                                                          \
        static inline void disable()                                                               \
        {                                                                                          \
            LL_I2C_DisableIT_##ll_name(I2CControllerTraits<tkControllerID>::skInstance);           \
        }                                                                                          \
        static inline bool is_enabled()                                                            \
        {                                                                                          \
            return LL_I2C_IsEnabledIT_##ll_name(I2CControllerTraits<tkControllerID>::skInstance);  \
        }                                                                                          \
        static inline bool flag_active()                                                           \
        {                                                                                          \
            return LL_I2C_IsActiveFlag_##ll_name(I2CControllerTraits<tkControllerID>::skInstance); \
        }                                                                                          \
                                                                                                   \
        static inline bool is_pending()                                                            \
        {                                                                                          \
            return flag_active() && is_enabled();                                                  \
        }                                                                                          \
        static inline void ack()                                                                   \
        {                                                                                          \
            if constexpr (tkIntType != I2CInterruptType::kTransferComplete)                        \
            {                                                                                      \
                LL_I2C_ClearFlag_##ll_name(I2CControllerTraits<tkControllerID>::skInstance);       \
            }                                                                                      \
        }                                                                                          \
    };

    DEFINE_I2C_INT_TRAIT(I2CInterruptType::kTx, TX);
    DEFINE_I2C_INT_TRAIT(I2CInterruptType::kRx, RX);
    DEFINE_I2C_INT_TRAIT(I2CInterruptType::kAddressMatch, ADDR);
    DEFINE_I2C_INT_TRAIT(I2CInterruptType::kNACKReceived, NACK);
    DEFINE_I2C_INT_TRAIT(I2CInterruptType::kStopDetection, STOP);
    DEFINE_I2C_INT_TRAIT(I2CInterruptType::kTransferComplete, TC);

#undef DEFINE_I2C_INT_TRAIT

    // Specialization for Error Interrupts
    template <I2CControllerID tkControllerID>
        requires(kValidI2CControllerID<tkControllerID>)
    struct I2CInterruptTraits<tkControllerID, I2CInterruptType::kError>
    {
        static inline void enable()
        {
            LL_I2C_EnableIT_ERR(I2CControllerTraits<tkControllerID>::skInstance);
        }

        static inline void disable()
        {
            LL_I2C_DisableIT_ERR(I2CControllerTraits<tkControllerID>::skInstance);
        }

        static inline bool is_enabled()
        {
            return LL_I2C_IsEnabledIT_ERR(I2CControllerTraits<tkControllerID>::skInstance);
        }

        template <I2CErrorInterruptType tkErrorIntType>
        static inline bool flag_active()
        {
            return I2CErrorInterruptTraits<tkControllerID, tkErrorIntType>::flag_active();
        }

        template <I2CErrorInterruptType tkErrorIntType>
        static inline bool is_pending()
        {
            return I2CErrorInterruptTraits<tkControllerID, tkErrorIntType>::is_pending();
        }

        template <I2CErrorInterruptType tkErrorIntType>
        static inline void ack()
        {
            I2CErrorInterruptTraits<tkControllerID, tkErrorIntType>::ack();
        }
    };

    // ============================================================================
    // INTERRUPT CONTROLLER
    // ============================================================================

    struct I2CInterruptMask
    {
        union
        {
            uint32_t mask;
            struct
            {
                bool event_tx : 1         = false;  // TX Event Interrupt Enable
                bool event_rx : 1         = false;  // RX Event Interrupt Enable
                bool event_addr_match : 1 = false;  // Address Match Interrupt Enable
                bool event_nack : 1       = false;  // NACK Received Interrupt Enable
                bool event_stop : 1       = false;  // STOP Detection Interrupt Enable
                bool event_tc : 1         = false;  // Transfer Complete Interrupt Enable
                bool error : 1            = false;  // Error Interrupt Enable
            };
        };

        constexpr bool any_event_enabled() const
        {
            constexpr auto event_mask = I2CInterruptMask{.event_tx         = true,
                                                         .event_rx         = true,
                                                         .event_addr_match = true,
                                                         .event_nack       = true,
                                                         .event_stop       = true,
                                                         .event_tc         = true};
            return (mask & event_mask.mask) != 0;
        }
    };

    /**
     * @brief Configuration for I2C Interrupts
     */
    struct I2CInterruptConfig
    {
        uint8_t          event_priority = 5;  // NVIC Priority  // NOLINT(readability-magic-numbers)
        uint8_t          error_priority = 5;  // NVIC Priority  // NOLINT(readability-magic-numbers)
        I2CInterruptMask interrupts{};        // Which interrupts to enable
    };

    template <I2CControllerID tkControllerID>
        requires(kValidI2CControllerID<tkControllerID>)
    class I2CInterruptController
    {
    public:
        static constexpr I2CControllerID skControllerID = tkControllerID;
        using ControllerTraitsT                         = I2CControllerTraits<tkControllerID>;

        static void enable_interrupts(const I2CInterruptConfig& config)
        {
            if (config.interrupts.any_event_enabled())
            {
                if (config.interrupts.event_tx)
                {
                    I2CInterruptTraits<tkControllerID, I2CInterruptType::kTx>::enable();
                }

                if (config.interrupts.event_rx)
                {
                    I2CInterruptTraits<tkControllerID, I2CInterruptType::kRx>::enable();
                }

                if (config.interrupts.event_addr_match)
                {
                    I2CInterruptTraits<tkControllerID, I2CInterruptType::kAddressMatch>::enable();
                }

                if (config.interrupts.event_nack)
                {
                    I2CInterruptTraits<tkControllerID, I2CInterruptType::kNACKReceived>::enable();
                }

                if (config.interrupts.event_stop)
                {
                    I2CInterruptTraits<tkControllerID, I2CInterruptType::kStopDetection>::enable();
                }

                if (config.interrupts.event_tc)
                {
                    I2CInterruptTraits<tkControllerID, I2CInterruptType::kTransferComplete>::enable();
                }

                NVIC_SetPriority(ControllerTraitsT::skEventIRQn, config.event_priority);
                NVIC_EnableIRQ(ControllerTraitsT::skEventIRQn);
            }

            if (config.interrupts.error)
            {
                // All error interrupts are enabled together
                I2CInterruptTraits<tkControllerID, I2CInterruptType::kError>::enable();
                NVIC_SetPriority(ControllerTraitsT::skErrorIRQn, config.error_priority);
                NVIC_EnableIRQ(ControllerTraitsT::skErrorIRQn);
            }
        }

        static void disable_interrupts()
        {
            I2CInterruptTraits<tkControllerID, I2CInterruptType::kTx>::disable();
            I2CInterruptTraits<tkControllerID, I2CInterruptType::kRx>::disable();
            I2CInterruptTraits<tkControllerID, I2CInterruptType::kAddressMatch>::disable();
            I2CInterruptTraits<tkControllerID, I2CInterruptType::kNACKReceived>::disable();
            I2CInterruptTraits<tkControllerID, I2CInterruptType::kStopDetection>::disable();
            I2CInterruptTraits<tkControllerID, I2CInterruptType::kTransferComplete>::disable();
            I2CInterruptTraits<tkControllerID, I2CInterruptType::kError>::disable();

            NVIC_DisableIRQ(ControllerTraitsT::skEventIRQn);
            NVIC_DisableIRQ(ControllerTraitsT::skErrorIRQn);
        }
    };

    // ===========================================================================
    // ISR ROUTERS
    // ===========================================================================

    // ---------------------------------------------------------------------------
    // GLOBAL ISR ROUTERS
    // ---------------------------------------------------------------------------

    /**
     * @brief Global Event ISR Router for a specific I2C.
     * Specializing this allows you to handle the entire Event ISR in one function
     * (e.g., when delegating to the ST HAL).
     *
     * @tparam tkControllerID I2C Controller ID.
     */
    template <I2CControllerID tkControllerID>
        requires(kValidI2CControllerID<tkControllerID>)
    struct I2CGlobalEventISRRouter
    {
        using UnboundIsrHandlerTag = void;
        static void handle()
        {
            // Default: Do nothing (Optimized away)
        }
    };

    /**
     * @brief Global Error ISR Router for a specific I2C.
     * Specializing this allows you to handle the entire Error ISR in one function
     * (e.g., when delegating to the ST HAL).
     *
     * @tparam tkControllerID I2C Controller ID.
     */
    template <I2CControllerID tkControllerID>
        requires(kValidI2CControllerID<tkControllerID>)
    struct I2CGlobalErrorISRRouter
    {
        using UnboundIsrHandlerTag = void;
        static void handle()
        {
            // Default: Do nothing (Optimized away)
        }
    };

    // ---------------------------------------------------------------------------
    // GRANULAR ISR ROUTER
    // ---------------------------------------------------------------------------

    /**
     * @brief I2C ISR Router
     *
     * Specialize this template in your application or driver to bind
     * logic to specific I2C interrupts.
     *
     * @tparam tkControllerID I2C Controller ID.
     * @tparam tkIntType      I2C Interrupt Type.
     */
    template <I2CControllerID tkControllerID, I2CInterruptType tkIntType>
        requires(kValidI2CControllerID<tkControllerID>)
    struct I2CISRRouter
    {
        using UnboundIsrHandlerTag = void;
        static void handle()
            requires(tkIntType != I2CInterruptType::kError)
        {
            // Default: Do nothing (Optimized away)
        }

        static void handle(I2CErrorInterruptType error)
            requires(tkIntType == I2CInterruptType::kError)
        {
            // Default: Do nothing (Optimized away)
        }
    };

}  // namespace valle::platform