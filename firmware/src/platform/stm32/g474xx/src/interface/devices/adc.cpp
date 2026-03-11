#include "valle/platform/devices/adc.hpp"

#include "magic_enum/magic_enum.hpp"

#if __has_include("app_isr_bindings.hpp")
#include "app_isr_bindings.hpp"
#endif

namespace valle::platform
{
    /**
     * @brief Check if any granular ISR handler is bound for this ADC.
     *
     * @tparam tkControllerID ADC Controller ID.
     */
    template <ADCControllerID tkID>
        requires(kValidADCControllerID<tkID>)
    [[nodiscard]] consteval static inline bool adc_has_any_granular_isr_handler()
    {
        constexpr auto values = magic_enum::enum_values<ADCInterruptType>();

        return [values]<std::size_t... Is>(std::index_sequence<Is...>)
        { return (CBoundISRRouter<ADCISRRouter<tkID, values[Is]>> || ...); }(std::make_index_sequence<values.size()>{});
    }

    /**
     * @brief ADC Interrupt Handler Router
     *
     * @tparam tkControllerID ADC Index (1-5)
     */
    template <ADCControllerID tkControllerID>
        requires(kValidADCControllerID<tkControllerID>)
    static inline void adc_irq_handler()
    {
        using GlobalRouterT               = ADCGlobalISRRouter<tkControllerID>;
        constexpr bool kHasGlobalRouter   = CBoundISRRouter<GlobalRouterT>;
        constexpr bool kHasGranularRouter = adc_has_any_granular_isr_handler<tkControllerID>();
        static_assert(!(kHasGlobalRouter && kHasGranularRouter), "VALLE CONFLICT: Global and Granular ISRs detected.");

        if constexpr (kHasGlobalRouter)
        {
            GlobalRouterT::handle();
            return;
        }

#define HANDLE_ADC_INT(tkIntType)                                      \
    {                                                                  \
        using RouterT = ADCISRRouter<tkControllerID, (tkIntType)>;     \
        using TraitsT = ADCInterruptTraits<tkControllerID, tkIntType>; \
        if constexpr (CBoundISRRouter<RouterT>)                        \
        {                                                              \
            if (TraitsT::is_pending())                                 \
            {                                                          \
                if constexpr (kISRRouterConfigAck<RouterT>)            \
                {                                                      \
                    TraitsT::ack();                                    \
                }                                                      \
                RouterT::handle();                                     \
            }                                                          \
        }                                                              \
        else                                                           \
        {                                                              \
            if constexpr (TraitsT::skShouldClear)                      \
            {                                                          \
                if (TraitsT::is_pending())                             \
                {                                                      \
                    TraitsT::ack();                                    \
                }                                                      \
            }                                                          \
        }                                                              \
    }

        HANDLE_ADC_INT(ADCInterruptType::kReady);
        HANDLE_ADC_INT(ADCInterruptType::kRegularEndOfConversion);
        HANDLE_ADC_INT(ADCInterruptType::kRegularEndOfSequence);
        HANDLE_ADC_INT(ADCInterruptType::kRegularEndOfSampling);
        HANDLE_ADC_INT(ADCInterruptType::kInjectEndOfConversion);
        HANDLE_ADC_INT(ADCInterruptType::kInjectEndOfSequence);
        HANDLE_ADC_INT(ADCInterruptType::kInjectContextQueueOverflow);
        HANDLE_ADC_INT(ADCInterruptType::kOverrun);
        HANDLE_ADC_INT(ADCInterruptType::kAnalogWatchdog1);
        HANDLE_ADC_INT(ADCInterruptType::kAnalogWatchdog2);
        HANDLE_ADC_INT(ADCInterruptType::kAnalogWatchdog3);
#undef HANDLE_ADC_INT
    }

    extern "C"
    {
        void ADC1_2_IRQHandler(void)  // NOLINT(readability-identifier-naming)
        {
            // NOTE: ADC1 and ADC2 share this IRQ vector (RM0440 Section 21.4.20).
            // Each ADC has separate flag registers (ADC1->ISR, ADC2->ISR), so no race condition exists.
            // DO NOT add shared volatile state between ADC1 and ADC2 handlers without proper synchronization!

            // We must check BOTH devices because they share the line.
            // The adc_irq_handler() function does a quick register check,
            // so it's cheap to call even if the ADC isn't active.
            adc_irq_handler<1>();
            adc_irq_handler<2>();
        }

        void ADC3_IRQHandler(void)  // NOLINT(readability-identifier-naming)
        {
            adc_irq_handler<3>();
        }

        void ADC4_IRQHandler(void)  // NOLINT(readability-identifier-naming)
        {
            adc_irq_handler<4>();
        }

        void ADC5_IRQHandler(void)  // NOLINT(readability-identifier-naming)
        {
            adc_irq_handler<5>();
        }
    }
}  // namespace valle::platform