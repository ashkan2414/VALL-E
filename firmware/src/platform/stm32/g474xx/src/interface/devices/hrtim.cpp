#include "valle/platform/devices/hrtim.hpp"

#include "magic_enum/magic_enum.hpp"

#if __has_include("app_isr_bindings.hpp")
#include "app_isr_bindings.hpp"
#endif

namespace valle::platform
{

    // -----------------------------------------------------------------------------
    // CONTROLLER IRQ HANDLER
    // -----------------------------------------------------------------------------
    /**
     * @brief Check if any granular ISR handler is bound for this HRTIM controller.
     *
     * @tparam tkControllerID HRTIM Controller ID.
     */
    template <HRTIMControllerID tkControllerID>
        requires(kValidHRTIMControllerID<tkControllerID>)
    [[nodiscard]] consteval static inline bool hrtim_controller_has_any_granular_isr_handler()
    {
        constexpr auto values = magic_enum::enum_values<HRTIMControllerInterruptType>();

        return [values]<std::size_t... Is>(std::index_sequence<Is...>)
        {
            return (CBoundISRRouter<HRTIMControllerISRRouter<tkControllerID, values[Is]>> || ...);
        }(std::make_index_sequence<values.size()>{});
    }

    /**
     * @brief HRTIM Controller Interrupt Handler Router
     *
     * @tparam tkControllerID HRTIM Controller ID.
     */
    template <HRTIMControllerID tkControllerID>
        requires(kValidHRTIMControllerID<tkControllerID>)
    static inline void hrtim_controller_irq_handler()
    {
        using GlobalRouterT               = HRTIMGlobalControllerISRRouter<tkControllerID>;
        constexpr bool kHasGlobalRouter   = CBoundISRRouter<GlobalRouterT>;
        constexpr bool kHasGranularRouter = hrtim_controller_has_any_granular_isr_handler<tkControllerID>();
        static_assert(!(kHasGlobalRouter && kHasGranularRouter), "VALLE CONFLICT: Global and Granular ISRs detected.");

        if constexpr (kHasGlobalRouter)
        {
            GlobalRouterT::handle();
            return;
        }

#define HANDLE_HRTIM_INT(tkIntType)                                                  \
    {                                                                                \
        using RouterT = HRTIMControllerISRRouter<tkControllerID, (tkIntType)>;       \
        using TraitsT = HRTIMControllerInterruptTraits<tkControllerID, (tkIntType)>; \
        if constexpr (CBoundISRRouter<RouterT>)                                      \
        {                                                                            \
            if (TraitsT::is_pending())                                               \
            {                                                                        \
                if constexpr (kISRRouterConfigAck<RouterT>)                          \
                {                                                                    \
                    TraitsT::ack();                                                  \
                }                                                                    \
                RouterT::handle();                                                   \
            }                                                                        \
        }                                                                            \
    }

        // Check all possible interrupts for this HRTIM Timer
        HANDLE_HRTIM_INT(HRTIMControllerInterruptType::kDLLReady);
        HANDLE_HRTIM_INT(HRTIMControllerInterruptType::kBurstModePeriod);
        HANDLE_HRTIM_INT(HRTIMControllerInterruptType::kSync);
#undef HANDLE_HRTIM_INT
    }

    // -----------------------------------------------------------------------------
    // FAULT IRQ HANDLER
    // -----------------------------------------------------------------------------
    /**
     * @brief Check if any granular ISR handler is bound for this HRTIM Fault.
     *
     * @tparam tkControllerID HRTIM Controller ID.
     */
    template <HRTIMControllerID tkControllerID>
        requires(kValidHRTIMControllerID<tkControllerID>)
    [[nodiscard]] consteval static inline bool hrtim_fault_has_any_granular_isr_handler()
    {
        constexpr auto values = magic_enum::enum_values<HRTIMFaultInterruptType>();

        return [values]<std::size_t... Is>(std::index_sequence<Is...>)
        {
            return (CBoundISRRouter<HRTIMFaultISRRouter<tkControllerID, values[Is]>> || ...);
        }(std::make_index_sequence<values.size()>{});
    }

