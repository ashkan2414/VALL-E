#include "Valle/Device/Devices/hrtim.hpp"

#include "magic_enum/magic_enum.hpp"

#if __has_include("app_isr_bindings.hpp")
#include "app_isr_bindings.hpp"
#endif

namespace valle
{
    /**
     * @brief Check if any granular ISR handler is bound for this HRTIM Timer.
     *
     * @tparam tkControllerID HRTIM Controller ID.
     * @tparam tkTimerID      HRTIM Timer ID.
     */
    template <HRTIMControllerID tkControllerID, HRTIMTimerID tkTimerID>
        requires(kValidHRTIMID<tkControllerID>)
    [[nodiscard]] consteval static inline bool hrtim_has_any_granular_isr_handler()
    {
        constexpr auto values = magic_enum::enum_values<HRTIMInterruptType>();

        return [values]<std::size_t... Is>(std::index_sequence<Is...>)
        {
            return (CBoundISRRouter<HRTIMISRRouter<tkControllerID, tkTimerID, values[Is]>> || ...);
        }(std::make_index_sequence<values.size()>{});
    }

    /**
     * @brief HRTIM Interrupt Handler Router
     *
     * @tparam tkControllerID HRTIM Controller ID.
     * @tparam tkTimerID      HRTIM Timer ID.
     */
    template <HRTIMControllerID tkControllerID, HRTIMTimerID tkTimerID>
        requires(kValidHRTIMID<tkControllerID>)
    static inline void hrtim_irq_handler()
    {
        using GlobalRouterT               = HRTIMGlobalISRRouter<tkControllerID, tkTimerID>;
        constexpr bool kHasGlobalRouter   = CBoundISRRouter<GlobalRouterT>;
        constexpr bool kHasGranularRouter = hrtim_has_any_granular_isr_handler<tkControllerID, tkTimerID>();
        static_assert(!(kHasGlobalRouter && kHasGranularRouter), "VALLE CONFLICT: Global and Granular ISRs detected.");

        if constexpr (kHasGlobalRouter)
        {
            GlobalRouterT::handle();
            return;
        }

#define HANDLE_HRTIM_INT(tkIntType)                                                   \
    {                                                                                 \
        using RouterT = HRTIMISRRouter<tkControllerID, tkTimerID, (tkIntType)>;       \
        using TraitsT = HRTIMInterruptTraits<tkControllerID, tkTimerID, (tkIntType)>; \
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
        HANDLE_HRTIM_INT(HRTIMInterruptType::kFault1);
        HANDLE_HRTIM_INT(HRTIMInterruptType::kFault2);
        HANDLE_HRTIM_INT(HRTIMInterruptType::kFault3);
        HANDLE_HRTIM_INT(HRTIMInterruptType::kFault4);
        HANDLE_HRTIM_INT(HRTIMInterruptType::kFault5);
        HANDLE_HRTIM_INT(HRTIMInterruptType::kFault6);
        HANDLE_HRTIM_INT(HRTIMInterruptType::kSystemFault);
        HANDLE_HRTIM_INT(HRTIMInterruptType::kDLLReady);
        HANDLE_HRTIM_INT(HRTIMInterruptType::kBurstModePeriod);
        HANDLE_HRTIM_INT(HRTIMInterruptType::kSync);
        HANDLE_HRTIM_INT(HRTIMInterruptType::kCompare1);
        HANDLE_HRTIM_INT(HRTIMInterruptType::kCompare2);
        HANDLE_HRTIM_INT(HRTIMInterruptType::kCompare3);
        HANDLE_HRTIM_INT(HRTIMInterruptType::kCompare4);
        HANDLE_HRTIM_INT(HRTIMInterruptType::kUpdate);
        HANDLE_HRTIM_INT(HRTIMInterruptType::kRepitition);
        HANDLE_HRTIM_INT(HRTIMInterruptType::kCapture1);
        HANDLE_HRTIM_INT(HRTIMInterruptType::kCapture2);
        HANDLE_HRTIM_INT(HRTIMInterruptType::kOutput1Set);
        HANDLE_HRTIM_INT(HRTIMInterruptType::kOutput1Reset);
        HANDLE_HRTIM_INT(HRTIMInterruptType::kOutput2Set);
        HANDLE_HRTIM_INT(HRTIMInterruptType::kOutput2Reset);
        HANDLE_HRTIM_INT(HRTIMInterruptType::kReset);
        HANDLE_HRTIM_INT(HRTIMInterruptType::kDelayedProtection);
#undef HANDLE_HRTIM_INT
    }

    extern "C"
    {
        // --- TIMER A ---
        void HRTIM1_TIMA_IRQHandler(void)  // NOLINT(readability-identifier-naming)
        {
            hrtim_irq_handler<1, HRTIMTimerID::kA>();
        }

        // --- TIMER B ---
        void HRTIM1_TIMB_IRQHandler(void)  // NOLINT(readability-identifier-naming)
        {
            hrtim_irq_handler<1, HRTIMTimerID::kB>();
        }

        // --- TIMER C ---
        void HRTIM1_TIMC_IRQHandler(void)  // NOLINT(readability-identifier-naming)
        {
            hrtim_irq_handler<1, HRTIMTimerID::kC>();
        }

        // --- TIMER D ---
        void HRTIM1_TIMD_IRQHandler(void)  // NOLINT(readability-identifier-naming)
        {
            hrtim_irq_handler<1, HRTIMTimerID::kD>();
        }

        // --- TIMER E ---
        void HRTIM1_TIME_IRQHandler(void)  // NOLINT(readability-identifier-naming)
        {
            hrtim_irq_handler<1, HRTIMTimerID::kE>();
        }

        // --- TIMER F ---
        void HRTIM1_TIMF_IRQHandler(void)  // NOLINT(readability-identifier-naming)
        {
            hrtim_irq_handler<1, HRTIMTimerID::kF>();
        }
    }

}  // namespace valle