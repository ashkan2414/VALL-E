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
     * @tparam tkPeripheralId HRTIM Peripheral ID.
     */
    template <HrtimPeripheralId tkPeripheralId>
    [[nodiscard]] consteval static inline bool hrtim_controller_has_any_granular_isr_handler()
    {
        constexpr auto values = magic_enum::enum_values<HrtimControllerInterruptType>();

        return [values]<std::size_t... Is>(std::index_sequence<Is...>)
        {
            return (CBoundIsrRouter<HrtimControllerIsrRouter<tkPeripheralId, values[Is]>> || ...);
        }(std::make_index_sequence<values.size()>{});
    }

    /**
     * @brief HRTIM Controller Interrupt Handler Router
     *
     * @tparam tkPeripheralId HRTIM Peripheral ID.
     */
    template <HrtimPeripheralId tkPeripheralId>
    static inline void hrtim_controller_irq_handler()
    {
        using GlobalRouterT               = HrtimGlobalControllerIsrRouter<tkPeripheralId>;
        constexpr bool kHasGlobalRouter   = CBoundIsrRouter<GlobalRouterT>;
        constexpr bool kHasGranularRouter = hrtim_controller_has_any_granular_isr_handler<tkPeripheralId>();
        static_assert(!(kHasGlobalRouter && kHasGranularRouter), "VALLE CONFLICT: Global and Granular ISRs detected.");

        if constexpr (kHasGlobalRouter)
        {
            GlobalRouterT::handle();
            return;
        }

#define HANDLE_HRTIM_INT(tkIntType)                                                  \
    {                                                                                \
        using RouterT = HrtimControllerIsrRouter<tkPeripheralId, (tkIntType)>;       \
        using TraitsT = HrtimControllerInterruptTraits<tkPeripheralId, (tkIntType)>; \
        if constexpr (CBoundIsrRouter<RouterT>)                                      \
        {                                                                            \
            if (TraitsT::is_pending())                                               \
            {                                                                        \
                if constexpr (kIsrRouterConfigAck<RouterT>)                          \
                {                                                                    \
                    TraitsT::ack();                                                  \
                }                                                                    \
                RouterT::handle();                                                   \
            }                                                                        \
        }                                                                            \
    }

        // Check all possible interrupts for this HRTIM Timer
        HANDLE_HRTIM_INT(HrtimControllerInterruptType::kDLLReady);
        HANDLE_HRTIM_INT(HrtimControllerInterruptType::kBurstModePeriod);
        HANDLE_HRTIM_INT(HrtimControllerInterruptType::kSync);
#undef HANDLE_HRTIM_INT
    }

    // -----------------------------------------------------------------------------
    // FAULT IRQ HANDLER
    // -----------------------------------------------------------------------------
    /**
     * @brief Check if any granular ISR handler is bound for this HRTIM Fault.
     *
     * @tparam tkPeripheralId HRTIM Peripheral ID.
     */
    template <HrtimPeripheralId tkPeripheralId>

    [[nodiscard]] consteval static inline bool hrtim_fault_has_any_granular_isr_handler()
    {
        constexpr auto values = magic_enum::enum_values<HrtimFaultInterruptType>();

        return [values]<std::size_t... Is>(std::index_sequence<Is...>)
        {
            return (CBoundIsrRouter<HrtimFaultIsrRouter<tkPeripheralId, values[Is]>> || ...);
        }(std::make_index_sequence<values.size()>{});
    }

    /**
     * @brief HRTIM Fault Interrupt Handler Router
     *
     * @tparam tkPeripheralId HRTIM Peripheral ID.
     */
    template <HrtimPeripheralId tkPeripheralId>

    static inline void hrtim_fault_irq_handler()
    {
        using GlobalRouterT               = HrtimGlobalFaultIsrRouter<tkPeripheralId>;
        constexpr bool kHasGlobalRouter   = CBoundIsrRouter<GlobalRouterT>;
        constexpr bool kHasGranularRouter = hrtim_fault_has_any_granular_isr_handler<tkPeripheralId>();
        static_assert(!(kHasGlobalRouter && kHasGranularRouter), "VALLE CONFLICT: Global and Granular ISRs detected.");

        if constexpr (kHasGlobalRouter)
        {
            GlobalRouterT::handle();
            return;
        }

#define HANDLE_HRTIM_INT(tkIntType)                                             \
    {                                                                           \
        using RouterT = HrtimFaultIsrRouter<tkPeripheralId, (tkIntType)>;       \
        using TraitsT = HrtimFaultInterruptTraits<tkPeripheralId, (tkIntType)>; \
        if constexpr (CBoundIsrRouter<RouterT>)                                 \
        {                                                                       \
            if (TraitsT::is_pending())                                          \
            {                                                                   \
                if constexpr (kIsrRouterConfigAck<RouterT>)                     \
                {                                                               \
                    TraitsT::ack();                                             \
                }                                                               \
                RouterT::handle();                                              \
            }                                                                   \
        }                                                                       \
    }

        // Check all possible interrupts for this HRTIM Timer
        HANDLE_HRTIM_INT(HrtimFaultInterruptType::kSystemFault);
        HANDLE_HRTIM_INT(HrtimFaultInterruptType::kFault1);
        HANDLE_HRTIM_INT(HrtimFaultInterruptType::kFault2);
        HANDLE_HRTIM_INT(HrtimFaultInterruptType::kFault3);
        HANDLE_HRTIM_INT(HrtimFaultInterruptType::kFault4);
        HANDLE_HRTIM_INT(HrtimFaultInterruptType::kFault5);
        HANDLE_HRTIM_INT(HrtimFaultInterruptType::kFault6);
#undef HANDLE_HRTIM_INT
    }

    // -----------------------------------------------------------------------------
    // MASTER TIMER IRQ HANDLER
    // -----------------------------------------------------------------------------
    /**
     * @brief Check if any granular ISR handler is bound for this HRTIM Master Timer.
     *
     * @tparam tkPeripheralId HRTIM Peripheral ID.
     */
    template <HrtimPeripheralId tkPeripheralId>

    [[nodiscard]] consteval static inline bool hrtim_master_timer_has_any_granular_isr_handler()
    {
        constexpr auto values = magic_enum::enum_values<HrtimMasterTimerInterruptType>();

        return [values]<std::size_t... Is>(std::index_sequence<Is...>)
        {
            return (CBoundIsrRouter<HrtimMasterTimerIsrRouter<tkPeripheralId, values[Is]>> || ...);
        }(std::make_index_sequence<values.size()>{});
    }

    /**
     * @brief HRTIM Master Timer Interrupt Handler Router
     *
     * @tparam tkPeripheralId HRTIM Peripheral ID.
     */
    template <HrtimPeripheralId tkPeripheralId>

    static inline void hrtim_master_timer_irq_handler()
    {
        using GlobalRouterT               = HrtimGlobalMasterTimerIsrRouter<tkPeripheralId>;
        constexpr bool kHasGlobalRouter   = CBoundIsrRouter<GlobalRouterT>;
        constexpr bool kHasGranularRouter = hrtim_master_timer_has_any_granular_isr_handler<tkPeripheralId>();
        static_assert(!(kHasGlobalRouter && kHasGranularRouter), "VALLE CONFLICT: Global and Granular ISRs detected.");

        if constexpr (kHasGlobalRouter)
        {
            GlobalRouterT::handle();
            return;
        }

#define HANDLE_HRTIM_INT(tkIntType)                                                   \
    {                                                                                 \
        using RouterT = HrtimMasterTimerIsrRouter<tkPeripheralId, (tkIntType)>;       \
        using TraitsT = HrtimMasterTimerInterruptTraits<tkPeripheralId, (tkIntType)>; \
        if constexpr (CBoundIsrRouter<RouterT>)                                       \
        {                                                                             \
            if (TraitsT::is_pending())                                                \
            {                                                                         \
                if constexpr (kIsrRouterConfigAck<RouterT>)                           \
                {                                                                     \
                    TraitsT::ack();                                                   \
                }                                                                     \
                RouterT::handle();                                                    \
            }                                                                         \
        }                                                                             \
    }

        // Check all possible interrupts for this HRTIM Timer
        HANDLE_HRTIM_INT(HrtimMasterTimerInterruptType::kCompare1);
        HANDLE_HRTIM_INT(HrtimMasterTimerInterruptType::kCompare2);
        HANDLE_HRTIM_INT(HrtimMasterTimerInterruptType::kCompare3);
        HANDLE_HRTIM_INT(HrtimMasterTimerInterruptType::kCompare4);
        HANDLE_HRTIM_INT(HrtimMasterTimerInterruptType::kUpdate);
        HANDLE_HRTIM_INT(HrtimMasterTimerInterruptType::kRepetition);
#undef HANDLE_HRTIM_INT
    }

    // -----------------------------------------------------------------------------
    // MASTER IRQ HANDLER
    // -----------------------------------------------------------------------------
    template <HrtimPeripheralId tkPeripheralId>

    [[nodiscard]] consteval static inline bool hrtim_master_has_any_granular_isr_handler()
    {
        return CBoundIsrRouter<HrtimGlobalMasterTimerIsrRouter<tkPeripheralId>> ||
               CBoundIsrRouter<HrtimGlobalControllerIsrRouter<tkPeripheralId>> ||
               hrtim_master_timer_has_any_granular_isr_handler<tkPeripheralId>() ||
               hrtim_controller_has_any_granular_isr_handler<tkPeripheralId>();
    }

    /**
     * @brief HRTIM Master Interrupt Handler Router
     *
     * @tparam tkPeripheralId HRTIM Peripheral ID.
     */
    template <HrtimPeripheralId tkPeripheralId>

    static inline void hrtim_master_irq_handler()
    {
        using GlobalRouterT               = HrtimGlobalMasterIsrRouter<tkPeripheralId>;
        constexpr bool kHasGlobalRouter   = CBoundIsrRouter<GlobalRouterT>;
        constexpr bool kHasGranularRouter = hrtim_master_has_any_granular_isr_handler<tkPeripheralId>();

        static_assert(!(kHasGlobalRouter && kHasGranularRouter), "VALLE CONFLICT: Global and Granular ISRs detected.");

        if constexpr (kHasGlobalRouter)
        {
            GlobalRouterT::handle();
            return;
        }

        hrtim_controller_irq_handler<tkPeripheralId>();
        hrtim_master_timer_irq_handler<tkPeripheralId>();
    }

    // -----------------------------------------------------------------------------
    // TIMER IRQ HANDLER
    // -----------------------------------------------------------------------------

    /**
     * @brief Check if any granular ISR handler is bound for this HRTIM Timer.
     *
     * @tparam tkPeripheralId HRTIM Peripheral ID.
     * @tparam tkTimerId      HRTIM Timer ID.
     */
    template <HrtimPeripheralId tkPeripheralId, HrtimTimerId tkTimerId>

    [[nodiscard]] consteval static inline bool hrtim_timer_has_any_granular_isr_handler()
    {
        constexpr auto values = magic_enum::enum_values<HrtimTimerInterruptType>();

        return [values]<std::size_t... Is>(std::index_sequence<Is...>)
        {
            return (CBoundIsrRouter<HrtimTimerIsrRouter<tkPeripheralId, tkTimerId, values[Is]>> || ...);
        }(std::make_index_sequence<values.size()>{});
    }

    /**
     * @brief HRTIM Timer Interrupt Handler Router
     *
     * @tparam tkPeripheralId HRTIM Peripheral ID.
     * @tparam tkTimerId      HRTIM Timer ID.
     */
    template <HrtimPeripheralId tkPeripheralId, HrtimTimerId tkTimerId>

    static inline void hrtim_timer_irq_handler()
    {
        using GlobalRouterT               = HrtimGlobalTimerIsrRouter<tkPeripheralId, tkTimerId>;
        constexpr bool kHasGlobalRouter   = CBoundIsrRouter<GlobalRouterT>;
        constexpr bool kHasGranularRouter = hrtim_timer_has_any_granular_isr_handler<tkPeripheralId, tkTimerId>();
        static_assert(!(kHasGlobalRouter && kHasGranularRouter), "VALLE CONFLICT: Global and Granular ISRs detected.");

        if constexpr (kHasGlobalRouter)
        {
            GlobalRouterT::handle();
            return;
        }

#define HANDLE_HRTIM_INT(tkIntType)                                                        \
    {                                                                                      \
        using RouterT = HrtimTimerIsrRouter<tkPeripheralId, tkTimerId, (tkIntType)>;       \
        using TraitsT = HrtimTimerInterruptTraits<tkPeripheralId, tkTimerId, (tkIntType)>; \
        if constexpr (CBoundIsrRouter<RouterT>)                                            \
        {                                                                                  \
            if (TraitsT::is_pending())                                                     \
            {                                                                              \
                if constexpr (kIsrRouterConfigAck<RouterT>)                                \
                {                                                                          \
                    TraitsT::ack();                                                        \
                }                                                                          \
                RouterT::handle();                                                         \
            }                                                                              \
        }                                                                                  \
    }

        // Check all possible interrupts for this HRTIM Timer
        HANDLE_HRTIM_INT(HrtimTimerInterruptType::kCompare1);
        HANDLE_HRTIM_INT(HrtimTimerInterruptType::kCompare2);
        HANDLE_HRTIM_INT(HrtimTimerInterruptType::kCompare3);
        HANDLE_HRTIM_INT(HrtimTimerInterruptType::kCompare4);
        HANDLE_HRTIM_INT(HrtimTimerInterruptType::kUpdate);
        HANDLE_HRTIM_INT(HrtimTimerInterruptType::kRepetition);
        HANDLE_HRTIM_INT(HrtimTimerInterruptType::kCapture1);
        HANDLE_HRTIM_INT(HrtimTimerInterruptType::kCapture2);
        HANDLE_HRTIM_INT(HrtimTimerInterruptType::kOutput1Set);
        HANDLE_HRTIM_INT(HrtimTimerInterruptType::kOutput1Reset);
        HANDLE_HRTIM_INT(HrtimTimerInterruptType::kOutput2Set);
        HANDLE_HRTIM_INT(HrtimTimerInterruptType::kOutput2Reset);
        HANDLE_HRTIM_INT(HrtimTimerInterruptType::kReset);
        HANDLE_HRTIM_INT(HrtimTimerInterruptType::kDelayedProtection);
#undef HANDLE_HRTIM_INT
    }

    extern "C"
    {
        // --- MASTER TIMER ---
        void HRTIM1_MASTER_IRQHandler(void)  // NOLINT(readability-identifier-naming)
        {
            hrtim_master_irq_handler<HrtimPeripheralId::kHrtim1>();
        }

        // --- FAULT ---
        void HRTIM1_FLT_IRQHandler(void)  // NOLINT(readability-identifier-naming)
        {
            hrtim_fault_irq_handler<HrtimPeripheralId::kHrtim1>();
        }

        // --- TIMER A ---
        void HRTIM1_TIMA_IRQHandler(void)  // NOLINT(readability-identifier-naming)
        {
            hrtim_timer_irq_handler<HrtimPeripheralId::kHrtim1, HrtimTimerId::kTimerA>();
        }

        // --- TIMER B ---
        void HRTIM1_TIMB_IRQHandler(void)  // NOLINT(readability-identifier-naming)
        {
            hrtim_timer_irq_handler<HrtimPeripheralId::kHrtim1, HrtimTimerId::kTimerB>();
        }

        // --- TIMER C ---
        void HRTIM1_TIMC_IRQHandler(void)  // NOLINT(readability-identifier-naming)
        {
            hrtim_timer_irq_handler<HrtimPeripheralId::kHrtim1, HrtimTimerId::kTimerC>();
        }

        // --- TIMER D ---
        void HRTIM1_TIMD_IRQHandler(void)  // NOLINT(readability-identifier-naming)
        {
            hrtim_timer_irq_handler<HrtimPeripheralId::kHrtim1, HrtimTimerId::kTimerD>();
        }

        // --- TIMER E ---
        void HRTIM1_TIME_IRQHandler(void)  // NOLINT(readability-identifier-naming)
        {
            hrtim_timer_irq_handler<HrtimPeripheralId::kHrtim1, HrtimTimerId::kTimerE>();
        }

        // --- TIMER F ---
        void HRTIM1_TIMF_IRQHandler(void)  // NOLINT(readability-identifier-naming)
        {
            hrtim_timer_irq_handler<HrtimPeripheralId::kHrtim1, HrtimTimerId::kTimerF>();
        }
    }

}  // namespace valle::platform
