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
     * @tparam tkControllerId HRTIM Controller ID.
     */
    template <HrtimControllerId tkControllerId>
    [[nodiscard]] consteval static inline bool hrtim_controller_has_any_granular_isr_handler()
    {
        constexpr auto values = magic_enum::enum_values<HrtimControllerInterruptSource>();

        return [values]<std::size_t... Is>(std::index_sequence<Is...>)
        {
            return (CBoundIsrRouter<HrtimControllerIsrRouter<tkControllerId, values[Is]>> || ...);
        }(std::make_index_sequence<values.size()>{});
    }

    /**
     * @brief HRTIM Controller Interrupt Handler Router
     *
     * @tparam tkControllerId HRTIM Controller ID.
     */
    template <HrtimControllerId tkControllerId>
    static inline void hrtim_controller_irq_handler()
    {
        using IrqRouterT                   = HrtimGlobalControllerIsrRouter<tkControllerId>;
        constexpr bool has_irq_handler     = CBoundIsrRouter<IrqRouterT>;
        constexpr bool has_source_handlers = hrtim_controller_has_any_granular_isr_handler<tkControllerId>();
        static_assert(!(has_irq_handler && has_source_handlers),
                      "VALLE CONFLICT: IRQ and Source ISR handlers detected.");

        if constexpr (has_irq_handler)
        {
            IrqRouterT::handle();
            return;
        }

#define HANDLE_HRTIM_INT(tkIntSource)                                                           \
    {                                                                                           \
        using RouterT = HrtimControllerIsrRouter<tkControllerId, (tkIntSource)>;                \
        using TraitsT = HrtimControllerInterruptSourceInterface<tkControllerId, (tkIntSource)>; \
        if constexpr (CBoundIsrRouter<RouterT>)                                                 \
        {                                                                                       \
            if (TraitsT::is_pending())                                                          \
            {                                                                                   \
                if constexpr (kIsrRouterConfigAck<RouterT>)                                     \
                {                                                                               \
                    TraitsT::clear();                                                           \
                }                                                                               \
                RouterT::handle();                                                              \
            }                                                                                   \
        }                                                                                       \
    }

        // Check all possible interrupts for this HRTIM Timer
        HANDLE_HRTIM_INT(HrtimControllerInterruptSource::kDLLReady);
        HANDLE_HRTIM_INT(HrtimControllerInterruptSource::kBurstModePeriod);
        HANDLE_HRTIM_INT(HrtimControllerInterruptSource::kSync);
#undef HANDLE_HRTIM_INT
    }

    // -----------------------------------------------------------------------------
    // FAULT IRQ HANDLER
    // -----------------------------------------------------------------------------
    /**
     * @brief Check if any granular ISR handler is bound for this HRTIM Fault.
     *
     * @tparam tkControllerId HRTIM Controller ID.
     */
    template <HrtimControllerId tkControllerId>

    [[nodiscard]] consteval static inline bool hrtim_fault_has_any_granular_isr_handler()
    {
        constexpr auto values = magic_enum::enum_values<HrtimFaultInterruptSource>();

        return [values]<std::size_t... Is>(std::index_sequence<Is...>)
        {
            return (CBoundIsrRouter<HrtimFaultIsrRouter<tkControllerId, values[Is]>> || ...);
        }(std::make_index_sequence<values.size()>{});
    }

    /**
     * @brief HRTIM Fault Interrupt Handler Router
     *
     * @tparam tkControllerId HRTIM Controller ID.
     */
    template <HrtimControllerId tkControllerId>

    static inline void hrtim_fault_irq_handler()
    {
        using IrqRouterT                   = HrtimGlobalFaultIsrRouter<tkControllerId>;
        constexpr bool has_irq_handler     = CBoundIsrRouter<IrqRouterT>;
        constexpr bool has_source_handlers = hrtim_fault_has_any_granular_isr_handler<tkControllerId>();
        static_assert(!(has_irq_handler && has_source_handlers),
                      "VALLE CONFLICT: IRQ and Source ISR handlers detected.");

        if constexpr (has_irq_handler)
        {
            IrqRouterT::handle();
            return;
        }

#define HANDLE_HRTIM_INT(tkIntSource)                                                      \
    {                                                                                      \
        using RouterT = HrtimFaultIsrRouter<tkControllerId, (tkIntSource)>;                \
        using TraitsT = HrtimFaultInterruptSourceInterface<tkControllerId, (tkIntSource)>; \
        if constexpr (CBoundIsrRouter<RouterT>)                                            \
        {                                                                                  \
            if (TraitsT::is_pending())                                                     \
            {                                                                              \
                if constexpr (kIsrRouterConfigAck<RouterT>)                                \
                {                                                                          \
                    TraitsT::clear();                                                      \
                }                                                                          \
                RouterT::handle();                                                         \
            }                                                                              \
        }                                                                                  \
    }

        // Check all possible interrupts for this HRTIM Timer
        HANDLE_HRTIM_INT(HrtimFaultInterruptSource::kSystemFault);
        HANDLE_HRTIM_INT(HrtimFaultInterruptSource::kFault1);
        HANDLE_HRTIM_INT(HrtimFaultInterruptSource::kFault2);
        HANDLE_HRTIM_INT(HrtimFaultInterruptSource::kFault3);
        HANDLE_HRTIM_INT(HrtimFaultInterruptSource::kFault4);
        HANDLE_HRTIM_INT(HrtimFaultInterruptSource::kFault5);
        HANDLE_HRTIM_INT(HrtimFaultInterruptSource::kFault6);
#undef HANDLE_HRTIM_INT
    }

    // -----------------------------------------------------------------------------
    // MASTER TIMER IRQ HANDLER
    // -----------------------------------------------------------------------------
    /**
     * @brief Check if any granular ISR handler is bound for this HRTIM Master Timer.
     *
     * @tparam tkControllerId HRTIM Controller ID.
     */
    template <HrtimControllerId tkControllerId>

    [[nodiscard]] consteval static inline bool hrtim_master_timer_has_any_granular_isr_handler()
    {
        constexpr auto values = magic_enum::enum_values<HrtimMasterTimerInterruptSource>();

        return [values]<std::size_t... Is>(std::index_sequence<Is...>)
        {
            return (CBoundIsrRouter<HrtimMasterTimerIsrRouter<tkControllerId, values[Is]>> || ...);
        }(std::make_index_sequence<values.size()>{});
    }

    /**
     * @brief HRTIM Master Timer Interrupt Handler Router
     *
     * @tparam tkControllerId HRTIM Controller ID.
     */
    template <HrtimControllerId tkControllerId>

    static inline void hrtim_master_timer_irq_handler()
    {
        using IrqRouterT                   = HrtimGlobalMasterTimerIsrRouter<tkControllerId>;
        constexpr bool has_irq_handler     = CBoundIsrRouter<IrqRouterT>;
        constexpr bool has_source_handlers = hrtim_master_timer_has_any_granular_isr_handler<tkControllerId>();
        static_assert(!(has_irq_handler && has_source_handlers),
                      "VALLE CONFLICT: IRQ and Source ISR handlers detected.");

        if constexpr (has_irq_handler)
        {
            IrqRouterT::handle();
            return;
        }

#define HANDLE_HRTIM_INT(tkIntSource)                                                            \
    {                                                                                            \
        using RouterT = HrtimMasterTimerIsrRouter<tkControllerId, (tkIntSource)>;                \
        using TraitsT = HrtimMasterTimerInterruptSourceInterface<tkControllerId, (tkIntSource)>; \
        if constexpr (CBoundIsrRouter<RouterT>)                                                  \
        {                                                                                        \
            if (TraitsT::is_pending())                                                           \
            {                                                                                    \
                if constexpr (kIsrRouterConfigAck<RouterT>)                                      \
                {                                                                                \
                    TraitsT::clear();                                                            \
                }                                                                                \
                RouterT::handle();                                                               \
            }                                                                                    \
        }                                                                                        \
    }

        // Check all possible interrupts for this HRTIM Timer
        HANDLE_HRTIM_INT(HrtimMasterTimerInterruptSource::kCompare1);
        HANDLE_HRTIM_INT(HrtimMasterTimerInterruptSource::kCompare2);
        HANDLE_HRTIM_INT(HrtimMasterTimerInterruptSource::kCompare3);
        HANDLE_HRTIM_INT(HrtimMasterTimerInterruptSource::kCompare4);
        HANDLE_HRTIM_INT(HrtimMasterTimerInterruptSource::kUpdate);
        HANDLE_HRTIM_INT(HrtimMasterTimerInterruptSource::kRepetition);
#undef HANDLE_HRTIM_INT
    }

    // -----------------------------------------------------------------------------
    // MASTER IRQ HANDLER
    // -----------------------------------------------------------------------------
    template <HrtimControllerId tkControllerId>

    [[nodiscard]] consteval static inline bool hrtim_master_has_any_granular_isr_handler()
    {
        return CBoundIsrRouter<HrtimGlobalMasterTimerIsrRouter<tkControllerId>> ||
               CBoundIsrRouter<HrtimGlobalControllerIsrRouter<tkControllerId>> ||
               hrtim_master_timer_has_any_granular_isr_handler<tkControllerId>() ||
               hrtim_controller_has_any_granular_isr_handler<tkControllerId>();
    }

    /**
     * @brief HRTIM Master Interrupt Handler Router
     *
     * @tparam tkControllerId HRTIM Controller ID.
     */
    template <HrtimControllerId tkControllerId>

    static inline void hrtim_master_irq_handler()
    {
        using IrqRouterT                   = HrtimGlobalMasterIsrRouter<tkControllerId>;
        constexpr bool has_irq_handler     = CBoundIsrRouter<IrqRouterT>;
        constexpr bool has_source_handlers = hrtim_master_has_any_granular_isr_handler<tkControllerId>();

        static_assert(!(has_irq_handler && has_source_handlers),
                      "VALLE CONFLICT: IRQ and Source ISR handlers detected.");

        if constexpr (has_irq_handler)
        {
            IrqRouterT::handle();
            return;
        }

        hrtim_controller_irq_handler<tkControllerId>();
        hrtim_master_timer_irq_handler<tkControllerId>();
    }

    // -----------------------------------------------------------------------------
    // TIMER IRQ HANDLER
    // -----------------------------------------------------------------------------

    /**
     * @brief Check if any granular ISR handler is bound for this HRTIM Timer.
     *
     * @tparam tkControllerId HRTIM Controller ID.
     * @tparam tkTimerId      HRTIM Timer ID.
     */
    template <HrtimControllerId tkControllerId, HrtimTimerId tkTimerId>

    [[nodiscard]] consteval static inline bool hrtim_timer_has_any_granular_isr_handler()
    {
        constexpr auto values = magic_enum::enum_values<HrtimTimerInterruptSource>();

        return [values]<std::size_t... Is>(std::index_sequence<Is...>)
        {
            return (CBoundIsrRouter<HrtimTimerIsrRouter<tkControllerId, tkTimerId, values[Is]>> || ...);
        }(std::make_index_sequence<values.size()>{});
    }

    /**
     * @brief HRTIM Timer Interrupt Handler Router
     *
     * @tparam tkControllerId HRTIM Controller ID.
     * @tparam tkTimerId      HRTIM Timer ID.
     */
    template <HrtimControllerId tkControllerId, HrtimTimerId tkTimerId>

    static inline void hrtim_timer_irq_handler()
    {
        using IrqRouterT                   = HrtimGlobalTimerIsrRouter<tkControllerId, tkTimerId>;
        constexpr bool has_irq_handler     = CBoundIsrRouter<IrqRouterT>;
        constexpr bool has_source_handlers = hrtim_timer_has_any_granular_isr_handler<tkControllerId, tkTimerId>();
        static_assert(!(has_irq_handler && has_source_handlers),
                      "VALLE CONFLICT: IRQ and Source ISR handlers detected.");

        if constexpr (has_irq_handler)
        {
            IrqRouterT::handle();
            return;
        }

#define HANDLE_HRTIM_INT(tkIntSource)                                                                 \
    {                                                                                                 \
        using RouterT = HrtimTimerIsrRouter<tkControllerId, tkTimerId, (tkIntSource)>;                \
        using TraitsT = HrtimTimerInterruptSourceInterface<tkControllerId, tkTimerId, (tkIntSource)>; \
        if constexpr (CBoundIsrRouter<RouterT>)                                                       \
        {                                                                                             \
            if (TraitsT::is_pending())                                                                \
            {                                                                                         \
                if constexpr (kIsrRouterConfigAck<RouterT>)                                           \
                {                                                                                     \
                    TraitsT::clear();                                                                 \
                }                                                                                     \
                RouterT::handle();                                                                    \
            }                                                                                         \
        }                                                                                             \
    }

        // Check all possible interrupts for this HRTIM Timer
        HANDLE_HRTIM_INT(HrtimTimerInterruptSource::kCompare1);
        HANDLE_HRTIM_INT(HrtimTimerInterruptSource::kCompare2);
        HANDLE_HRTIM_INT(HrtimTimerInterruptSource::kCompare3);
        HANDLE_HRTIM_INT(HrtimTimerInterruptSource::kCompare4);
        HANDLE_HRTIM_INT(HrtimTimerInterruptSource::kUpdate);
        HANDLE_HRTIM_INT(HrtimTimerInterruptSource::kRepetition);
        HANDLE_HRTIM_INT(HrtimTimerInterruptSource::kCapture1);
        HANDLE_HRTIM_INT(HrtimTimerInterruptSource::kCapture2);
        HANDLE_HRTIM_INT(HrtimTimerInterruptSource::kOutput1Set);
        HANDLE_HRTIM_INT(HrtimTimerInterruptSource::kOutput1Reset);
        HANDLE_HRTIM_INT(HrtimTimerInterruptSource::kOutput2Set);
        HANDLE_HRTIM_INT(HrtimTimerInterruptSource::kOutput2Reset);
        HANDLE_HRTIM_INT(HrtimTimerInterruptSource::kReset);
        HANDLE_HRTIM_INT(HrtimTimerInterruptSource::kDelayedProtection);
#undef HANDLE_HRTIM_INT
    }

    extern "C"
    {
        // --- MASTER TIMER ---
        void HRTIM1_MASTER_IRQHandler(void)  // NOLINT(readability-identifier-naming)
        {
            hrtim_master_irq_handler<HrtimControllerId::kHrtim1>();
        }

        // --- FAULT ---
        void HRTIM1_FLT_IRQHandler(void)  // NOLINT(readability-identifier-naming)
        {
            hrtim_fault_irq_handler<HrtimControllerId::kHrtim1>();
        }

        // --- TIMER A ---
        void HRTIM1_TIMA_IRQHandler(void)  // NOLINT(readability-identifier-naming)
        {
            hrtim_timer_irq_handler<HrtimControllerId::kHrtim1, HrtimTimerId::kTimerA>();
        }

        // --- TIMER B ---
        void HRTIM1_TIMB_IRQHandler(void)  // NOLINT(readability-identifier-naming)
        {
            hrtim_timer_irq_handler<HrtimControllerId::kHrtim1, HrtimTimerId::kTimerB>();
        }

        // --- TIMER C ---
        void HRTIM1_TIMC_IRQHandler(void)  // NOLINT(readability-identifier-naming)
        {
            hrtim_timer_irq_handler<HrtimControllerId::kHrtim1, HrtimTimerId::kTimerC>();
        }

        // --- TIMER D ---
        void HRTIM1_TIMD_IRQHandler(void)  // NOLINT(readability-identifier-naming)
        {
            hrtim_timer_irq_handler<HrtimControllerId::kHrtim1, HrtimTimerId::kTimerD>();
        }

        // --- TIMER E ---
        void HRTIM1_TIME_IRQHandler(void)  // NOLINT(readability-identifier-naming)
        {
            hrtim_timer_irq_handler<HrtimControllerId::kHrtim1, HrtimTimerId::kTimerE>();
        }

        // --- TIMER F ---
        void HRTIM1_TIMF_IRQHandler(void)  // NOLINT(readability-identifier-naming)
        {
            hrtim_timer_irq_handler<HrtimControllerId::kHrtim1, HrtimTimerId::kTimerF>();
        }
    }

}  // namespace valle::platform
