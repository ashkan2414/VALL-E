#pragma once

#include "valle/platform/hardware/hrtim/interface.hpp"

namespace valle::platform
{
    // =============================================================================
    // HRTIM CONTROLLER INTERRUPTS
    // =============================================================================

    enum class HrtimControllerInterruptType : uint8_t
    {
        kDLLReady,         // DLLRDY
        kBurstModePeriod,  // BMPER
        kSync,             // SYNC
    };

    template <HrtimPeripheralId tkPeripheralId, HrtimControllerInterruptType tkIntType>
    struct HrtimControllerInterruptTraits;

#define DEFINE_HRTIM_INT_TRAIT(tkIntType, ll_name)                                                     \
    template <HrtimPeripheralId tkPeripheralId>                                                        \
    struct HrtimControllerInterruptTraits<tkPeripheralId, tkIntType>                                   \
    {                                                                                                  \
        static inline void enable()                                                                    \
        {                                                                                              \
            LL_HRTIM_EnableIT_##ll_name(HrtimControllerTraits<tkPeripheralId>::skInstance);            \
        }                                                                                              \
        static inline void disable()                                                                   \
        {                                                                                              \
            LL_HRTIM_DisableIT_##ll_name(HrtimControllerTraits<tkPeripheralId>::skInstance);           \
        }                                                                                              \
        static inline bool is_enabled()                                                                \
        {                                                                                              \
            return LL_HRTIM_IsEnabledIT_##ll_name(HrtimControllerTraits<tkPeripheralId>::skInstance);  \
        }                                                                                              \
        static inline bool flag_active()                                                               \
        {                                                                                              \
            return LL_HRTIM_IsActiveFlag_##ll_name(HrtimControllerTraits<tkPeripheralId>::skInstance); \
        }                                                                                              \
                                                                                                       \
        static inline bool is_pending()                                                                \
        {                                                                                              \
            return flag_active() && is_enabled();                                                      \
        }                                                                                              \
        static inline void ack()                                                                       \
        {                                                                                              \
            LL_HRTIM_ClearFlag_##ll_name(HrtimControllerTraits<tkPeripheralId>::skInstance);           \
        }                                                                                              \
    };

    DEFINE_HRTIM_INT_TRAIT(HrtimControllerInterruptType::kDLLReady, DLLRDY);
    DEFINE_HRTIM_INT_TRAIT(HrtimControllerInterruptType::kBurstModePeriod, BMPER);
    DEFINE_HRTIM_INT_TRAIT(HrtimControllerInterruptType::kSync, SYNC);

#undef DEFINE_HRTIM_INT_TRAIT

    // =============================================================================
    // HRTIM FAULT INTERRUPTS
    // =============================================================================
    enum class HrtimFaultInterruptType : uint8_t
    {
        kSystemFault,  // SYSFLT
        kFault1,       // FLT1
        kFault2,       // FLT2
        kFault3,       // FLT3
        kFault4,       // FLT4
        kFault5,       // FLT5
        kFault6,       // FLT6
    };

