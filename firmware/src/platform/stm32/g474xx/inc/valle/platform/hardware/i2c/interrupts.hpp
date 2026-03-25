#pragma once

#include "valle/platform/hardware/i2c/interface.hpp"

namespace valle::platform
{
    // ============================================================================
    // I2C INTERRUPT TRAITS
    // ===========================================================================

    enum class I2cInterruptType : uint8_t
    {
        kTx = 0,
        kRx,
        kAddressMatch,
        kNACKReceived,
        kStopDetection,
        kTransferComplete,
        kError
    };

    template <I2cPeripheralId tkPeripheralId, I2cInterruptType tkIntType>

    struct I2cInterruptTraits;

    // ----------------------------------------------------------------------------
    // I2C ERROR INTERRUPT TRAITS
    // ----------------------------------------------------------------------------

    enum class I2cErrorInterruptType : uint8_t
    {
        kArbitrationLoss = 0,
        kBusError,
        kOverrunUnderrun,
        kSMBusTimeout,
        kSMBusPecError,
        kSMBusAlert
    };

    template <I2cPeripheralId tkPeripheralId, I2cErrorInterruptType tkIntType>

    struct I2cErrorInterruptTraits;

#define DEFINE_I2C_ERROR_INT_TRAIT(tkIntType, flag_id, ll_name)                                                 \
    template <I2cPeripheralId tkPeripheralId>                                                                   \
    struct I2cErrorInterruptTraits<tkPeripheralId, (tkIntType)>                                                 \
    {                                                                                                           \
        static inline bool flag_active()                                                                        \
        {                                                                                                       \
            return LL_I2C_IsActive##flag_id##_##ll_name(I2cControllerTraits<tkPeripheralId>::skInstance);       \
        }                                                                                                       \
                                                                                                                \
        static inline bool is_pending()                                                                         \
        {                                                                                                       \
            return flag_active() && I2cInterruptTraits<tkPeripheralId, I2cInterruptType::kError>::is_enabled(); \
        }                                                                                                       \
        static inline void ack()                                                                                \
        {                                                                                                       \
            LL_I2C_Clear##flag_id##_##ll_name(I2cControllerTraits<tkPeripheralId>::skInstance);                 \
        }                                                                                                       \
    };

    DEFINE_I2C_ERROR_INT_TRAIT(I2cErrorInterruptType::kArbitrationLoss, Flag, ARLO);
    DEFINE_I2C_ERROR_INT_TRAIT(I2cErrorInterruptType::kBusError, Flag, BERR);
    DEFINE_I2C_ERROR_INT_TRAIT(I2cErrorInterruptType::kOverrunUnderrun, Flag, OVR);
    DEFINE_I2C_ERROR_INT_TRAIT(I2cErrorInterruptType::kSMBusTimeout, SMBusFlag, TimEOUT);
    DEFINE_I2C_ERROR_INT_TRAIT(I2cErrorInterruptType::kSMBusPecError, SMBusFlag, PECERR);
    DEFINE_I2C_ERROR_INT_TRAIT(I2cErrorInterruptType::kSMBusAlert, SMBusFlag, ALERT);

#undef DEFINE_I2C_ERROR_INT_TRAIT

#define DEFINE_I2C_INT_TRAIT(tkIntType, ll_name)                                                   \
    template <I2cPeripheralId tkPeripheralId>                                                      \
    struct I2cInterruptTraits<tkPeripheralId, (tkIntType)>                                         \
    {                                                                                              \
        static inline void enable()                                                                \
        {                                                                                          \
            LL_I2C_EnableIT_##ll_name(I2cControllerTraits<tkPeripheralId>::skInstance);            \
        }                                                                                          \
        static inline void disable()                                                               \
        {                                                                                          \
            LL_I2C_DisableIT_##ll_name(I2cControllerTraits<tkPeripheralId>::skInstance);           \
        }                                                                                          \
        static inline bool is_enabled()                                                            \
        {                                                                                          \
            return LL_I2C_IsEnabledIT_##ll_name(I2cControllerTraits<tkPeripheralId>::skInstance);  \
        }                                                                                          \
        static inline bool flag_active()                                                           \
        {                                                                                          \
            return LL_I2C_IsActiveFlag_##ll_name(I2cControllerTraits<tkPeripheralId>::skInstance); \
        }                                                                                          \
                                                                                                   \
        static inline bool is_pending()                                                            \
        {                                                                                          \
            return flag_active() && is_enabled();                                                  \
        }                                                                                          \
        static inline void ack()                                                                   \
        {                                                                                          \
            if constexpr (tkIntType != I2cInterruptType::kTransferComplete)                        \
            {                                                                                      \
                LL_I2C_ClearFlag_##ll_name(I2cControllerTraits<tkPeripheralId>::skInstance);       \
            }                                                                                      \
        }                                                                                          \
    };

    DEFINE_I2C_INT_TRAIT(I2cInterruptType::kTx, TX);
    DEFINE_I2C_INT_TRAIT(I2cInterruptType::kRx, RX);
    DEFINE_I2C_INT_TRAIT(I2cInterruptType::kAddressMatch, ADDR);
    DEFINE_I2C_INT_TRAIT(I2cInterruptType::kNACKReceived, NACK);
    DEFINE_I2C_INT_TRAIT(I2cInterruptType::kStopDetection, STOP);
    DEFINE_I2C_INT_TRAIT(I2cInterruptType::kTransferComplete, TC);

#undef DEFINE_I2C_INT_TRAIT

