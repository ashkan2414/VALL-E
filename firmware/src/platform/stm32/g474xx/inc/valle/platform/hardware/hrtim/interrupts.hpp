#pragma once

#include "valle/platform/hardware/hrtim/interface.hpp"

namespace valle::platform
{
    // =============================================================================
    // HRTIM CONTROLLER INTERRUPTS
    // =============================================================================

    enum class HrtimControllerInterruptSource : uint8_t
    {
        kDLLReady,         // DLLRDY
        kBurstModePeriod,  // BMPER
        kSync,             // SYNC
    };

    template <HrtimControllerId tkControllerId, HrtimControllerInterruptSource tkIntSource>
    struct HrtimControllerInterruptSourceInterface;

#define DEFINE_HRTIM_INT_TRAIT(tkIntSource, ll_name)                                                   \
    template <HrtimControllerId tkControllerId>                                                        \
    struct HrtimControllerInterruptSourceInterface<tkControllerId, tkIntSource>                        \
    {                                                                                                  \
        static inline void enable()                                                                    \
        {                                                                                              \
            LL_HRTIM_EnableIT_##ll_name(HrtimControllerTraits<tkControllerId>::skInstance);            \
        }                                                                                              \
        static inline void disable()                                                                   \
        {                                                                                              \
            LL_HRTIM_DisableIT_##ll_name(HrtimControllerTraits<tkControllerId>::skInstance);           \
        }                                                                                              \
        static inline bool is_enabled()                                                                \
        {                                                                                              \
            return LL_HRTIM_IsEnabledIT_##ll_name(HrtimControllerTraits<tkControllerId>::skInstance);  \
        }                                                                                              \
        static inline bool flag_active()                                                               \
        {                                                                                              \
            return LL_HRTIM_IsActiveFlag_##ll_name(HrtimControllerTraits<tkControllerId>::skInstance); \
        }                                                                                              \
                                                                                                       \
        static inline bool is_pending()                                                                \
        {                                                                                              \
            return flag_active() && is_enabled();                                                      \
        }                                                                                              \
        static inline void clear()                                                                     \
        {                                                                                              \
            LL_HRTIM_ClearFlag_##ll_name(HrtimControllerTraits<tkControllerId>::skInstance);           \
        }                                                                                              \
    };

    DEFINE_HRTIM_INT_TRAIT(HrtimControllerInterruptSource::kDLLReady, DLLRDY);
    DEFINE_HRTIM_INT_TRAIT(HrtimControllerInterruptSource::kBurstModePeriod, BMPER);
    DEFINE_HRTIM_INT_TRAIT(HrtimControllerInterruptSource::kSync, SYNC);

#undef DEFINE_HRTIM_INT_TRAIT

    // =============================================================================
    // HRTIM FAULT INTERRUPTS
    // =============================================================================
    enum class HrtimFaultInterruptSource : uint8_t
    {
        kSystemFault,  // SYSFLT
        kFault1,       // FLT1
        kFault2,       // FLT2
        kFault3,       // FLT3
        kFault4,       // FLT4
        kFault5,       // FLT5
        kFault6,       // FLT6
    };

    template <HrtimControllerId tkControllerId, HrtimFaultInterruptSource tkIntSource>
    struct HrtimFaultInterruptSourceInterface;

#define DEFINE_HRTIM_FAULT_INT_TRAIT(tkIntSource, ll_name)                                             \
    template <HrtimControllerId tkControllerId>                                                        \
    struct HrtimFaultInterruptSourceInterface<tkControllerId, tkIntSource>                             \
    {                                                                                                  \
        static inline void enable()                                                                    \
        {                                                                                              \
            LL_HRTIM_EnableIT_##ll_name(HrtimControllerTraits<tkControllerId>::skInstance);            \
        }                                                                                              \
        static inline void disable()                                                                   \
        {                                                                                              \
            LL_HRTIM_DisableIT_##ll_name(HrtimControllerTraits<tkControllerId>::skInstance);           \
        }                                                                                              \
        static inline bool is_enabled()                                                                \
        {                                                                                              \
            return LL_HRTIM_IsEnabledIT_##ll_name(HrtimControllerTraits<tkControllerId>::skInstance);  \
        }                                                                                              \
        static inline bool flag_active()                                                               \
        {                                                                                              \
            return LL_HRTIM_IsActiveFlag_##ll_name(HrtimControllerTraits<tkControllerId>::skInstance); \
        }                                                                                              \
        static inline bool is_pending()                                                                \
        {                                                                                              \
            return flag_active() && is_enabled();                                                      \
        }                                                                                              \
        static inline void clear()                                                                     \
        {                                                                                              \
            LL_HRTIM_ClearFlag_##ll_name(HrtimControllerTraits<tkControllerId>::skInstance);           \
        }                                                                                              \
    };

