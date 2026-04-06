#include "valle/platform/devices/adc.hpp"

#include "magic_enum/magic_enum.hpp"

#if __has_include("app_isr_bindings.hpp")
#include "app_isr_bindings.hpp"
#endif

namespace valle::platform
{
    /**
     * @brief ADC Interrupt Handler Router
     *
     * @tparam tkControllerSpec    ADC Controller Specification.
     */
    template <AdcControllerSpec tkControllerSpec>
    static void adc_controller_irq_handler()
    {
        AdcControllerIrqRouterContext::dispatch<tkControllerSpec>();
    }

    extern "C"
    {
        void ADC1_2_IRQHandler(void)  // NOLINT(readability-identifier-naming)
        {
            // NOTE: ADC1 and ADC2 share this IRQ vector (RM0440 Section 21.4.20).
            // Each ADC has separate flag registers (ADC1->ISR, ADC2->ISR), so no race condition exists.
            // DO NOT add shared volatile state between ADC1 and ADC2 handlers without proper synchronization!

            // We must check BOTH devices because they share the line.
            // The adc_controller_irq_handler() function does a quick register check,
            // so it's cheap to call even if the ADC isn't active.
            adc_controller_irq_handler<Adc1ControllerSpec>();
            adc_controller_irq_handler<Adc2ControllerSpec>();
        }

        void ADC3_IRQHandler(void)  // NOLINT(readability-identifier-naming)
        {
            adc_controller_irq_handler<Adc3ControllerSpec>();
        }

        void ADC4_IRQHandler(void)  // NOLINT(readability-identifier-naming)
        {
            adc_controller_irq_handler<Adc4ControllerSpec>();
        }

        void ADC5_IRQHandler(void)  // NOLINT(readability-identifier-naming)
        {
            adc_controller_irq_handler<Adc5ControllerSpec>();
        }
    }
}  // namespace valle::platform