    /**
     * @brief HRTIM Fault Interrupt Handler Router
     *
     * @tparam tkControllerID HRTIM Controller ID.
     */
    template <HRTIMControllerID tkControllerID>
        requires(kValidHRTIMControllerID<tkControllerID>)
    static inline void hrtim_fault_irq_handler()
    {
        using GlobalRouterT               = HRTIMGlobalFaultISRRouter<tkControllerID>;
        constexpr bool kHasGlobalRouter   = CBoundISRRouter<GlobalRouterT>;
        constexpr bool kHasGranularRouter = hrtim_fault_has_any_granular_isr_handler<tkControllerID>();
        static_assert(!(kHasGlobalRouter && kHasGranularRouter), "VALLE CONFLICT: Global and Granular ISRs detected.");

        if constexpr (kHasGlobalRouter)
        {
            GlobalRouterT::handle();
            return;
        }

#define HANDLE_HRTIM_INT(tkIntType)                                             \
    {                                                                           \
        using RouterT = HRTIMFaultISRRouter<tkControllerID, (tkIntType)>;       \
        using TraitsT = HRTIMFaultInterruptTraits<tkControllerID, (tkIntType)>; \
        if constexpr (CBoundISRRouter<RouterT>)                                 \
        {                                                                       \
            if (TraitsT::is_pending())                                          \
            {                                                                   \
                if constexpr (kISRRouterConfigAck<RouterT>)                     \
                {                                                               \
                    TraitsT::ack();                                             \
                }                                                               \
                RouterT::handle();                                              \
            }                                                                   \
        }                                                                       \
    }

        // Check all possible interrupts for this HRTIM Timer
        HANDLE_HRTIM_INT(HRTIMFaultInterruptType::kSystemFault);
        HANDLE_HRTIM_INT(HRTIMFaultInterruptType::kFault1);
        HANDLE_HRTIM_INT(HRTIMFaultInterruptType::kFault2);
        HANDLE_HRTIM_INT(HRTIMFaultInterruptType::kFault3);
        HANDLE_HRTIM_INT(HRTIMFaultInterruptType::kFault4);
        HANDLE_HRTIM_INT(HRTIMFaultInterruptType::kFault5);
        HANDLE_HRTIM_INT(HRTIMFaultInterruptType::kFault6);
#undef HANDLE_HRTIM_INT
    }

    // -----------------------------------------------------------------------------
    // MASTER TIMER IRQ HANDLER
    // -----------------------------------------------------------------------------
    /**
     * @brief Check if any granular ISR handler is bound for this HRTIM Master Timer.
     *
     * @tparam tkControllerID HRTIM Controller ID.
     */
    template <HRTIMControllerID tkControllerID>
        requires(kValidHRTIMControllerID<tkControllerID>)
    [[nodiscard]] consteval static inline bool hrtim_master_timer_has_any_granular_isr_handler()
    {
        constexpr auto values = magic_enum::enum_values<HRTIMMasterTimerInterruptType>();

        return [values]<std::size_t... Is>(std::index_sequence<Is...>)
        {
            return (CBoundISRRouter<HRTIMMasterTimerISRRouter<tkControllerID, values[Is]>> || ...);
        }(std::make_index_sequence<values.size()>{});
    }

    /**
     * @brief HRTIM Master Timer Interrupt Handler Router
     *
     * @tparam tkControllerID HRTIM Controller ID.
     */
    template <HRTIMControllerID tkControllerID>
        requires(kValidHRTIMControllerID<tkControllerID>)
    static inline void hrtim_master_timer_irq_handler()
    {
        using GlobalRouterT               = HRTIMGlobalMasterTimerISRRouter<tkControllerID>;
        constexpr bool kHasGlobalRouter   = CBoundISRRouter<GlobalRouterT>;
        constexpr bool kHasGranularRouter = hrtim_master_timer_has_any_granular_isr_handler<tkControllerID>();
        static_assert(!(kHasGlobalRouter && kHasGranularRouter), "VALLE CONFLICT: Global and Granular ISRs detected.");

        if constexpr (kHasGlobalRouter)
        {
            GlobalRouterT::handle();
            return;
        }

#define HANDLE_HRTIM_INT(tkIntType)                                                   \
    {                                                                                 \
        using RouterT = HRTIMMasterTimerISRRouter<tkControllerID, (tkIntType)>;       \
        using TraitsT = HRTIMMasterTimerInterruptTraits<tkControllerID, (tkIntType)>; \
        if constexpr (CBoundISRRouter<RouterT>)                                       \
        {                                                                             \
            if (TraitsT::is_pending())                                                \
            {                                                                         \
                if constexpr (kISRRouterConfigAck<RouterT>)                           \
                {                                                                     \
                    TraitsT::ack();                                                   \
                }                                                                     \
                RouterT::handle();                                                    \
            }                                                                         \
        }                                                                             \
    }

        // Check all possible interrupts for this HRTIM Timer
        HANDLE_HRTIM_INT(HRTIMMasterTimerInterruptType::kCompare1);
        HANDLE_HRTIM_INT(HRTIMMasterTimerInterruptType::kCompare2);
        HANDLE_HRTIM_INT(HRTIMMasterTimerInterruptType::kCompare3);
        HANDLE_HRTIM_INT(HRTIMMasterTimerInterruptType::kCompare4);
        HANDLE_HRTIM_INT(HRTIMMasterTimerInterruptType::kUpdate);
        HANDLE_HRTIM_INT(HRTIMMasterTimerInterruptType::kRepetition);
#undef HANDLE_HRTIM_INT
    }