    DEFINE_HRTIM_FAULT_INT_TRAIT(HrtimFaultInterruptSource::kSystemFault, SYSFLT);
    DEFINE_HRTIM_FAULT_INT_TRAIT(HrtimFaultInterruptSource::kFault1, FLT1);
    DEFINE_HRTIM_FAULT_INT_TRAIT(HrtimFaultInterruptSource::kFault2, FLT2);
    DEFINE_HRTIM_FAULT_INT_TRAIT(HrtimFaultInterruptSource::kFault3, FLT3);
    DEFINE_HRTIM_FAULT_INT_TRAIT(HrtimFaultInterruptSource::kFault4, FLT4);
    DEFINE_HRTIM_FAULT_INT_TRAIT(HrtimFaultInterruptSource::kFault5, FLT5);
    DEFINE_HRTIM_FAULT_INT_TRAIT(HrtimFaultInterruptSource::kFault6, FLT6);

#undef DEFINE_HRTIM_FAULT_INT_TRAIT

    // =============================================================================
    // HRTIM MASTER TIMER INTERRUPTS
    // =============================================================================

    enum class HrtimMasterTimerInterruptSource : uint8_t
    {
        kCompare1,    // CMP1
        kCompare2,    // CMP2
        kCompare3,    // CMP3
        kCompare4,    // CMP4
        kUpdate,      // UPDATE
        kRepetition,  // REP
    };

    template <HrtimControllerId tkControllerId, HrtimMasterTimerInterruptSource tkIntSource>
    struct HrtimMasterTimerInterruptSourceInterface;

#define DEFINE_HRTIM_INT_TRAIT(tkIntSource, ll_name)                                                                \
    template <HrtimControllerId tkControllerId>                                                                     \
    struct HrtimMasterTimerInterruptSourceInterface<tkControllerId, tkIntSource>                                    \
    {                                                                                                               \
        static inline void enable()                                                                                 \
        {                                                                                                           \
            LL_HRTIM_EnableIT_##ll_name(HrtimControllerTraits<tkControllerId>::skInstance, LL_HRTIM_TIMER_MASTER);  \
        }                                                                                                           \
        static inline void disable()                                                                                \
        {                                                                                                           \
            LL_HRTIM_DisableIT_##ll_name(HrtimControllerTraits<tkControllerId>::skInstance, LL_HRTIM_TIMER_MASTER); \
        }                                                                                                           \
        static inline bool is_enabled()                                                                             \
        {                                                                                                           \
            return LL_HRTIM_IsEnabledIT_##ll_name(HrtimControllerTraits<tkControllerId>::skInstance,                \
                                                  LL_HRTIM_TIMER_MASTER);                                           \
        }                                                                                                           \
        static inline bool flag_active()                                                                            \
        {                                                                                                           \
            return LL_HRTIM_IsActiveFlag_##ll_name(HrtimControllerTraits<tkControllerId>::skInstance,               \
                                                   LL_HRTIM_TIMER_MASTER);                                          \
        }                                                                                                           \
                                                                                                                    \
        static inline bool is_pending()                                                                             \
        {                                                                                                           \
            return flag_active() && is_enabled();                                                                   \
        }                                                                                                           \
        static inline void clear()                                                                                  \
        {                                                                                                           \
            LL_HRTIM_ClearFlag_##ll_name(HrtimControllerTraits<tkControllerId>::skInstance, LL_HRTIM_TIMER_MASTER); \
        }                                                                                                           \
    };

    DEFINE_HRTIM_INT_TRAIT(HrtimMasterTimerInterruptSource::kCompare1, CMP1);
    DEFINE_HRTIM_INT_TRAIT(HrtimMasterTimerInterruptSource::kCompare2, CMP2);
    DEFINE_HRTIM_INT_TRAIT(HrtimMasterTimerInterruptSource::kCompare3, CMP3);
    DEFINE_HRTIM_INT_TRAIT(HrtimMasterTimerInterruptSource::kCompare4, CMP4);
    DEFINE_HRTIM_INT_TRAIT(HrtimMasterTimerInterruptSource::kUpdate, UPDATE);
    DEFINE_HRTIM_INT_TRAIT(HrtimMasterTimerInterruptSource::kRepetition, REP);

