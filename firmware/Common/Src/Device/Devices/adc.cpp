#include "Valle/Device/Devices/adc.hpp"

/**
 * @brief ADC Interrupt Handler Router
 *
 * @tparam tkADCIdx ADC Index (1-5)
 */
template <uint8_t tkADCIdx>
    requires(kValidADCIndex<tkADCIdx>)
static inline void adc_irq_handler()
{
    ADC_TypeDef* adc = ADCTraits<tkADCIdx>::skInstance;

    // --------------------------------------------------------------------
    // SAFETY FIRST: Analog Watchdog (AWD)
    // --------------------------------------------------------------------
    if (LL_ADC_IsActiveFlag_AWD1(adc) && LL_ADC_IsEnabledIT_AWD1(adc))
    {
        // Clear flag immediately to prevent infinite loop
        LL_ADC_ClearFlag_AWD1(adc);

        // Handle Analog Watchdog event (overvoltage/undervoltage)
        ADCIsrRouter<tkADCIdx, ADCInterruptType::kAnalogWatchdog>::handle();
    }

    // --------------------------------------------------------------------
    // CRITICAL: End of Injected Sequence (JEOS)
    // --------------------------------------------------------------------
    if (LL_ADC_IsActiveFlag_JEOS(adc) && LL_ADC_IsEnabledIT_JEOS(adc))
    {
        LL_ADC_ClearFlag_JEOS(adc);
        ADCIsrRouter<tkADCIdx, ADCInterruptType::kEndOfInjectSequence>::handle();
    }

    // --------------------------------------------------------------------
    // BACKGROUND: End of Regular Sequence (EOS)
    // --------------------------------------------------------------------
    // Usually handled by DMA TC interrupt, but if using IT:
    if (LL_ADC_IsActiveFlag_EOS(adc) && LL_ADC_IsEnabledIT_EOS(adc))
    {
        LL_ADC_ClearFlag_EOS(adc);
        ADCIsrRouter<tkADCIdx, ADCInterruptType::kEndOfRegularSequence>::handle();
    }

    // --------------------------------------------------------------------
    // ERROR: Overrun (OVR)
    // --------------------------------------------------------------------
    if (LL_ADC_IsActiveFlag_OVR(adc) && LL_ADC_IsEnabledIT_OVR(adc))
    {
        // Clear it so we can resume
        LL_ADC_ClearFlag_OVR(adc);
        ADCIsrRouter<tkADCIdx, ADCInterruptType::kOverrun>::handle();
    }
}

extern "C"
{
    void ADC1_2_IRQHandler(void)  // NOLINT(readability-identifier-naming)
    {
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