    template <HrtimPeripheralId tkPeripheralId, HrtimFaultInterruptType tkIntType>
    struct HrtimFaultInterruptTraits;

#define DEFINE_HRTIM_FAULT_INT_TRAIT(tkIntType, ll_name)                                               \
    template <HrtimPeripheralId tkPeripheralId>                                                        \
    struct HrtimFaultInterruptTraits<tkPeripheralId, tkIntType>                                        \
    {                                                                                                  \
        static inline void enable()                                                                    \
        {                                                                                              \
            LL_HRTIM_EnableIT_##ll_name(HrtimControllerTraits<tkPeripheralId>::skInstance);            \
        }                                                                                              \
        static inline void disable()                                                                   \
        {                                                                                              \
            LL_HRTIM_DisableIT_##ll_name(HrtimControllerTraits<tkPeripheralId>::skInstance);           \
        }                                                                                              \
        static inline bool is_enabled()                                                                \
        {                                                                                              \
            return LL_HRTIM_IsEnabledIT_##ll_name(HrtimControllerTraits<tkPeripheralId>::skInstance);  \
        }                                                                                              \
        static inline bool flag_active()                                                               \
        {                                                                                              \
            return LL_HRTIM_IsActiveFlag_##ll_name(HrtimControllerTraits<tkPeripheralId>::skInstance); \
        }                                                                                              \
        static inline bool is_pending()                                                                \
        {                                                                                              \
            return flag_active() && is_enabled();                                                      \
        }                                                                                              \
        static inline void ack()                                                                       \
        {                                                                                              \
            LL_HRTIM_ClearFlag_##ll_name(HrtimControllerTraits<tkPeripheralId>::skInstance);           \
        }                                                                                              \
    };

    DEFINE_HRTIM_FAULT_INT_TRAIT(HrtimFaultInterruptType::kSystemFault, SYSFLT);
    DEFINE_HRTIM_FAULT_INT_TRAIT(HrtimFaultInterruptType::kFault1, FLT1);
    DEFINE_HRTIM_FAULT_INT_TRAIT(HrtimFaultInterruptType::kFault2, FLT2);
    DEFINE_HRTIM_FAULT_INT_TRAIT(HrtimFaultInterruptType::kFault3, FLT3);
    DEFINE_HRTIM_FAULT_INT_TRAIT(HrtimFaultInterruptType::kFault4, FLT4);
    DEFINE_HRTIM_FAULT_INT_TRAIT(HrtimFaultInterruptType::kFault5, FLT5);
    DEFINE_HRTIM_FAULT_INT_TRAIT(HrtimFaultInterruptType::kFault6, FLT6);

#undef DEFINE_HRTIM_FAULT_INT_TRAIT

    // =============================================================================
    // HRTIM MASTER TIMER INTERRUPTS
    // =============================================================================

    enum class HrtimMasterTimerInterruptType : uint8_t
    {
        kCompare1,    // CMP1
        kCompare2,    // CMP2
        kCompare3,    // CMP3
        kCompare4,    // CMP4
        kUpdate,      // UPDATE
        kRepetition,  // REP
    };

    template <HrtimPeripheralId tkPeripheralId, HrtimMasterTimerInterruptType tkIntType>
    struct HrtimMasterTimerInterruptTraits;

#define DEFINE_HRTIM_INT_TRAIT(tkIntType, ll_name)                                                                  \
    template <HrtimPeripheralId tkPeripheralId>                                                                     \
    struct HrtimMasterTimerInterruptTraits<tkPeripheralId, tkIntType>                                               \
    {                                                                                                               \
        static inline void enable()                                                                                 \
        {                                                                                                           \
            LL_HRTIM_EnableIT_##ll_name(HrtimControllerTraits<tkPeripheralId>::skInstance, LL_HRTIM_TIMER_MASTER);  \
        }                                                                                                           \
        static inline void disable()                                                                                \
        {                                                                                                           \
            LL_HRTIM_DisableIT_##ll_name(HrtimControllerTraits<tkPeripheralId>::skInstance, LL_HRTIM_TIMER_MASTER); \
        }                                                                                                           \
        static inline bool is_enabled()                                                                             \
        {                                                                                                           \
            return LL_HRTIM_IsEnabledIT_##ll_name(HrtimControllerTraits<tkPeripheralId>::skInstance,                \
                                                  LL_HRTIM_TIMER_MASTER);                                           \
        }                                                                                                           \
        static inline bool flag_active()                                                                            \
        {                                                                                                           \
            return LL_HRTIM_IsActiveFlag_##ll_name(HrtimControllerTraits<tkPeripheralId>::skInstance,               \
                                                   LL_HRTIM_TIMER_MASTER);                                          \
        }                                                                                                           \
                                                                                                                    \
        static inline bool is_pending()                                                                             \
        {                                                                                                           \
            return flag_active() && is_enabled();                                                                   \
        }                                                                                                           \
        static inline void ack()                                                                                    \
        {                                                                                                           \
            LL_HRTIM_ClearFlag_##ll_name(HrtimControllerTraits<tkPeripheralId>::skInstance, LL_HRTIM_TIMER_MASTER); \
        }                                                                                                           \
    };

    DEFINE_HRTIM_INT_TRAIT(HrtimMasterTimerInterruptType::kCompare1, CMP1);
    DEFINE_HRTIM_INT_TRAIT(HrtimMasterTimerInterruptType::kCompare2, CMP2);
    DEFINE_HRTIM_INT_TRAIT(HrtimMasterTimerInterruptType::kCompare3, CMP3);
    DEFINE_HRTIM_INT_TRAIT(HrtimMasterTimerInterruptType::kCompare4, CMP4);
    DEFINE_HRTIM_INT_TRAIT(HrtimMasterTimerInterruptType::kUpdate, UPDATE);
    DEFINE_HRTIM_INT_TRAIT(HrtimMasterTimerInterruptType::kRepetition, REP);

