#pragma once

#include "valle/platform/hardware/hrtim/interface.hpp"

namespace valle::platform
{
    // =============================================================================
    // HRTIM CONTROLLER INTERRUPTS
    // =============================================================================

    enum class HRTIMControllerInterruptType : uint8_t
    {
        kDLLReady,         // DLLRDY
        kBurstModePeriod,  // BMPER
        kSync,             // SYNC
    };

    template <HRTIMControllerID tkControllerID, HRTIMControllerInterruptType tkIntType>
    struct HRTIMControllerInterruptTraits;

#define DEFINE_HRTIM_INT_TRAIT(tkIntType, ll_name)                                                     \
    template <HRTIMControllerID tkControllerID>                                                        \
    struct HRTIMControllerInterruptTraits<tkControllerID, tkIntType>                                   \
    {                                                                                                  \
        static inline void enable()                                                                    \
        {                                                                                              \
            LL_HRTIM_EnableIT_##ll_name(HRTIMControllerTraits<tkControllerID>::skInstance);            \
        }                                                                                              \
        static inline void disable()                                                                   \
        {                                                                                              \
            LL_HRTIM_DisableIT_##ll_name(HRTIMControllerTraits<tkControllerID>::skInstance);           \
        }                                                                                              \
        static inline bool is_enabled()                                                                \
        {                                                                                              \
            return LL_HRTIM_IsEnabledIT_##ll_name(HRTIMControllerTraits<tkControllerID>::skInstance);  \
        }                                                                                              \
        static inline bool flag_active()                                                               \
        {                                                                                              \
            return LL_HRTIM_IsActiveFlag_##ll_name(HRTIMControllerTraits<tkControllerID>::skInstance); \
        }                                                                                              \
                                                                                                       \
        static inline bool is_pending()                                                                \
        {                                                                                              \
            return flag_active() && is_enabled();                                                      \
        }                                                                                              \
        static inline void ack()                                                                       \
        {                                                                                              \
            LL_HRTIM_ClearFlag_##ll_name(HRTIMControllerTraits<tkControllerID>::skInstance);           \
        }                                                                                              \
    };

    DEFINE_HRTIM_INT_TRAIT(HRTIMControllerInterruptType::kDLLReady, DLLRDY);
    DEFINE_HRTIM_INT_TRAIT(HRTIMControllerInterruptType::kBurstModePeriod, BMPER);
    DEFINE_HRTIM_INT_TRAIT(HRTIMControllerInterruptType::kSync, SYNC);

#undef DEFINE_HRTIM_INT_TRAIT

    // =============================================================================
    // HRTIM FAULT INTERRUPTS
    // =============================================================================
    enum class HRTIMFaultInterruptType : uint8_t
    {
        kSystemFault,  // SYSFLT
        kFault1,       // FLT1
        kFault2,       // FLT2
        kFault3,       // FLT3
        kFault4,       // FLT4
        kFault5,       // FLT5
        kFault6,       // FLT6
    };

    template <HRTIMControllerID tkControllerID, HRTIMFaultInterruptType tkIntType>
    struct HRTIMFaultInterruptTraits;

#define DEFINE_HRTIM_FAULT_INT_TRAIT(tkIntType, ll_name)                                               \
    template <HRTIMControllerID tkControllerID>                                                        \
    struct HRTIMFaultInterruptTraits<tkControllerID, tkIntType>                                        \
    {                                                                                                  \
        static inline void enable()                                                                    \
        {                                                                                              \
            LL_HRTIM_EnableIT_##ll_name(HRTIMControllerTraits<tkControllerID>::skInstance);            \
        }                                                                                              \
        static inline void disable()                                                                   \
        {                                                                                              \
            LL_HRTIM_DisableIT_##ll_name(HRTIMControllerTraits<tkControllerID>::skInstance);           \
        }                                                                                              \
        static inline bool is_enabled()                                                                \
        {                                                                                              \
            return LL_HRTIM_IsEnabledIT_##ll_name(HRTIMControllerTraits<tkControllerID>::skInstance);  \
        }                                                                                              \
        static inline bool flag_active()                                                               \
        {                                                                                              \
            return LL_HRTIM_IsActiveFlag_##ll_name(HRTIMControllerTraits<tkControllerID>::skInstance); \
        }                                                                                              \
        static inline bool is_pending()                                                                \
        {                                                                                              \
            return flag_active() && is_enabled();                                                      \
        }                                                                                              \
        static inline void ack()                                                                       \
        {                                                                                              \
            LL_HRTIM_ClearFlag_##ll_name(HRTIMControllerTraits<tkControllerID>::skInstance);           \
        }                                                                                              \
    };