    // -----------------------------------------------------------------------------
    // MASTER IRQ HANDLER
    // -----------------------------------------------------------------------------
    template <HRTIMControllerID tkControllerID>
        requires(kValidHRTIMControllerID<tkControllerID>)
    [[nodiscard]] consteval static inline bool hrtim_master_has_any_granular_isr_handler()
    {
        return CBoundISRRouter<HRTIMGlobalMasterTimerISRRouter<tkControllerID>> ||
               CBoundISRRouter<HRTIMGlobalControllerISRRouter<tkControllerID>> ||
               hrtim_master_timer_has_any_granular_isr_handler<tkControllerID>() ||
               hrtim_controller_has_any_granular_isr_handler<tkControllerID>();
    }

    /**
     * @brief HRTIM Master Interrupt Handler Router
     *
     * @tparam tkControllerID HRTIM Controller ID.
     */
    template <HRTIMControllerID tkControllerID>
        requires(kValidHRTIMControllerID<tkControllerID>)
    static inline void hrtim_master_irq_handler()
    {
        using GlobalRouterT               = HRTIMGlobalMasterISRRouter<tkControllerID>;
        constexpr bool kHasGlobalRouter   = CBoundISRRouter<GlobalRouterT>;
        constexpr bool kHasGranularRouter = hrtim_master_has_any_granular_isr_handler<tkControllerID>();

        static_assert(!(kHasGlobalRouter && kHasGranularRouter), "VALLE CONFLICT: Global and Granular ISRs detected.");

        if constexpr (kHasGlobalRouter)
        {
            GlobalRouterT::handle();
            return;
        }

        hrtim_controller_irq_handler<tkControllerID>();
        hrtim_master_timer_irq_handler<tkControllerID>();
    }

    // -----------------------------------------------------------------------------
    // TIMER IRQ HANDLER
    // -----------------------------------------------------------------------------

    /**
     * @brief Check if any granular ISR handler is bound for this HRTIM Timer.
     *
     * @tparam tkControllerID HRTIM Controller ID.
     * @tparam tkTimerID      HRTIM Timer ID.
     */
    template <HRTIMControllerID tkControllerID, HRTIMTimerID tkTimerID>
        requires(kValidHRTIMControllerID<tkControllerID>)
    [[nodiscard]] consteval static inline bool hrtim_timer_has_any_granular_isr_handler()
    {
        constexpr auto values = magic_enum::enum_values<HRTIMTimerInterruptType>();

        return [values]<std::size_t... Is>(std::index_sequence<Is...>)
        {
            return (CBoundISRRouter<HRTIMTimerISRRouter<tkControllerID, tkTimerID, values[Is]>> || ...);
        }(std::make_index_sequence<values.size()>{});
    }