#undef DEFINE_HRTIM_INT_TRAIT

    // =============================================================================
    // HRTIM TIMER INTERRUPTS
    // =============================================================================

    enum class HrtimTimerInterruptType : uint8_t
    {
        kCompare1,           // CMP1
        kCompare2,           // CMP2
        kCompare3,           // CMP3
        kCompare4,           // CMP4
        kUpdate,             // UPDATE
        kRepetition,         // REP
        kCapture1,           // CPT1
        kCapture2,           // CPT2
        kOutput1Set,         // SET1
        kOutput1Reset,       // RST1
        kOutput2Set,         // SET2
        kOutput2Reset,       // RST2
        kReset,              // RST
        kDelayedProtection,  // DLYPRT
    };

    template <HrtimPeripheralId tkPeripheralId, HrtimTimerId tkTimerId, HrtimTimerInterruptType tkIntType>
    struct HrtimTimerInterruptTraits;

#define DEFINE_HRTIM_INT_TRAIT(tkIntType, ll_name)                                                       \
    template <HrtimPeripheralId tkPeripheralId, HrtimTimerId tkTimerId>                                  \
    struct HrtimTimerInterruptTraits<tkPeripheralId, tkTimerId, tkIntType>                               \
    {                                                                                                    \
        static inline void enable()                                                                      \
        {                                                                                                \
            LL_HRTIM_EnableIT_##ll_name(HrtimControllerTraits<tkPeripheralId>::skInstance,               \
                                        HrtimTimerTraits<tkPeripheralId, tkTimerId>::skLLId);            \
        }                                                                                                \
        static inline void disable()                                                                     \
        {                                                                                                \
            LL_HRTIM_DisableIT_##ll_name(HrtimControllerTraits<tkPeripheralId>::skInstance,              \
                                         HrtimTimerTraits<tkPeripheralId, tkTimerId>::skLLId);           \
        }                                                                                                \
        static inline bool is_enabled()                                                                  \
        {                                                                                                \
            return LL_HRTIM_IsEnabledIT_##ll_name(HrtimControllerTraits<tkPeripheralId>::skInstance,     \
                                                  HrtimTimerTraits<tkPeripheralId, tkTimerId>::skLLId);  \
        }                                                                                                \
        static inline bool flag_active()                                                                 \
        {                                                                                                \
            return LL_HRTIM_IsActiveFlag_##ll_name(HrtimControllerTraits<tkPeripheralId>::skInstance,    \
                                                   HrtimTimerTraits<tkPeripheralId, tkTimerId>::skLLId); \
        }                                                                                                \
                                                                                                         \
        static inline bool is_pending()                                                                  \
        {                                                                                                \
            return flag_active() && is_enabled();                                                        \
        }                                                                                                \
        static inline void ack()                                                                         \
        {                                                                                                \
            LL_HRTIM_ClearFlag_##ll_name(HrtimControllerTraits<tkPeripheralId>::skInstance,              \
                                         HrtimTimerTraits<tkPeripheralId, tkTimerId>::skLLId);           \
        }                                                                                                \
    };

    DEFINE_HRTIM_INT_TRAIT(HrtimTimerInterruptType::kCompare1, CMP1);
    DEFINE_HRTIM_INT_TRAIT(HrtimTimerInterruptType::kCompare2, CMP2);
    DEFINE_HRTIM_INT_TRAIT(HrtimTimerInterruptType::kCompare3, CMP3);
    DEFINE_HRTIM_INT_TRAIT(HrtimTimerInterruptType::kCompare4, CMP4);
    DEFINE_HRTIM_INT_TRAIT(HrtimTimerInterruptType::kUpdate, UPDATE);
    DEFINE_HRTIM_INT_TRAIT(HrtimTimerInterruptType::kRepetition, REP);
    DEFINE_HRTIM_INT_TRAIT(HrtimTimerInterruptType::kCapture1, CPT1);
    DEFINE_HRTIM_INT_TRAIT(HrtimTimerInterruptType::kCapture2, CPT2);
    DEFINE_HRTIM_INT_TRAIT(HrtimTimerInterruptType::kOutput1Set, SET1);
    DEFINE_HRTIM_INT_TRAIT(HrtimTimerInterruptType::kOutput1Reset, RST1);
    DEFINE_HRTIM_INT_TRAIT(HrtimTimerInterruptType::kOutput2Set, SET2);
    DEFINE_HRTIM_INT_TRAIT(HrtimTimerInterruptType::kOutput2Reset, RST2);
    DEFINE_HRTIM_INT_TRAIT(HrtimTimerInterruptType::kReset, RST);
    DEFINE_HRTIM_INT_TRAIT(HrtimTimerInterruptType::kDelayedProtection, DLYPRT);