    DEFINE_HRTIM_FAULT_INT_TRAIT(HRTIMFaultInterruptType::kSystemFault, SYSFLT);
    DEFINE_HRTIM_FAULT_INT_TRAIT(HRTIMFaultInterruptType::kFault1, FLT1);
    DEFINE_HRTIM_FAULT_INT_TRAIT(HRTIMFaultInterruptType::kFault2, FLT2);
    DEFINE_HRTIM_FAULT_INT_TRAIT(HRTIMFaultInterruptType::kFault3, FLT3);
    DEFINE_HRTIM_FAULT_INT_TRAIT(HRTIMFaultInterruptType::kFault4, FLT4);
    DEFINE_HRTIM_FAULT_INT_TRAIT(HRTIMFaultInterruptType::kFault5, FLT5);
    DEFINE_HRTIM_FAULT_INT_TRAIT(HRTIMFaultInterruptType::kFault6, FLT6);

#undef DEFINE_HRTIM_FAULT_INT_TRAIT

    // =============================================================================
    // HRTIM MASTER TIMER INTERRUPTS
    // =============================================================================

    enum class HRTIMMasterTimerInterruptType : uint8_t
    {
        kCompare1,    // CMP1
        kCompare2,    // CMP2
        kCompare3,    // CMP3
        kCompare4,    // CMP4
        kUpdate,      // UPDATE
        kRepetition,  // REP
    };

    template <HRTIMControllerID tkControllerID, HRTIMMasterTimerInterruptType tkIntType>
    struct HRTIMMasterTimerInterruptTraits;

#define DEFINE_HRTIM_INT_TRAIT(tkIntType, ll_name)                                                                  \
    template <HRTIMControllerID tkControllerID>                                                                     \
    struct HRTIMMasterTimerInterruptTraits<tkControllerID, tkIntType>                                               \
    {                                                                                                               \
        static inline void enable()                                                                                 \
        {                                                                                                           \
            LL_HRTIM_EnableIT_##ll_name(HRTIMControllerTraits<tkControllerID>::skInstance, LL_HRTIM_TIMER_MASTER);  \
        }                                                                                                           \
        static inline void disable()                                                                                \
        {                                                                                                           \
            LL_HRTIM_DisableIT_##ll_name(HRTIMControllerTraits<tkControllerID>::skInstance, LL_HRTIM_TIMER_MASTER); \
        }                                                                                                           \
        static inline bool is_enabled()                                                                             \
        {                                                                                                           \
            return LL_HRTIM_IsEnabledIT_##ll_name(HRTIMControllerTraits<tkControllerID>::skInstance,                \
                                                  LL_HRTIM_TIMER_MASTER);                                           \
        }                                                                                                           \
        static inline bool flag_active()                                                                            \
        {                                                                                                           \
            return LL_HRTIM_IsActiveFlag_##ll_name(HRTIMControllerTraits<tkControllerID>::skInstance,               \
                                                   LL_HRTIM_TIMER_MASTER);                                          \
        }                                                                                                           \
                                                                                                                    \
        static inline bool is_pending()                                                                             \
        {                                                                                                           \
            return flag_active() && is_enabled();                                                                   \
        }                                                                                                           \
        static inline void ack()                                                                                    \
        {                                                                                                           \
            LL_HRTIM_ClearFlag_##ll_name(HRTIMControllerTraits<tkControllerID>::skInstance, LL_HRTIM_TIMER_MASTER); \
        }                                                                                                           \
    };

    DEFINE_HRTIM_INT_TRAIT(HRTIMMasterTimerInterruptType::kCompare1, CMP1);
    DEFINE_HRTIM_INT_TRAIT(HRTIMMasterTimerInterruptType::kCompare2, CMP2);
    DEFINE_HRTIM_INT_TRAIT(HRTIMMasterTimerInterruptType::kCompare3, CMP3);
    DEFINE_HRTIM_INT_TRAIT(HRTIMMasterTimerInterruptType::kCompare4, CMP4);
    DEFINE_HRTIM_INT_TRAIT(HRTIMMasterTimerInterruptType::kUpdate, UPDATE);
    DEFINE_HRTIM_INT_TRAIT(HRTIMMasterTimerInterruptType::kRepetition, REP);

