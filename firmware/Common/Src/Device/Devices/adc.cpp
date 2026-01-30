#include "Valle/Device/Devices/adc.hpp"

#if __has_include("app_isr_bindings.hpp")
#include "app_isr_bindings.hpp"
#endif

namespace valle
{

    /**
     * @brief ADC Interrupt Handler Router
     *
     * @tparam tkControllerID ADC Index (1-5)
     */
    template <ADCControllerID tkControllerID>
        requires(kValidADCControllerID<tkControllerID>)
    static inline void adc_irq_handler()
    {
#define HANDLE_ADC_INT(tkIntType)                                              \
    if constexpr (CBoundIsrHandler<ADCIsrRouter<tkControllerID, (tkIntType)>>) \
    {                                                                          \
        if (ADCInterruptTraits<tkControllerID, tkIntType>::is_pending())       \
        {                                                                      \
            ADCInterruptTraits<tkControllerID, tkIntType>::ack();              \
            ADCIsrRouter<tkControllerID, tkIntType>::handle();                 \
        }                                                                      \
    }

        HANDLE_ADC_INT(ADCInterruptType::kAnalogWatchdog1);
        HANDLE_ADC_INT(ADCInterruptType::kAnalogWatchdog2);
        HANDLE_ADC_INT(ADCInterruptType::kAnalogWatchdog3);
        HANDLE_ADC_INT(ADCInterruptType::kEndOfRegularSequence);
        HANDLE_ADC_INT(ADCInterruptType::kEndOfInjectSequence);
        HANDLE_ADC_INT(ADCInterruptType::kOverrun);
#undef HANDLE_ADC_INT
    }

    extern "C"
    {
        void ADC1_2_IRQHandler(void)  // NOLINT(readability-identifier-naming)
        {
            // CRITICAL NOTE: ADC1 and ADC2 share this IRQ vector (RM0440 Section 21.4.20).
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
}  // namespace valle