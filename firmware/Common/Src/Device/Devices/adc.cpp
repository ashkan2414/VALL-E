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
    using ADCPeripheralDeviceT = ADCPeripheralDevice<tkADCIdx>;

    // --------------------------------------------------------------------
    // SAFETY FIRST: Analog Watchdog (AWD) (overvoltage/undervoltage)
    // --------------------------------------------------------------------
    if (ADCPeripheralDeviceT::awd1_int_pending())
    {
        ADCPeripheralDeviceT::ack_awd1_int();
        ADCIsrRouter<tkADCIdx, ADCInterruptType::kAnalogWatchdog>::handle();
    }

    // --------------------------------------------------------------------
    // CRITICAL: End of Injected Sequence (JEOS)
    // --------------------------------------------------------------------
    if (ADCPeripheralDeviceT::inject_eos_int_pending())
    {
        ADCPeripheralDeviceT::ack_inject_eos_int();
        ADCIsrRouter<tkADCIdx, ADCInterruptType::kEndOfInjectSequence>::handle();
    }

    // --------------------------------------------------------------------
    // BACKGROUND: End of Regular Sequence (EOS)
    // --------------------------------------------------------------------
    // Usually handled by DMA TC interrupt, but if using IT:
    if (ADCPeripheralDeviceT::regular_eos_int_pending())
    {
        ADCPeripheralDeviceT::ack_regular_eos_int();
        ADCIsrRouter<tkADCIdx, ADCInterruptType::kEndOfRegularSequence>::handle();
    }

    // --------------------------------------------------------------------
    // ERROR: Overrun (OVR)
    // --------------------------------------------------------------------
    if (ADCPeripheralDeviceT::ovr_int_pending())
    {
        ADCPeripheralDeviceT::ack_ovr_int();
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