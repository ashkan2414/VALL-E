#pragma once

#include "valle/platform/hardware/i2c/interface.hpp"

namespace valle::platform
{
    // ============================================================================
    // I2C INTERRUPT TRAITS
    // ===========================================================================

    enum class I2cInterruptSource : uint8_t
    {
        kTx = 0,
        kRx,
        kAddressMatch,
        kNACKReceived,
        kStopDetection,
        kTransferComplete,
        kError
    };

    template <I2cControllerId tkControllerId, I2cInterruptSource tkIntSource>

    struct I2cInterruptSourceInterface;

    // ----------------------------------------------------------------------------
    // I2C ERROR INTERRUPT TRAITS
    // ----------------------------------------------------------------------------

    enum class I2cErrorInterruptSource : uint8_t
    {
        kArbitrationLoss = 0,
        kBusError,
        kOverrunUnderrun,
        kSMBusTimeout,
        kSMBusPecError,
        kSMBusAlert
    };

    template <I2cControllerId tkControllerId, I2cErrorInterruptSource tkIntSource>

    struct I2cErrorInterruptSourceInterface;

#define DEFINE_I2C_ERROR_INT_TRAIT(tkIntSource, flag_id, ll_name)                                         \
    template <I2cControllerId tkControllerId>                                                             \
    struct I2cErrorInterruptSourceInterface<tkControllerId, (tkIntSource)>                                \
    {                                                                                                     \
        static inline bool flag_active()                                                                  \
        {                                                                                                 \
            return LL_I2C_IsActive##flag_id##_##ll_name(I2cControllerTraits<tkControllerId>::skInstance); \
        }                                                                                                 \
                                                                                                          \
        static inline bool is_pending()                                                                   \
        {                                                                                                 \
            return flag_active() &&                                                                       \
                   I2cInterruptSourceInterface<tkControllerId, I2cInterruptSource::kError>::is_enabled(); \
        }                                                                                                 \
        static inline void clear()                                                                        \
        {                                                                                                 \
            LL_I2C_Clear##flag_id##_##ll_name(I2cControllerTraits<tkControllerId>::skInstance);           \
        }                                                                                                 \
    };

    DEFINE_I2C_ERROR_INT_TRAIT(I2cErrorInterruptSource::kArbitrationLoss, Flag, ARLO);
    DEFINE_I2C_ERROR_INT_TRAIT(I2cErrorInterruptSource::kBusError, Flag, BERR);
    DEFINE_I2C_ERROR_INT_TRAIT(I2cErrorInterruptSource::kOverrunUnderrun, Flag, OVR);
    DEFINE_I2C_ERROR_INT_TRAIT(I2cErrorInterruptSource::kSMBusTimeout, SMBusFlag, TimEOUT);
    DEFINE_I2C_ERROR_INT_TRAIT(I2cErrorInterruptSource::kSMBusPecError, SMBusFlag, PECERR);
    DEFINE_I2C_ERROR_INT_TRAIT(I2cErrorInterruptSource::kSMBusAlert, SMBusFlag, ALERT);

#undef DEFINE_I2C_ERROR_INT_TRAIT

#define DEFINE_I2C_INT_TRAIT(tkIntSource, ll_name)                                                 \
    template <I2cControllerId tkControllerId>                                                      \
    struct I2cInterruptSourceInterface<tkControllerId, (tkIntSource)>                              \
    {                                                                                              \
        static inline void enable()                                                                \
        {                                                                                          \
            LL_I2C_EnableIT_##ll_name(I2cControllerTraits<tkControllerId>::skInstance);            \
        }                                                                                          \
        static inline void disable()                                                               \
        {                                                                                          \
            LL_I2C_DisableIT_##ll_name(I2cControllerTraits<tkControllerId>::skInstance);           \
        }                                                                                          \
        static inline bool is_enabled()                                                            \
        {                                                                                          \
            return LL_I2C_IsEnabledIT_##ll_name(I2cControllerTraits<tkControllerId>::skInstance);  \
        }                                                                                          \
        static inline bool flag_active()                                                           \
        {                                                                                          \
            return LL_I2C_IsActiveFlag_##ll_name(I2cControllerTraits<tkControllerId>::skInstance); \
        }                                                                                          \
                                                                                                   \
        static inline bool is_pending()                                                            \
        {                                                                                          \
            return flag_active() && is_enabled();                                                  \
        }                                                                                          \
        static inline void clear()                                                                 \
        {                                                                                          \
            if constexpr (tkIntSource != I2cInterruptSource::kTransferComplete)                    \
            {                                                                                      \
                LL_I2C_ClearFlag_##ll_name(I2cControllerTraits<tkControllerId>::skInstance);       \
            }                                                                                      \
        }                                                                                          \
    };

    DEFINE_I2C_INT_TRAIT(I2cInterruptSource::kTx, TX);
    DEFINE_I2C_INT_TRAIT(I2cInterruptSource::kRx, RX);
    DEFINE_I2C_INT_TRAIT(I2cInterruptSource::kAddressMatch, ADDR);
    DEFINE_I2C_INT_TRAIT(I2cInterruptSource::kNACKReceived, NACK);
    DEFINE_I2C_INT_TRAIT(I2cInterruptSource::kStopDetection, STOP);
    DEFINE_I2C_INT_TRAIT(I2cInterruptSource::kTransferComplete, TC);

#undef DEFINE_I2C_INT_TRAIT

