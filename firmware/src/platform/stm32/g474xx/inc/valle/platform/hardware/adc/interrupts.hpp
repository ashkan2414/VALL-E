#pragma once

#include "valle/platform/hardware/adc/interface.hpp"

namespace valle::platform
{
    // ============================================================================
    // INTERRUPT TRAITS
    // ============================================================================

    template <AdcControllerSpec tkControllerSpec, AdcControllerInterruptSource tkIntSource>
    struct AdcControllerInterruptSourceInterface
        : InterruptSourceInterfaceBase<AdcControllerInterface<tkControllerSpec>, tkIntSource>
    {
    };

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
                AdcControllerInterruptSourceInterface<tkControllerSpec, AdcControllerInterruptSource::kReady>::clear();
                AdcControllerInterruptSourceInterface<tkControllerSpec, AdcControllerInterruptSource::kReady>::enable();
            }

            if (config.interrupts.reg_eoc)
            {
                AdcControllerInterruptSourceInterface<tkControllerSpec,
                                                      AdcControllerInterruptSource::kRegularEndOfConversion>::clear();
                AdcControllerInterruptSourceInterface<tkControllerSpec,
                                                      AdcControllerInterruptSource::kRegularEndOfConversion>::enable();
            }

            if (config.interrupts.reg_eos)
            {
                AdcControllerInterruptSourceInterface<tkControllerSpec,
                                                      AdcControllerInterruptSource::kRegularEndOfSequence>::clear();
                AdcControllerInterruptSourceInterface<tkControllerSpec,
                                                      AdcControllerInterruptSource::kRegularEndOfSequence>::enable();
            }

            if (config.interrupts.reg_eosmp)
            {
                AdcControllerInterruptSourceInterface<tkControllerSpec,
                                                      AdcControllerInterruptSource::kRegularEndOfSampling>::clear();
                AdcControllerInterruptSourceInterface<tkControllerSpec,
                                                      AdcControllerInterruptSource::kRegularEndOfSampling>::enable();
            }

            if (config.interrupts.inj_eoc)
            {
                AdcControllerInterruptSourceInterface<tkControllerSpec,
                                                      AdcControllerInterruptSource::kInjectEndOfConversion>::clear();
                AdcControllerInterruptSourceInterface<tkControllerSpec,
                                                      AdcControllerInterruptSource::kInjectEndOfConversion>::enable();
            }

            if (config.interrupts.inj_eos)
            {
                AdcControllerInterruptSourceInterface<tkControllerSpec,
                                                      AdcControllerInterruptSource::kInjectEndOfSequence>::clear();
                AdcControllerInterruptSourceInterface<tkControllerSpec,
                                                      AdcControllerInterruptSource::kInjectEndOfSequence>::enable();
            }

            if (config.interrupts.inj_cqovf)
            {
                AdcControllerInterruptSourceInterface<
                    tkControllerSpec,
                    AdcControllerInterruptSource::kInjectContextQueueOverflow>::clear();
                AdcControllerInterruptSourceInterface<
                    tkControllerSpec,
                    AdcControllerInterruptSource::kInjectContextQueueOverflow>::enable();
            }

            if (config.interrupts.ovr)
            {
                AdcControllerInterruptSourceInterface<tkControllerSpec,
                                                      AdcControllerInterruptSource::kOverrun>::clear();
                AdcControllerInterruptSourceInterface<tkControllerSpec,
                                                      AdcControllerInterruptSource::kOverrun>::enable();
            }

            if (config.interrupts.awd1)
            {
                AdcControllerInterruptSourceInterface<tkControllerSpec,
                                                      AdcControllerInterruptSource::kAnalogWatchdog1>::clear();
                AdcControllerInterruptSourceInterface<tkControllerSpec,
                                                      AdcControllerInterruptSource::kAnalogWatchdog1>::enable();
            }

            if (config.interrupts.awd2)
            {
                AdcControllerInterruptSourceInterface<tkControllerSpec,
                                                      AdcControllerInterruptSource::kAnalogWatchdog2>::clear();
                AdcControllerInterruptSourceInterface<tkControllerSpec,
                                                      AdcControllerInterruptSource::kAnalogWatchdog2>::enable();
            }

            if (config.interrupts.awd3)
            {
                AdcControllerInterruptSourceInterface<tkControllerSpec,
                                                      AdcControllerInterruptSource::kAnalogWatchdog3>::clear();
                AdcControllerInterruptSourceInterface<tkControllerSpec,
                                                      AdcControllerInterruptSource::kAnalogWatchdog3>::enable();
            }

            NVIC_SetPriority(ControllerTraitsT::skIRQn, config.priority);
            NVIC_EnableIRQ(ControllerTraitsT::skIRQn);
        }

        void disable_interrupts()
        {
            NVIC_DisableIRQ(ControllerTraitsT::skIRQn);
        }

        template <AdcControllerInterruptSource tkIntSource>
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
    struct AdcControllerIrqRouter
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
    template <AdcControllerId tkControllerSpec, AdcControllerInterruptSource tkIntSource>
    struct AdcControllerIsrRouter
    {
        using UnboundIsrHandlerTag = void;
        static void handle()
        {
            // Default: Do nothing (Optimized away)
        }
    };

    using AdcControllerIrqRouterContext = IrqRouterContext<AdcControllerSpec,
                                                           AdcControllerInterruptSource,
                                                           AdcControllerInterruptSourceInterface,
                                                           AdcControllerIrqRouter,
                                                           AdcControllerIsrRouter>;

}  // namespace valle::platform