#undef DEFINE_HRTIM_INT_TRAIT

    // =============================================================================
    // HRTIM TIMER INTERRUPTS
    // =============================================================================

    enum class HRTIMTimerInterruptType : uint8_t
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

    template <HRTIMControllerID tkControllerID, HRTIMTimerID tkTimerID, HRTIMTimerInterruptType tkIntType>
    struct HRTIMTimerInterruptTraits;

#define DEFINE_HRTIM_INT_TRAIT(tkIntType, ll_name)                                                       \
    template <HRTIMControllerID tkControllerID, HRTIMTimerID tkTimerID>                                  \
    struct HRTIMTimerInterruptTraits<tkControllerID, tkTimerID, tkIntType>                               \
    {                                                                                                    \
        static inline void enable()                                                                      \
        {                                                                                                \
            LL_HRTIM_EnableIT_##ll_name(HRTIMControllerTraits<tkControllerID>::skInstance,               \
                                        HRTIMTimerTraits<tkControllerID, tkTimerID>::skLLID);            \
        }                                                                                                \
        static inline void disable()                                                                     \
        {                                                                                                \
            LL_HRTIM_DisableIT_##ll_name(HRTIMControllerTraits<tkControllerID>::skInstance,              \
                                         HRTIMTimerTraits<tkControllerID, tkTimerID>::skLLID);           \
        }                                                                                                \
        static inline bool is_enabled()                                                                  \
        {                                                                                                \
            return LL_HRTIM_IsEnabledIT_##ll_name(HRTIMControllerTraits<tkControllerID>::skInstance,     \
                                                  HRTIMTimerTraits<tkControllerID, tkTimerID>::skLLID);  \
        }                                                                                                \
        static inline bool flag_active()                                                                 \
        {                                                                                                \
            return LL_HRTIM_IsActiveFlag_##ll_name(HRTIMControllerTraits<tkControllerID>::skInstance,    \
                                                   HRTIMTimerTraits<tkControllerID, tkTimerID>::skLLID); \
        }                                                                                                \
                                                                                                         \
        static inline bool is_pending()                                                                  \
        {                                                                                                \
            return flag_active() && is_enabled();                                                        \
        }                                                                                                \
        static inline void ack()                                                                         \
        {                                                                                                \
            LL_HRTIM_ClearFlag_##ll_name(HRTIMControllerTraits<tkControllerID>::skInstance,              \
                                         HRTIMTimerTraits<tkControllerID, tkTimerID>::skLLID);           \
        }                                                                                                \
    };

    DEFINE_HRTIM_INT_TRAIT(HRTIMTimerInterruptType::kCompare1, CMP1);
    DEFINE_HRTIM_INT_TRAIT(HRTIMTimerInterruptType::kCompare2, CMP2);
    DEFINE_HRTIM_INT_TRAIT(HRTIMTimerInterruptType::kCompare3, CMP3);
    DEFINE_HRTIM_INT_TRAIT(HRTIMTimerInterruptType::kCompare4, CMP4);
    DEFINE_HRTIM_INT_TRAIT(HRTIMTimerInterruptType::kUpdate, UPDATE);
    DEFINE_HRTIM_INT_TRAIT(HRTIMTimerInterruptType::kRepetition, REP);
    DEFINE_HRTIM_INT_TRAIT(HRTIMTimerInterruptType::kCapture1, CPT1);
    DEFINE_HRTIM_INT_TRAIT(HRTIMTimerInterruptType::kCapture2, CPT2);
    DEFINE_HRTIM_INT_TRAIT(HRTIMTimerInterruptType::kOutput1Set, SET1);
    DEFINE_HRTIM_INT_TRAIT(HRTIMTimerInterruptType::kOutput1Reset, RST1);
    DEFINE_HRTIM_INT_TRAIT(HRTIMTimerInterruptType::kOutput2Set, SET2);
    DEFINE_HRTIM_INT_TRAIT(HRTIMTimerInterruptType::kOutput2Reset, RST2);
    DEFINE_HRTIM_INT_TRAIT(HRTIMTimerInterruptType::kReset, RST);
    DEFINE_HRTIM_INT_TRAIT(HRTIMTimerInterruptType::kDelayedProtection, DLYPRT);