#undef DEFINE_HRTIM_INT_TRAIT

    // ===========================================================================
    // INTERRUPT CONTROLLER
    // ===========================================================================

    // -----------------------------------------------------------------------------
    // TIMER INTERRUPT CONTROLLER
    // -----------------------------------------------------------------------------
    struct HrtimTimerInterruptMask
    {
        union
        {
            uint32_t mask;
            struct
            {
                bool compare1 : 1           = false;
                bool compare2 : 1           = false;
                bool compare3 : 1           = false;
                bool compare4 : 1           = false;
                bool update : 1             = false;
                bool repetition : 1         = false;
                bool capture1 : 1           = false;
                bool capture2 : 1           = false;
                bool output1_set : 1        = false;
                bool output1_reset : 1      = false;
                bool output2_set : 1        = false;
                bool output2_reset : 1      = false;
                bool reset : 1              = false;
                bool delayed_protection : 1 = false;
            };
        };
    };

    struct HrtimTimerInterruptConfig
    {
        uint32_t                priority = 5;  // NVIC Priority (0-15)  // NOLINT(readability-magic-numbers)
        HrtimTimerInterruptMask interrupts{};
    };

    template <HrtimPeripheralId tkPeripheralId, HrtimTimerId tkTimerId>

    class HrtimTimerInterruptController
    {
    public:
        static constexpr HrtimPeripheralId skPeripheralId = tkPeripheralId;
        static constexpr HrtimTimerId      skTimerId      = tkTimerId;
        using ControllerTraitsT                           = HrtimControllerTraits<tkPeripheralId>;
        using TimerTraitsT                                = HrtimTimerTraits<tkPeripheralId, tkTimerId>;

        static void enable_interrupts(const HrtimTimerInterruptConfig& config)
        {
            if (config.interrupts.mask == 0)  // No interrupts enabled
            {
                return;
            }

            if (config.interrupts.compare1)
            {
                HrtimTimerInterruptTraits<skPeripheralId, skTimerId, HrtimTimerInterruptType::kCompare1>::enable();
            }

            if (config.interrupts.compare2)
            {
                HrtimTimerInterruptTraits<skPeripheralId, skTimerId, HrtimTimerInterruptType::kCompare2>::enable();
            }

            if (config.interrupts.compare3)
            {
                HrtimTimerInterruptTraits<skPeripheralId, skTimerId, HrtimTimerInterruptType::kCompare3>::enable();
            }

            if (config.interrupts.compare4)
            {
                HrtimTimerInterruptTraits<skPeripheralId, skTimerId, HrtimTimerInterruptType::kCompare4>::enable();
            }

            if (config.interrupts.update)
            {
                HrtimTimerInterruptTraits<skPeripheralId, skTimerId, HrtimTimerInterruptType::kUpdate>::enable();
            }

            if (config.interrupts.repetition)
            {
                HrtimTimerInterruptTraits<skPeripheralId, skTimerId, HrtimTimerInterruptType::kRepetition>::enable();
            }

            if (config.interrupts.capture1)
            {
                HrtimTimerInterruptTraits<skPeripheralId, skTimerId, HrtimTimerInterruptType::kCapture1>::enable();
            }

            if (config.interrupts.capture2)
            {
                HrtimTimerInterruptTraits<skPeripheralId, skTimerId, HrtimTimerInterruptType::kCapture2>::enable();
            }

            if (config.interrupts.output1_set)
            {
                HrtimTimerInterruptTraits<skPeripheralId, skTimerId, HrtimTimerInterruptType::kOutput1Set>::enable();
            }

            if (config.interrupts.output1_reset)
            {
                HrtimTimerInterruptTraits<skPeripheralId, skTimerId, HrtimTimerInterruptType::kOutput1Reset>::enable();
            }

            if (config.interrupts.output2_set)
            {
                HrtimTimerInterruptTraits<skPeripheralId, skTimerId, HrtimTimerInterruptType::kOutput2Set>::enable();
            }

            if (config.interrupts.output2_reset)
            {
                HrtimTimerInterruptTraits<skPeripheralId, skTimerId, HrtimTimerInterruptType::kOutput2Reset>::enable();
            }

            if (config.interrupts.reset)
            {
                HrtimTimerInterruptTraits<skPeripheralId, skTimerId, HrtimTimerInterruptType::kReset>::enable();
            }

            if (config.interrupts.delayed_protection)
            {
                HrtimTimerInterruptTraits<skPeripheralId, skTimerId, HrtimTimerInterruptType::kDelayedProtection>::
                    enable();
            }

            NVIC_SetPriority(TimerTraitsT::skIRQn, config.priority);
            NVIC_EnableIRQ(TimerTraitsT::skIRQn);
        }

        static void disable_interrupts()
        {
            HrtimTimerInterruptTraits<skPeripheralId, skTimerId, HrtimTimerInterruptType::kCompare1>::disable();
            HrtimTimerInterruptTraits<skPeripheralId, skTimerId, HrtimTimerInterruptType::kCompare2>::disable();
            HrtimTimerInterruptTraits<skPeripheralId, skTimerId, HrtimTimerInterruptType::kCompare3>::disable();
            HrtimTimerInterruptTraits<skPeripheralId, skTimerId, HrtimTimerInterruptType::kCompare4>::disable();
            HrtimTimerInterruptTraits<skPeripheralId, skTimerId, HrtimTimerInterruptType::kUpdate>::disable();
            HrtimTimerInterruptTraits<skPeripheralId, skTimerId, HrtimTimerInterruptType::kRepetition>::disable();
            HrtimTimerInterruptTraits<skPeripheralId, skTimerId, HrtimTimerInterruptType::kCapture1>::disable();
            HrtimTimerInterruptTraits<skPeripheralId, skTimerId, HrtimTimerInterruptType::kCapture2>::disable();
            HrtimTimerInterruptTraits<skPeripheralId, skTimerId, HrtimTimerInterruptType::kOutput1Set>::disable();
            HrtimTimerInterruptTraits<skPeripheralId, skTimerId, HrtimTimerInterruptType::kOutput1Reset>::disable();
            HrtimTimerInterruptTraits<skPeripheralId, skTimerId, HrtimTimerInterruptType::kOutput2Set>::disable();
            HrtimTimerInterruptTraits<skPeripheralId, skTimerId, HrtimTimerInterruptType::kOutput2Reset>::disable();
            HrtimTimerInterruptTraits<skPeripheralId, skTimerId, HrtimTimerInterruptType::kReset>::disable();
            HrtimTimerInterruptTraits<skPeripheralId, skTimerId, HrtimTimerInterruptType::kDelayedProtection>::
                disable();

            NVIC_DisableIRQ(TimerTraitsT::skIRQn);
        }
    };

    // ===========================================================================
    // INTERRUPT ROUTERS
    // ===========================================================================

    // -----------------------------------------------------------------------------
    // GLOBAL CONTROLLER AND MASTER TIMER ISR ROUTER
    // -----------------------------------------------------------------------------
    /**
     * @brief Global Controller and Master Timer ISR Router for a specific HRTIM.
     * Specializing this allows you to handle the entire ISR in one function
     * (e.g., when delegating to the ST HAL). Binding this takes precedence over
     * all controller and master timer ISR routers.
     *
     * @tparam tkPeripheralId HRTIM Peripheral ID.
     */
    template <HrtimPeripheralId tkPeripheralId>

    struct HrtimGlobalMasterIsrRouter
    {
        using UnboundIsrHandlerTag = void;
        static void handle()
        {
            // Default: Do nothing (Optimized away)
        }
    };

    // -----------------------------------------------------------------------------
    // GLOBAL CONTROLLER ISR ROUTER
    // -----------------------------------------------------------------------------
    /**
     * @brief Global Controller ISR Router for a specific HRTIM.
     * Specializing this allows you to handle the entire ISR in one function
     * (e.g., when delegating to the ST HAL).
     *
     * @tparam tkPeripheralId HRTIM Peripheral ID.
     */
    template <HrtimPeripheralId tkPeripheralId>

    struct HrtimGlobalControllerIsrRouter
    {
        using UnboundIsrHandlerTag = void;
        static void handle()
        {
            // Default: Do nothing (Optimized away)
        }
    };

    // -----------------------------------------------------------------------------
    // GRANULAR CONTROLLER ISR ROUTER
    // -----------------------------------------------------------------------------
    /**
     * @brief Controller ISR Router for a specific HRTIM.
     * Specializing this allows you to handle the entire ISR in one function
     * (e.g., when delegating to the ST HAL).
     *
     * @tparam tkPeripheralId HRTIM Peripheral ID.
     */
    template <HrtimPeripheralId tkPeripheralId, HrtimControllerInterruptType tkIntType>

    struct HrtimControllerIsrRouter
    {
        using UnboundIsrHandlerTag = void;
        static void handle()
        {
            // Default: Do nothing (Optimized away)
        }
    };

    // -----------------------------------------------------------------------------
    // GLOBAL FAULT ISR ROUTER
    // -----------------------------------------------------------------------------
    /**
     * @brief Global Fault ISR Router for a specific HRTIM.
     * Specializing this allows you to handle the entire ISR in one function
     * (e.g., when delegating to the ST HAL).
     *
     * @tparam tkPeripheralId HRTIM Peripheral ID.
     */
    template <HrtimPeripheralId tkPeripheralId>

    struct HrtimGlobalFaultIsrRouter
    {
        using UnboundIsrHandlerTag = void;
        static void handle()
        {
            // Default: Do nothing (Optimized away)
        }
    };

    // -----------------------------------------------------------------------------
    // GRANULAR FAULT ISR ROUTER
    // -----------------------------------------------------------------------------
    /**
     * @brief Fault ISR Router for a specific HRTIM.
     * Specializing this allows you to handle the entire ISR in one function
     * (e.g., when delegating to the ST HAL).
     *
     * @tparam tkPeripheralId HRTIM Peripheral ID.
     */
    template <HrtimPeripheralId tkPeripheralId, HrtimFaultInterruptType tkIntType>

    struct HrtimFaultIsrRouter
    {
        using UnboundIsrHandlerTag = void;
        static void handle()
        {
            // Default: Do nothing (Optimized away)
        }
    };

    // -----------------------------------------------------------------------------
    // GLOBAL MASTER TIMER ISR ROUTER
    // -----------------------------------------------------------------------------
    /**
     * @brief Global Master Timer ISR Router for a specific HRTIM.
     * Specializing this allows you to handle the entire ISR in one function
     * (e.g., when delegating to the ST HAL).
     *
     * @tparam tkPeripheralId HRTIM Peripheral ID.
     */
    template <HrtimPeripheralId tkPeripheralId>

    struct HrtimGlobalMasterTimerIsrRouter
    {
        using UnboundIsrHandlerTag = void;
        static void handle()
        {
            // Default: Do nothing (Optimized away)
        }
    };

    // -----------------------------------------------------------------------------
    // GRANULAR MASTER TIMER ISR ROUTER
    // -----------------------------------------------------------------------------
    /**
     * @brief Master Timer ISR Router for a specific HRTIM.
     * Specializing this allows you to handle the entire ISR in one function
     * (e.g., when delegating to the ST HAL).
     *
     * @tparam tkPeripheralId HRTIM Peripheral ID.
     */
    template <HrtimPeripheralId tkPeripheralId, HrtimMasterTimerInterruptType tkIntType>

    struct HrtimMasterTimerIsrRouter
    {
        using UnboundIsrHandlerTag = void;
        static void handle()
        {
            // Default: Do nothing (Optimized away)
        }
    };

    // -----------------------------------------------------------------------------
    // GLOBAL TIMER ISR ROUTER
    // -----------------------------------------------------------------------------
    /**
     * @brief Global Timer ISR Router for a specific HRTIM.
     * Specializing this allows you to handle the entire ISR in one function
     * (e.g., when delegating to the ST HAL).
     *
     * @tparam tkPeripheralId HRTIM Peripheral ID.
     * @tparam tkTimerId HRTIM Timer ID.
     */
    template <HrtimPeripheralId tkPeripheralId, HrtimTimerId tkTimerId>

    struct HrtimGlobalTimerIsrRouter
    {
        using UnboundIsrHandlerTag = void;
        static void handle()
        {
            // Default: Do nothing (Optimized away)
        }
    };

    // -----------------------------------------------------------------------------
    // GRANULAR TIMER ISR ROUTER
    // -----------------------------------------------------------------------------

    /**
     * @brief HRTIM Interrupt Router.
     *
     * Specialize this template in your application or driver to bind
     * logic to specific HRTIM interrupts.
     *
     * @tparam tkPeripheralId The HRTIM peripheral index the interrupt belongs to.
     * @tparam tkTimerId The HRTIM Timer index the interrupt belongs to.
     * @tparam tkIntType The interrupt type triggered.
     */
    template <HrtimPeripheralId tkPeripheralId, HrtimTimerId tkTimerId, HrtimTimerInterruptType tkIntType>

    struct HrtimTimerIsrRouter
    {
        using UnboundIsrHandlerTag = void;

        static void handle()
        {
            // Default implementation does nothing.
        }
    };

}  // namespace valle::platform