#undef DEFINE_HRTIM_INT_TRAIT

    // =============================================================================
    // HRTIM TIMER INTERRUPTS
    // =============================================================================

    enum class HrtimTimerInterruptSource : uint8_t
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

    template <HrtimControllerId tkControllerId, HrtimTimerId tkTimerId, HrtimTimerInterruptSource tkIntSource>
    struct HrtimTimerInterruptSourceInterface;

#define DEFINE_HRTIM_INT_TRAIT(tkIntSource, ll_name)                                                     \
    template <HrtimControllerId tkControllerId, HrtimTimerId tkTimerId>                                  \
    struct HrtimTimerInterruptSourceInterface<tkControllerId, tkTimerId, tkIntSource>                    \
    {                                                                                                    \
        static inline void enable()                                                                      \
        {                                                                                                \
            LL_HRTIM_EnableIT_##ll_name(HrtimControllerTraits<tkControllerId>::skInstance,               \
                                        HrtimTimerTraits<tkControllerId, tkTimerId>::skLLId);            \
        }                                                                                                \
        static inline void disable()                                                                     \
        {                                                                                                \
            LL_HRTIM_DisableIT_##ll_name(HrtimControllerTraits<tkControllerId>::skInstance,              \
                                         HrtimTimerTraits<tkControllerId, tkTimerId>::skLLId);           \
        }                                                                                                \
        static inline bool is_enabled()                                                                  \
        {                                                                                                \
            return LL_HRTIM_IsEnabledIT_##ll_name(HrtimControllerTraits<tkControllerId>::skInstance,     \
                                                  HrtimTimerTraits<tkControllerId, tkTimerId>::skLLId);  \
        }                                                                                                \
        static inline bool flag_active()                                                                 \
        {                                                                                                \
            return LL_HRTIM_IsActiveFlag_##ll_name(HrtimControllerTraits<tkControllerId>::skInstance,    \
                                                   HrtimTimerTraits<tkControllerId, tkTimerId>::skLLId); \
        }                                                                                                \
                                                                                                         \
        static inline bool is_pending()                                                                  \
        {                                                                                                \
            return flag_active() && is_enabled();                                                        \
        }                                                                                                \
        static inline void clear()                                                                       \
        {                                                                                                \
            LL_HRTIM_ClearFlag_##ll_name(HrtimControllerTraits<tkControllerId>::skInstance,              \
                                         HrtimTimerTraits<tkControllerId, tkTimerId>::skLLId);           \
        }                                                                                                \
    };

    DEFINE_HRTIM_INT_TRAIT(HrtimTimerInterruptSource::kCompare1, CMP1);
    DEFINE_HRTIM_INT_TRAIT(HrtimTimerInterruptSource::kCompare2, CMP2);
    DEFINE_HRTIM_INT_TRAIT(HrtimTimerInterruptSource::kCompare3, CMP3);
    DEFINE_HRTIM_INT_TRAIT(HrtimTimerInterruptSource::kCompare4, CMP4);
    DEFINE_HRTIM_INT_TRAIT(HrtimTimerInterruptSource::kUpdate, UPDATE);
    DEFINE_HRTIM_INT_TRAIT(HrtimTimerInterruptSource::kRepetition, REP);
    DEFINE_HRTIM_INT_TRAIT(HrtimTimerInterruptSource::kCapture1, CPT1);
    DEFINE_HRTIM_INT_TRAIT(HrtimTimerInterruptSource::kCapture2, CPT2);
    DEFINE_HRTIM_INT_TRAIT(HrtimTimerInterruptSource::kOutput1Set, SET1);
    DEFINE_HRTIM_INT_TRAIT(HrtimTimerInterruptSource::kOutput1Reset, RST1);
    DEFINE_HRTIM_INT_TRAIT(HrtimTimerInterruptSource::kOutput2Set, SET2);
    DEFINE_HRTIM_INT_TRAIT(HrtimTimerInterruptSource::kOutput2Reset, RST2);
    DEFINE_HRTIM_INT_TRAIT(HrtimTimerInterruptSource::kReset, RST);
    DEFINE_HRTIM_INT_TRAIT(HrtimTimerInterruptSource::kDelayedProtection, DLYPRT);

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

    template <HrtimControllerId tkControllerId, HrtimTimerId tkTimerId>

    class HrtimTimerInterruptController
    {
    public:
        static constexpr HrtimControllerId skControllerId = tkControllerId;
        static constexpr HrtimTimerId      skTimerId      = tkTimerId;
        using ControllerTraitsT                           = HrtimControllerTraits<tkControllerId>;
        using TimerTraitsT                                = HrtimTimerTraits<tkControllerId, tkTimerId>;

        static void enable_interrupts(const HrtimTimerInterruptConfig& config)
        {
            if (config.interrupts.mask == 0)  // No interrupts enabled
            {
                return;
            }

            if (config.interrupts.compare1)
            {
                HrtimTimerInterruptSourceInterface<skControllerId, skTimerId, HrtimTimerInterruptSource::kCompare1>::
                    enable();
            }

            if (config.interrupts.compare2)
            {
                HrtimTimerInterruptSourceInterface<skControllerId, skTimerId, HrtimTimerInterruptSource::kCompare2>::
                    enable();
            }

            if (config.interrupts.compare3)
            {
                HrtimTimerInterruptSourceInterface<skControllerId, skTimerId, HrtimTimerInterruptSource::kCompare3>::
                    enable();
            }

            if (config.interrupts.compare4)
            {
                HrtimTimerInterruptSourceInterface<skControllerId, skTimerId, HrtimTimerInterruptSource::kCompare4>::
                    enable();
            }

            if (config.interrupts.update)
            {
                HrtimTimerInterruptSourceInterface<skControllerId, skTimerId, HrtimTimerInterruptSource::kUpdate>::
                    enable();
            }

            if (config.interrupts.repetition)
            {
                HrtimTimerInterruptSourceInterface<skControllerId, skTimerId, HrtimTimerInterruptSource::kRepetition>::
                    enable();
            }

            if (config.interrupts.capture1)
            {
                HrtimTimerInterruptSourceInterface<skControllerId, skTimerId, HrtimTimerInterruptSource::kCapture1>::
                    enable();
            }

            if (config.interrupts.capture2)
            {
                HrtimTimerInterruptSourceInterface<skControllerId, skTimerId, HrtimTimerInterruptSource::kCapture2>::
                    enable();
            }

            if (config.interrupts.output1_set)
            {
                HrtimTimerInterruptSourceInterface<skControllerId, skTimerId, HrtimTimerInterruptSource::kOutput1Set>::
                    enable();
            }

            if (config.interrupts.output1_reset)
            {
                HrtimTimerInterruptSourceInterface<skControllerId,
                                                   skTimerId,
                                                   HrtimTimerInterruptSource::kOutput1Reset>::enable();
            }

            if (config.interrupts.output2_set)
            {
                HrtimTimerInterruptSourceInterface<skControllerId, skTimerId, HrtimTimerInterruptSource::kOutput2Set>::
                    enable();
            }

            if (config.interrupts.output2_reset)
            {
                HrtimTimerInterruptSourceInterface<skControllerId,
                                                   skTimerId,
                                                   HrtimTimerInterruptSource::kOutput2Reset>::enable();
            }

            if (config.interrupts.reset)
            {
                HrtimTimerInterruptSourceInterface<skControllerId, skTimerId, HrtimTimerInterruptSource::kReset>::
                    enable();
            }

            if (config.interrupts.delayed_protection)
            {
                HrtimTimerInterruptSourceInterface<skControllerId,
                                                   skTimerId,
                                                   HrtimTimerInterruptSource::kDelayedProtection>::enable();
            }

            NVIC_SetPriority(TimerTraitsT::skIRQn, config.priority);
            NVIC_EnableIRQ(TimerTraitsT::skIRQn);
        }

        static void disable_interrupts()
        {
            HrtimTimerInterruptSourceInterface<skControllerId, skTimerId, HrtimTimerInterruptSource::kCompare1>::
                disable();
            HrtimTimerInterruptSourceInterface<skControllerId, skTimerId, HrtimTimerInterruptSource::kCompare2>::
                disable();
            HrtimTimerInterruptSourceInterface<skControllerId, skTimerId, HrtimTimerInterruptSource::kCompare3>::
                disable();
            HrtimTimerInterruptSourceInterface<skControllerId, skTimerId, HrtimTimerInterruptSource::kCompare4>::
                disable();
            HrtimTimerInterruptSourceInterface<skControllerId, skTimerId, HrtimTimerInterruptSource::kUpdate>::
                disable();
            HrtimTimerInterruptSourceInterface<skControllerId, skTimerId, HrtimTimerInterruptSource::kRepetition>::
                disable();
            HrtimTimerInterruptSourceInterface<skControllerId, skTimerId, HrtimTimerInterruptSource::kCapture1>::
                disable();
            HrtimTimerInterruptSourceInterface<skControllerId, skTimerId, HrtimTimerInterruptSource::kCapture2>::
                disable();
            HrtimTimerInterruptSourceInterface<skControllerId, skTimerId, HrtimTimerInterruptSource::kOutput1Set>::
                disable();
            HrtimTimerInterruptSourceInterface<skControllerId, skTimerId, HrtimTimerInterruptSource::kOutput1Reset>::
                disable();
            HrtimTimerInterruptSourceInterface<skControllerId, skTimerId, HrtimTimerInterruptSource::kOutput2Set>::
                disable();
            HrtimTimerInterruptSourceInterface<skControllerId, skTimerId, HrtimTimerInterruptSource::kOutput2Reset>::
                disable();
            HrtimTimerInterruptSourceInterface<skControllerId, skTimerId, HrtimTimerInterruptSource::kReset>::disable();
            HrtimTimerInterruptSourceInterface<skControllerId,
                                               skTimerId,
                                               HrtimTimerInterruptSource::kDelayedProtection>::disable();

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
     * @tparam tkControllerId HRTIM Controller ID.
     */
    template <HrtimControllerId tkControllerId>

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
     * @tparam tkControllerId HRTIM Controller ID.
     */
    template <HrtimControllerId tkControllerId>

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
     * @tparam tkControllerId HRTIM Controller ID.
     */
    template <HrtimControllerId tkControllerId, HrtimControllerInterruptSource tkIntSource>

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
     * @tparam tkControllerId HRTIM Controller ID.
     */
    template <HrtimControllerId tkControllerId>

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
     * @tparam tkControllerId HRTIM Controller ID.
     */
    template <HrtimControllerId tkControllerId, HrtimFaultInterruptSource tkIntSource>

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
     * @tparam tkControllerId HRTIM Controller ID.
     */
    template <HrtimControllerId tkControllerId>

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
     * @tparam tkControllerId HRTIM Controller ID.
     */
    template <HrtimControllerId tkControllerId, HrtimMasterTimerInterruptSource tkIntSource>

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
     * @tparam tkControllerId HRTIM Controller ID.
     * @tparam tkTimerId HRTIM Timer ID.
     */
    template <HrtimControllerId tkControllerId, HrtimTimerId tkTimerId>

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
     * @tparam tkControllerId The HRTIM peripheral index the interrupt belongs to.
     * @tparam tkTimerId The HRTIM Timer index the interrupt belongs to.
     * @tparam tkIntSource The interrupt type triggered.
     */
    template <HrtimControllerId tkControllerId, HrtimTimerId tkTimerId, HrtimTimerInterruptSource tkIntSource>

    struct HrtimTimerIsrRouter
    {
        using UnboundIsrHandlerTag = void;

        static void handle()
        {
            // Default implementation does nothing.
        }
    };

}  // namespace valle::platform
