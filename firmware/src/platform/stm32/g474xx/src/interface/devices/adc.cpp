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
     * @tparam tkControllerId ADC Controller ID.
     */
    template <AdcControllerId tkControllerId>
    [[nodiscard]] consteval static inline bool adc_has_any_granular_isr_handler()
    {
        constexpr auto values = magic_enum::enum_values<AdcInterruptSource>();

        return [values]<std::size_t... Is>(std::index_sequence<Is...>)
        {
            return (CBoundIsrRouter<AdcIsrRouter<tkControllerId, values[Is]>> || ...);
        }(std::make_index_sequence<values.size()>{});
    }

    /**
     * @brief ADC Interrupt Handler Router
     *
     * @tparam tkControllerId ADC Index (1-5)
     */
    template <AdcControllerId tkControllerId>
    static inline void adc_irq_handler()
    {
        using IrqRouterT                   = AdcIrqRouter<tkControllerId>;
        constexpr bool has_irq_handler     = CBoundIsrRouter<IrqRouterT>;
        constexpr bool has_source_handlers = adc_has_any_granular_isr_handler<tkControllerId>();
        static_assert(!(has_irq_handler && has_source_handlers),
                      "VALLE CONFLICT: IRQ and Source ISR handlers detected.");

        if constexpr (has_irq_handler)
        {
            IrqRouterT::handle();
            return;
        }

#define HANDLE_ADC_INT(tkIntSource)                                                         \
    {                                                                                       \
        using RouterT = AdcIsrRouter<tkControllerId, (tkIntSource)>;                        \
        using TraitsT = AdcControllerInterruptSourceInterface<tkControllerId, tkIntSource>; \
        if constexpr (CBoundIsrRouter<RouterT>)                                             \
        {                                                                                   \
            if (TraitsT::is_pending())                                                      \
            {                                                                               \
                if constexpr (kIsrRouterConfigAck<RouterT>)                                 \
                {                                                                           \
                    TraitsT::clear();                                                       \
                }                                                                           \
                RouterT::handle();                                                          \
            }                                                                               \
        }                                                                                   \
        else                                                                                \
        {                                                                                   \
            if constexpr (TraitsT::skShouldClear)                                           \
            {                                                                               \
                if (TraitsT::is_pending())                                                  \
                {                                                                           \
                    TraitsT::clear();                                                       \
                }                                                                           \
            }                                                                               \
        }                                                                                   \
    }

        HANDLE_ADC_INT(AdcInterruptSource::kReady);
        HANDLE_ADC_INT(AdcInterruptSource::kRegularEndOfConversion);
        HANDLE_ADC_INT(AdcInterruptSource::kRegularEndOfSequence);
        HANDLE_ADC_INT(AdcInterruptSource::kRegularEndOfSampling);
        HANDLE_ADC_INT(AdcInterruptSource::kInjectEndOfConversion);
        HANDLE_ADC_INT(AdcInterruptSource::kInjectEndOfSequence);
        HANDLE_ADC_INT(AdcInterruptSource::kInjectContextQueueOverflow);
        HANDLE_ADC_INT(AdcInterruptSource::kOverrun);
        HANDLE_ADC_INT(AdcInterruptSource::kAnalogWatchdog1);
        HANDLE_ADC_INT(AdcInterruptSource::kAnalogWatchdog2);
        HANDLE_ADC_INT(AdcInterruptSource::kAnalogWatchdog3);
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
            adc_irq_handler<AdcControllerId::kAdc1>();
            adc_irq_handler<AdcControllerId::kAdc2>();
        }

        void ADC3_IRQHandler(void)  // NOLINT(readability-identifier-naming)
        {
            adc_irq_handler<AdcControllerId::kAdc3>();
        }

        void ADC4_IRQHandler(void)  // NOLINT(readability-identifier-naming)
        {
            adc_irq_handler<AdcControllerId::kAdc4>();
        }

        void ADC5_IRQHandler(void)  // NOLINT(readability-identifier-naming)
        {
            adc_irq_handler<AdcControllerId::kAdc5>();
        }
    }
}  // namespace valle::platform