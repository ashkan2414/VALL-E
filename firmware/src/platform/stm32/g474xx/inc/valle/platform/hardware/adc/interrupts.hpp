#pragma once

#include "valle/platform/hardware/adc/interface.hpp"

namespace valle::platform
{
    // ============================================================================
    // INTERRUPT TRAITS
    // ============================================================================

    template <AdcControllerSpec tkControllerSpec, AdcInterruptSource tkIntSource>
    struct AdcControllerInterruptSourceInterface;

#define DEFINE_ADC_INT_TRAIT(tkIntSource, ll_name, should_clear)                                     \
    template <AdcControllerSpec tkControllerSpec>                                                    \
    struct AdcControllerInterruptSourceInterface<tkControllerSpec, (tkIntSource)>                    \
    {                                                                                                \
        static constexpr bool skShouldClear = (should_clear);                                        \
                                                                                                     \
        static inline void enable()                                                                  \
        {                                                                                            \
            LL_ADC_EnableIT_##ll_name(AdcControllerTraits<tkControllerSpec>::skInstance);            \
        }                                                                                            \
        static inline void disable()                                                                 \
        {                                                                                            \
            LL_ADC_DisableIT_##ll_name(AdcControllerTraits<tkControllerSpec>::skInstance);           \
        }                                                                                            \
        static inline bool is_enabled()                                                              \
        {                                                                                            \
            return LL_ADC_IsEnabledIT_##ll_name(AdcControllerTraits<tkControllerSpec>::skInstance);  \
        }                                                                                            \
        static inline bool flag_active()                                                             \
        {                                                                                            \
            return LL_ADC_IsActiveFlag_##ll_name(AdcControllerTraits<tkControllerSpec>::skInstance); \
        }                                                                                            \
        static inline bool is_pending()                                                              \
        {                                                                                            \
            return flag_active() && is_enabled();                                                    \
        }                                                                                            \
        static inline void clear()                                                                   \
        {                                                                                            \
            LL_ADC_ClearFlag_##ll_name(AdcControllerTraits<tkControllerSpec>::skInstance);           \
        }                                                                                            \
    };

    DEFINE_ADC_INT_TRAIT(AdcInterruptSource::kReady, ADRDY, true);
    DEFINE_ADC_INT_TRAIT(AdcInterruptSource::kRegularEndOfConversion, EOC, true);
    DEFINE_ADC_INT_TRAIT(AdcInterruptSource::kRegularEndOfSequence, EOS, true);
    DEFINE_ADC_INT_TRAIT(AdcInterruptSource::kRegularEndOfSampling, EOSMP, true);
    DEFINE_ADC_INT_TRAIT(AdcInterruptSource::kInjectEndOfConversion, JEOC, true);
    DEFINE_ADC_INT_TRAIT(AdcInterruptSource::kInjectEndOfSequence, JEOS, true);
    DEFINE_ADC_INT_TRAIT(AdcInterruptSource::kInjectContextQueueOverflow, JQOVF, true);
    DEFINE_ADC_INT_TRAIT(AdcInterruptSource::kOverrun, OVR, true);
    DEFINE_ADC_INT_TRAIT(AdcInterruptSource::kAnalogWatchdog1, AWD1, true);
    DEFINE_ADC_INT_TRAIT(AdcInterruptSource::kAnalogWatchdog2, AWD2, true);
    DEFINE_ADC_INT_TRAIT(AdcInterruptSource::kAnalogWatchdog3, AWD3, true);
#undef DEFINE_ADC_INT_TRAIT

    // ============================================================================
    // Interrupt Controller
    // ============================================================================
    struct AdcInterruptMask
    {
        union
        {
            uint32_t mask;  // Full mask (for easy initialization)
            struct
            {
                bool ready : 1     = false;  // Ready flag
                bool reg_eoc : 1   = false;  // Regular End Of Conversion
                bool reg_eos : 1   = false;  // Regular End Of Sequence
                bool reg_eosmp : 1 = false;  // Regular End Of Sampling
                bool inj_eoc : 1   = false;  // Inject End Of Conversion
                bool inj_eos : 1   = false;  // Inject End Of Sequence
                bool inj_cqovf : 1 = false;  // Inject Context Queue Overflow
                bool ovr : 1       = false;  // Overrun
                bool awd1 : 1      = false;  // Analog Watchdog 1
                bool awd2 : 1      = false;  // Analog Watchdog 2
                bool awd3 : 1      = false;  // Analog Watchdog 3
            };
        };
    };

    struct AdcInterruptConfig
    {
        uint32_t         priority = 5;  // NVIC Priority (0 = highest) // NOLINT(readability-magic-numbers)
        AdcInterruptMask interrupts;    // Which interrupts to enable
    };

    template <AdcControllerSpec tkControllerSpec>
    class AdcInterruptController
    {
    public:
        static constexpr AdcControllerSpec skControllerSpec = tkControllerSpec;
        using ControllerTraitsT                             = AdcControllerTraits<tkControllerSpec>;