    // Specialization for Error Interrupts
    template <I2cPeripheralId tkPeripheralId>
    struct I2cInterruptTraits<tkPeripheralId, I2cInterruptType::kError>
    {
        static inline void enable()
        {
            LL_I2C_EnableIT_ERR(I2cControllerTraits<tkPeripheralId>::skInstance);
        }

        static inline void disable()
        {
            LL_I2C_DisableIT_ERR(I2cControllerTraits<tkPeripheralId>::skInstance);
        }

        static inline bool is_enabled()
        {
            return LL_I2C_IsEnabledIT_ERR(I2cControllerTraits<tkPeripheralId>::skInstance);
        }

        template <I2cErrorInterruptType tkErrorIntType>
        static inline bool flag_active()
        {
            return I2cErrorInterruptTraits<tkPeripheralId, tkErrorIntType>::flag_active();
        }

        template <I2cErrorInterruptType tkErrorIntType>
        static inline bool is_pending()
        {
            return I2cErrorInterruptTraits<tkPeripheralId, tkErrorIntType>::is_pending();
        }

        template <I2cErrorInterruptType tkErrorIntType>
        static inline void ack()
        {
            I2cErrorInterruptTraits<tkPeripheralId, tkErrorIntType>::ack();
        }
    };

    // ============================================================================
    // INTERRUPT CONTROLLER
    // ============================================================================

    struct I2cInterruptMask
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
            constexpr auto event_mask = I2cInterruptMask{.event_tx         = true,
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
    struct I2cInterruptConfig
    {
        uint8_t          event_priority = 5;  // NVIC Priority  // NOLINT(readability-magic-numbers)
        uint8_t          error_priority = 5;  // NVIC Priority  // NOLINT(readability-magic-numbers)
        I2cInterruptMask interrupts{};        // Which interrupts to enable
    };

    template <I2cPeripheralId tkPeripheralId>

    class I2cInterruptController
    {
    public:
        static constexpr I2cPeripheralId skPeripheralId = tkPeripheralId;
        using ControllerTraitsT                         = I2cControllerTraits<tkPeripheralId>;

        static void enable_interrupts(const I2cInterruptConfig& config)
        {
            if (config.interrupts.any_event_enabled())
            {
                if (config.interrupts.event_tx)
                {
                    I2cInterruptTraits<tkPeripheralId, I2cInterruptType::kTx>::enable();
                }

                if (config.interrupts.event_rx)
                {
                    I2cInterruptTraits<tkPeripheralId, I2cInterruptType::kRx>::enable();
                }

                if (config.interrupts.event_addr_match)
                {
                    I2cInterruptTraits<tkPeripheralId, I2cInterruptType::kAddressMatch>::enable();
                }

                if (config.interrupts.event_nack)
                {
                    I2cInterruptTraits<tkPeripheralId, I2cInterruptType::kNACKReceived>::enable();
                }

                if (config.interrupts.event_stop)
                {
                    I2cInterruptTraits<tkPeripheralId, I2cInterruptType::kStopDetection>::enable();
                }

                if (config.interrupts.event_tc)
                {
                    I2cInterruptTraits<tkPeripheralId, I2cInterruptType::kTransferComplete>::enable();
                }

                NVIC_SetPriority(ControllerTraitsT::skEventIRQn, config.event_priority);
                NVIC_EnableIRQ(ControllerTraitsT::skEventIRQn);
            }

            if (config.interrupts.error)
            {
                // All error interrupts are enabled together
                I2cInterruptTraits<tkPeripheralId, I2cInterruptType::kError>::enable();
                NVIC_SetPriority(ControllerTraitsT::skErrorIRQn, config.error_priority);
                NVIC_EnableIRQ(ControllerTraitsT::skErrorIRQn);
            }
        }

        static void disable_interrupts()
        {
            I2cInterruptTraits<tkPeripheralId, I2cInterruptType::kTx>::disable();
            I2cInterruptTraits<tkPeripheralId, I2cInterruptType::kRx>::disable();
            I2cInterruptTraits<tkPeripheralId, I2cInterruptType::kAddressMatch>::disable();
            I2cInterruptTraits<tkPeripheralId, I2cInterruptType::kNACKReceived>::disable();
            I2cInterruptTraits<tkPeripheralId, I2cInterruptType::kStopDetection>::disable();
            I2cInterruptTraits<tkPeripheralId, I2cInterruptType::kTransferComplete>::disable();
            I2cInterruptTraits<tkPeripheralId, I2cInterruptType::kError>::disable();

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
     * @tparam tkPeripheralId I2C Peripheral ID.
     */
    template <I2cPeripheralId tkPeripheralId>

    struct I2cGlobalEventIsrRouter
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
     * @tparam tkPeripheralId I2C Peripheral ID.
     */
    template <I2cPeripheralId tkPeripheralId>

    struct I2cGlobalErrorIsrRouter
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
     * @tparam tkPeripheralId I2C Peripheral ID.
     * @tparam tkIntType      I2C Interrupt Type.
     */
    template <I2cPeripheralId tkPeripheralId, I2cInterruptType tkIntType>

    struct I2cIsrRouter
    {
        using UnboundIsrHandlerTag = void;
        static void handle()
            requires(tkIntType != I2cInterruptType::kError)
        {
            // Default: Do nothing (Optimized away)
        }

        static void handle(I2cErrorInterruptType error)
            requires(tkIntType == I2cInterruptType::kError)
        {
            // Default: Do nothing (Optimized away)
        }
    };

}  // namespace valle::platform