    // Specialization for Error Interrupts
    template <I2cControllerId tkControllerId>
    struct I2cInterruptSourceInterface<tkControllerId, I2cInterruptSource::kError>
    {
        static inline void enable()
        {
            LL_I2C_EnableIT_ERR(I2cControllerTraits<tkControllerId>::skInstance);
        }

        static inline void disable()
        {
            LL_I2C_DisableIT_ERR(I2cControllerTraits<tkControllerId>::skInstance);
        }

        static inline bool is_enabled()
        {
            return LL_I2C_IsEnabledIT_ERR(I2cControllerTraits<tkControllerId>::skInstance);
        }

        template <I2cErrorInterruptSource tkErrorIntType>
        static inline bool flag_active()
        {
            return I2cErrorInterruptSourceInterface<tkControllerId, tkErrorIntType>::flag_active();
        }

        template <I2cErrorInterruptSource tkErrorIntType>
        static inline bool is_pending()
        {
            return I2cErrorInterruptSourceInterface<tkControllerId, tkErrorIntType>::is_pending();
        }

        template <I2cErrorInterruptSource tkErrorIntType>
        static inline void clear()
        {
            I2cErrorInterruptSourceInterface<tkControllerId, tkErrorIntType>::clear();
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

    template <I2cControllerId tkControllerId>

    class I2cInterruptController
    {
    public:
        static constexpr I2cControllerId skControllerId = tkControllerId;
        using ControllerTraitsT                         = I2cControllerTraits<tkControllerId>;

        static void enable_interrupts(const I2cInterruptConfig& config)
        {
            if (config.interrupts.any_event_enabled())
            {
                if (config.interrupts.event_tx)
                {
                    I2cInterruptSourceInterface<tkControllerId, I2cInterruptSource::kTx>::enable();
                }

                if (config.interrupts.event_rx)
                {
                    I2cInterruptSourceInterface<tkControllerId, I2cInterruptSource::kRx>::enable();
                }

                if (config.interrupts.event_addr_match)
                {
                    I2cInterruptSourceInterface<tkControllerId, I2cInterruptSource::kAddressMatch>::enable();
                }

                if (config.interrupts.event_nack)
                {
                    I2cInterruptSourceInterface<tkControllerId, I2cInterruptSource::kNACKReceived>::enable();
                }

                if (config.interrupts.event_stop)
                {
                    I2cInterruptSourceInterface<tkControllerId, I2cInterruptSource::kStopDetection>::enable();
                }

                if (config.interrupts.event_tc)
                {
                    I2cInterruptSourceInterface<tkControllerId, I2cInterruptSource::kTransferComplete>::enable();
                }

                NVIC_SetPriority(ControllerTraitsT::skEventIRQn, config.event_priority);
                NVIC_EnableIRQ(ControllerTraitsT::skEventIRQn);
            }

            if (config.interrupts.error)
            {
                // All error interrupts are enabled together
                I2cInterruptSourceInterface<tkControllerId, I2cInterruptSource::kError>::enable();
                NVIC_SetPriority(ControllerTraitsT::skErrorIRQn, config.error_priority);
                NVIC_EnableIRQ(ControllerTraitsT::skErrorIRQn);
            }
        }

        static void disable_interrupts()
        {
            I2cInterruptSourceInterface<tkControllerId, I2cInterruptSource::kTx>::disable();
            I2cInterruptSourceInterface<tkControllerId, I2cInterruptSource::kRx>::disable();
            I2cInterruptSourceInterface<tkControllerId, I2cInterruptSource::kAddressMatch>::disable();
            I2cInterruptSourceInterface<tkControllerId, I2cInterruptSource::kNACKReceived>::disable();
            I2cInterruptSourceInterface<tkControllerId, I2cInterruptSource::kStopDetection>::disable();
            I2cInterruptSourceInterface<tkControllerId, I2cInterruptSource::kTransferComplete>::disable();
            I2cInterruptSourceInterface<tkControllerId, I2cInterruptSource::kError>::disable();

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
     * @tparam tkControllerId I2C Controller ID.
     */
    template <I2cControllerId tkControllerId>

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
     * @tparam tkControllerId I2C Controller ID.
     */
    template <I2cControllerId tkControllerId>

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
     * @tparam tkControllerId I2C Controller ID.
     * @tparam tkIntSource      I2C Interrupt Type.
     */
    template <I2cControllerId tkControllerId, I2cInterruptSource tkIntSource>

    struct I2cIsrRouter
    {
        using UnboundIsrHandlerTag = void;
        static void handle()
            requires(tkIntSource != I2cInterruptSource::kError)
        {
            // Default: Do nothing (Optimized away)
        }

        static void handle(I2cErrorInterruptSource error)
            requires(tkIntSource == I2cInterruptSource::kError)
        {
            // Default: Do nothing (Optimized away)
        }
    };

}  // namespace valle::platform