#undef DEFINE_HRTIM_INT_TRAIT

    // ===========================================================================
    // INTERRUPT CONTROLLER
    // ===========================================================================

    // -----------------------------------------------------------------------------
    // TIMER INTERRUPT CONTROLLER
    // -----------------------------------------------------------------------------
    struct HRTIMTimerInterruptMask
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

    struct HRTIMTimerInterruptConfig
    {
        uint32_t                priority = 5;  // NVIC Priority (0-15)  // NOLINT(readability-magic-numbers)
        HRTIMTimerInterruptMask interrupts{};
    };

    template <HRTIMControllerID tkControllerID, HRTIMTimerID tkTimerID>
        requires(kValidHRTIMControllerID<tkControllerID>)
    class HRTIMTimerInterruptController
    {
    public:
        static constexpr HRTIMControllerID skControllerID = tkControllerID;
        static constexpr HRTIMTimerID      skTimerID      = tkTimerID;
        using ControllerTraitsT                           = HRTIMControllerTraits<tkControllerID>;
        using TimerTraitsT                                = HRTIMTimerTraits<tkControllerID, tkTimerID>;

        static void enable_interrupts(const HRTIMTimerInterruptConfig& config)
        {
            if (config.interrupts.mask == 0)  // No interrupts enabled
            {
                return;
            }

            if (config.interrupts.compare1)
            {
                HRTIMTimerInterruptTraits<skControllerID, skTimerID, HRTIMTimerInterruptType::kCompare1>::enable();
            }

            if (config.interrupts.compare2)
            {
                HRTIMTimerInterruptTraits<skControllerID, skTimerID, HRTIMTimerInterruptType::kCompare2>::enable();
            }

            if (config.interrupts.compare3)
            {
                HRTIMTimerInterruptTraits<skControllerID, skTimerID, HRTIMTimerInterruptType::kCompare3>::enable();
            }

            if (config.interrupts.compare4)
            {
                HRTIMTimerInterruptTraits<skControllerID, skTimerID, HRTIMTimerInterruptType::kCompare4>::enable();
            }

            if (config.interrupts.update)
            {
                HRTIMTimerInterruptTraits<skControllerID, skTimerID, HRTIMTimerInterruptType::kUpdate>::enable();
            }

            if (config.interrupts.repetition)
            {
                HRTIMTimerInterruptTraits<skControllerID, skTimerID, HRTIMTimerInterruptType::kRepetition>::enable();
            }

            if (config.interrupts.capture1)
            {
                HRTIMTimerInterruptTraits<skControllerID, skTimerID, HRTIMTimerInterruptType::kCapture1>::enable();
            }

            if (config.interrupts.capture2)
            {
                HRTIMTimerInterruptTraits<skControllerID, skTimerID, HRTIMTimerInterruptType::kCapture2>::enable();
            }

            if (config.interrupts.output1_set)
            {
                HRTIMTimerInterruptTraits<skControllerID, skTimerID, HRTIMTimerInterruptType::kOutput1Set>::enable();
            }

            if (config.interrupts.output1_reset)
            {
                HRTIMTimerInterruptTraits<skControllerID, skTimerID, HRTIMTimerInterruptType::kOutput1Reset>::enable();
            }

            if (config.interrupts.output2_set)
            {
                HRTIMTimerInterruptTraits<skControllerID, skTimerID, HRTIMTimerInterruptType::kOutput2Set>::enable();
            }

            if (config.interrupts.output2_reset)
            {
                HRTIMTimerInterruptTraits<skControllerID, skTimerID, HRTIMTimerInterruptType::kOutput2Reset>::enable();
            }

            if (config.interrupts.reset)
            {
                HRTIMTimerInterruptTraits<skControllerID, skTimerID, HRTIMTimerInterruptType::kReset>::enable();
            }

            if (config.interrupts.delayed_protection)
            {
                HRTIMTimerInterruptTraits<skControllerID, skTimerID, HRTIMTimerInterruptType::kDelayedProtection>::
                    enable();
            }

            NVIC_SetPriority(TimerTraitsT::skIRQn, config.priority);
            NVIC_EnableIRQ(TimerTraitsT::skIRQn);
        }

        static void disable_interrupts()
        {
            HRTIMTimerInterruptTraits<skControllerID, skTimerID, HRTIMTimerInterruptType::kCompare1>::disable();
            HRTIMTimerInterruptTraits<skControllerID, skTimerID, HRTIMTimerInterruptType::kCompare2>::disable();
            HRTIMTimerInterruptTraits<skControllerID, skTimerID, HRTIMTimerInterruptType::kCompare3>::disable();
            HRTIMTimerInterruptTraits<skControllerID, skTimerID, HRTIMTimerInterruptType::kCompare4>::disable();
            HRTIMTimerInterruptTraits<skControllerID, skTimerID, HRTIMTimerInterruptType::kUpdate>::disable();
            HRTIMTimerInterruptTraits<skControllerID, skTimerID, HRTIMTimerInterruptType::kRepetition>::disable();
            HRTIMTimerInterruptTraits<skControllerID, skTimerID, HRTIMTimerInterruptType::kCapture1>::disable();
            HRTIMTimerInterruptTraits<skControllerID, skTimerID, HRTIMTimerInterruptType::kCapture2>::disable();
            HRTIMTimerInterruptTraits<skControllerID, skTimerID, HRTIMTimerInterruptType::kOutput1Set>::disable();
            HRTIMTimerInterruptTraits<skControllerID, skTimerID, HRTIMTimerInterruptType::kOutput1Reset>::disable();
            HRTIMTimerInterruptTraits<skControllerID, skTimerID, HRTIMTimerInterruptType::kOutput2Set>::disable();
            HRTIMTimerInterruptTraits<skControllerID, skTimerID, HRTIMTimerInterruptType::kOutput2Reset>::disable();
            HRTIMTimerInterruptTraits<skControllerID, skTimerID, HRTIMTimerInterruptType::kReset>::disable();
            HRTIMTimerInterruptTraits<skControllerID, skTimerID, HRTIMTimerInterruptType::kDelayedProtection>::
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
     * @tparam tkControllerID HRTIM Controller ID.
     */
    template <HRTIMControllerID tkControllerID>
        requires(kValidHRTIMControllerID<tkControllerID>)
    struct HRTIMGlobalMasterISRRouter
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
     * @tparam tkControllerID HRTIM Controller ID.
     */
    template <HRTIMControllerID tkControllerID>
        requires(kValidHRTIMControllerID<tkControllerID>)
    struct HRTIMGlobalControllerISRRouter
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
     * @tparam tkControllerID HRTIM Controller ID.
     */
    template <HRTIMControllerID tkControllerID, HRTIMControllerInterruptType tkIntType>
        requires(kValidHRTIMControllerID<tkControllerID>)
    struct HRTIMControllerISRRouter
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
     * @tparam tkControllerID HRTIM Controller ID.
     */
    template <HRTIMControllerID tkControllerID>
        requires(kValidHRTIMControllerID<tkControllerID>)
    struct HRTIMGlobalFaultISRRouter
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
     * @tparam tkControllerID HRTIM Controller ID.
     */
    template <HRTIMControllerID tkControllerID, HRTIMFaultInterruptType tkIntType>
        requires(kValidHRTIMControllerID<tkControllerID>)
    struct HRTIMFaultISRRouter
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
     * @tparam tkControllerID HRTIM Controller ID.
     */
    template <HRTIMControllerID tkControllerID>
        requires(kValidHRTIMControllerID<tkControllerID>)
    struct HRTIMGlobalMasterTimerISRRouter
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
     * @tparam tkControllerID HRTIM Controller ID.
     */
    template <HRTIMControllerID tkControllerID, HRTIMMasterTimerInterruptType tkIntType>
        requires(kValidHRTIMControllerID<tkControllerID>)
    struct HRTIMMasterTimerISRRouter
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
     * @tparam tkControllerID HRTIM Controller ID.
     * @tparam tkTimerID HRTIM Timer ID.
     */
    template <HRTIMControllerID tkControllerID, HRTIMTimerID tkTimerID>
        requires(kValidHRTIMControllerID<tkControllerID>)
    struct HRTIMGlobalTimerISRRouter
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
     * @tparam tkControllerID The HRTIM peripheral index the interrupt belongs to.
     * @tparam tkTimerID The HRTIM Timer index the interrupt belongs to.
     * @tparam tkIntType The interrupt type triggered.
     */
    template <HRTIMControllerID tkControllerID, HRTIMTimerID tkTimerID, HRTIMTimerInterruptType tkIntType>
        requires(kValidHRTIMControllerID<tkControllerID>)
    struct HRTIMTimerISRRouter
    {
        using UnboundIsrHandlerTag = void;

        static void handle()
        {
            // Default implementation does nothing.
        }
    };

}  // namespace valle::platform