    /**
     * @brief HRTIM Timer Interrupt Handler Router
     *
     * @tparam tkControllerID HRTIM Controller ID.
     * @tparam tkTimerID      HRTIM Timer ID.
     */
    template <HRTIMControllerID tkControllerID, HRTIMTimerID tkTimerID>
        requires(kValidHRTIMControllerID<tkControllerID>)
    static inline void hrtim_timer_irq_handler()
    {
        using GlobalRouterT               = HRTIMGlobalTimerISRRouter<tkControllerID, tkTimerID>;
        constexpr bool kHasGlobalRouter   = CBoundISRRouter<GlobalRouterT>;
        constexpr bool kHasGranularRouter = hrtim_timer_has_any_granular_isr_handler<tkControllerID, tkTimerID>();
        static_assert(!(kHasGlobalRouter && kHasGranularRouter), "VALLE CONFLICT: Global and Granular ISRs detected.");

        if constexpr (kHasGlobalRouter)
        {
            GlobalRouterT::handle();
            return;
        }

#define HANDLE_HRTIM_INT(tkIntType)                                                        \
    {                                                                                      \
        using RouterT = HRTIMTimerISRRouter<tkControllerID, tkTimerID, (tkIntType)>;       \
        using TraitsT = HRTIMTimerInterruptTraits<tkControllerID, tkTimerID, (tkIntType)>; \
        if constexpr (CBoundISRRouter<RouterT>)                                            \
        {                                                                                  \
            if (TraitsT::is_pending())                                                     \
            {                                                                              \
                if constexpr (kISRRouterConfigAck<RouterT>)                                \
                {                                                                          \
                    TraitsT::ack();                                                        \
                }                                                                          \
                RouterT::handle();                                                         \
            }                                                                              \
        }                                                                                  \
    }

        // Check all possible interrupts for this HRTIM Timer
        HANDLE_HRTIM_INT(HRTIMTimerInterruptType::kCompare1);
        HANDLE_HRTIM_INT(HRTIMTimerInterruptType::kCompare2);
        HANDLE_HRTIM_INT(HRTIMTimerInterruptType::kCompare3);
        HANDLE_HRTIM_INT(HRTIMTimerInterruptType::kCompare4);
        HANDLE_HRTIM_INT(HRTIMTimerInterruptType::kUpdate);
        HANDLE_HRTIM_INT(HRTIMTimerInterruptType::kRepetition);
        HANDLE_HRTIM_INT(HRTIMTimerInterruptType::kCapture1);
        HANDLE_HRTIM_INT(HRTIMTimerInterruptType::kCapture2);
        HANDLE_HRTIM_INT(HRTIMTimerInterruptType::kOutput1Set);
        HANDLE_HRTIM_INT(HRTIMTimerInterruptType::kOutput1Reset);
        HANDLE_HRTIM_INT(HRTIMTimerInterruptType::kOutput2Set);
        HANDLE_HRTIM_INT(HRTIMTimerInterruptType::kOutput2Reset);
        HANDLE_HRTIM_INT(HRTIMTimerInterruptType::kReset);
        HANDLE_HRTIM_INT(HRTIMTimerInterruptType::kDelayedProtection);
#undef HANDLE_HRTIM_INT
    }

    extern "C"
    {
        // --- MASTER TIMER ---
        void HRTIM1_MASTER_IRQHandler(void)  // NOLINT(readability-identifier-naming)
        {
            hrtim_master_irq_handler<1>();
        }

        // --- FAULT ---
        void HRTIM1_FLT_IRQHandler(void)  // NOLINT(readability-identifier-naming)
        {
            hrtim_fault_irq_handler<1>();
        }

        // --- TIMER A ---
        void HRTIM1_TIMA_IRQHandler(void)  // NOLINT(readability-identifier-naming)
        {
            hrtim_timer_irq_handler<1, HRTIMTimerID::kA>();
        }

        // --- TIMER B ---
        void HRTIM1_TIMB_IRQHandler(void)  // NOLINT(readability-identifier-naming)
        {
            hrtim_timer_irq_handler<1, HRTIMTimerID::kB>();
        }

        // --- TIMER C ---
        void HRTIM1_TIMC_IRQHandler(void)  // NOLINT(readability-identifier-naming)
        {
            hrtim_timer_irq_handler<1, HRTIMTimerID::kC>();
        }

        // --- TIMER D ---
        void HRTIM1_TIMD_IRQHandler(void)  // NOLINT(readability-identifier-naming)
        {
            hrtim_timer_irq_handler<1, HRTIMTimerID::kD>();
        }

        // --- TIMER E ---
        void HRTIM1_TIME_IRQHandler(void)  // NOLINT(readability-identifier-naming)
        {
            hrtim_timer_irq_handler<1, HRTIMTimerID::kE>();
        }

        // --- TIMER F ---
        void HRTIM1_TIMF_IRQHandler(void)  // NOLINT(readability-identifier-naming)
        {
            hrtim_timer_irq_handler<1, HRTIMTimerID::kF>();
        }
    }

}  // namespace valle::platform