        static void enable_interrupts(const AdcInterruptConfig& config)
        {
            if (config.interrupts.mask == 0)
            {
                return;  // No interrupts to enable
            }

            if (config.interrupts.ready)
            {
                AdcControllerInterruptSourceInterface<tkControllerSpec, AdcInterruptSource::kReady>::clear();
                AdcControllerInterruptSourceInterface<tkControllerSpec, AdcInterruptSource::kReady>::enable();
            }

            if (config.interrupts.reg_eoc)
            {
                AdcControllerInterruptSourceInterface<tkControllerSpec,
                                                      AdcInterruptSource::kRegularEndOfConversion>::clear();
                AdcControllerInterruptSourceInterface<tkControllerSpec,
                                                      AdcInterruptSource::kRegularEndOfConversion>::enable();
            }

            if (config.interrupts.reg_eos)
            {
                AdcControllerInterruptSourceInterface<tkControllerSpec,
                                                      AdcInterruptSource::kRegularEndOfSequence>::clear();
                AdcControllerInterruptSourceInterface<tkControllerSpec,
                                                      AdcInterruptSource::kRegularEndOfSequence>::enable();
            }

            if (config.interrupts.reg_eosmp)
            {
                AdcControllerInterruptSourceInterface<tkControllerSpec,
                                                      AdcInterruptSource::kRegularEndOfSampling>::clear();
                AdcControllerInterruptSourceInterface<tkControllerSpec,
                                                      AdcInterruptSource::kRegularEndOfSampling>::enable();
            }

            if (config.interrupts.inj_eoc)
            {
                AdcControllerInterruptSourceInterface<tkControllerSpec,
                                                      AdcInterruptSource::kInjectEndOfConversion>::clear();
                AdcControllerInterruptSourceInterface<tkControllerSpec,
                                                      AdcInterruptSource::kInjectEndOfConversion>::enable();
            }

            if (config.interrupts.inj_eos)
            {
                AdcControllerInterruptSourceInterface<tkControllerSpec,
                                                      AdcInterruptSource::kInjectEndOfSequence>::clear();
                AdcControllerInterruptSourceInterface<tkControllerSpec,
                                                      AdcInterruptSource::kInjectEndOfSequence>::enable();
            }

            if (config.interrupts.inj_cqovf)
            {
                AdcControllerInterruptSourceInterface<tkControllerSpec,
                                                      AdcInterruptSource::kInjectContextQueueOverflow>::clear();
                AdcControllerInterruptSourceInterface<tkControllerSpec,
                                                      AdcInterruptSource::kInjectContextQueueOverflow>::enable();
            }

            if (config.interrupts.ovr)
            {
                AdcControllerInterruptSourceInterface<tkControllerSpec, AdcInterruptSource::kOverrun>::clear();
                AdcControllerInterruptSourceInterface<tkControllerSpec, AdcInterruptSource::kOverrun>::enable();
            }

            if (config.interrupts.awd1)
            {
                AdcControllerInterruptSourceInterface<tkControllerSpec, AdcInterruptSource::kAnalogWatchdog1>::clear();
                AdcControllerInterruptSourceInterface<tkControllerSpec, AdcInterruptSource::kAnalogWatchdog1>::enable();
            }

            if (config.interrupts.awd2)
            {
                AdcControllerInterruptSourceInterface<tkControllerSpec, AdcInterruptSource::kAnalogWatchdog2>::clear();
                AdcControllerInterruptSourceInterface<tkControllerSpec, AdcInterruptSource::kAnalogWatchdog2>::enable();
            }

            if (config.interrupts.awd3)
            {
                AdcControllerInterruptSourceInterface<tkControllerSpec, AdcInterruptSource::kAnalogWatchdog3>::clear();
                AdcControllerInterruptSourceInterface<tkControllerSpec, AdcInterruptSource::kAnalogWatchdog3>::enable();
            }

            NVIC_SetPriority(ControllerTraitsT::skIRQn, config.priority);
            NVIC_EnableIRQ(ControllerTraitsT::skIRQn);
        }

        void disable_interrupts()
        {
            NVIC_DisableIRQ(ControllerTraitsT::skIRQn);
        }

        template <AdcInterruptSource tkIntSource>
        static void enable_interrupt()
        {
            AdcControllerInterruptSourceInterface<tkControllerSpec, tkIntSource>::clear();
            AdcControllerInterruptSourceInterface<tkControllerSpec, tkIntSource>::enable();
        }
    };

    // =========================================================================
    // INTERRUPT ROUTERS
    // =========================================================================

    // --------------------------------------------------------------------------
    // GLOBAL ISR ROUTER
    // --------------------------------------------------------------------------

    /**
     * @brief ADC Global ISR Router
     *
     * Specialize this template to handle all ADC interrupts for a given
     * controller in one function (e.g., when using the ST HAL).
     *
     * @tparam tkControllerSpec ADC Controller ID (1-5)
     */
    template <AdcControllerId tkControllerSpec>
    struct AdcIrqRouter
    {
        using UnboundIsrHandlerTag = void;
        static void handle()
        {
            // Default: Do nothing (Optimized away)
        }
    };

    // --------------------------------------------------------------------------
    // GRANULAR ISR ROUTER
    // --------------------------------------------------------------------------

    /**
     * @brief ADC Interrupt Router.
     *
     * @tparam tkControllerSpec The ADC peripheral index the interrupt belongs to.
     * @tparam tkIntSource The interrupt type triggered.
     */
    template <AdcControllerId tkControllerSpec, AdcInterruptSource tkIntSource>
    struct AdcIsrRouter
    {
        using UnboundIsrHandlerTag = void;
        static void handle()
        {
            // Default: Do nothing (Optimized away)
        }
    };

}  // namespace valle::platform