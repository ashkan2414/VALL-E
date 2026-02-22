#pragma once

#include "Valle/Board/Traits/ADC/hardware.hpp"

namespace valle
{
    // ============================================================================
    // INTERRUPT TRAITS
    // ============================================================================
    enum class ADCInterruptType : uint8_t
    {
        kReady = 0,
        kRegularEndOfConversion,
        kRegularEndOfSequence,
        kRegularEndOfSampling,
        kInjectEndOfConversion,
        kInjectEndOfSequence,
        kInjectContextQueueOverflow,
        kOverrun,
        kAnalogWatchdog1,
        kAnalogWatchdog2,
        kAnalogWatchdog3,
    };

    template <ADCControllerID tkControllerID, ADCInterruptType tkIntType>
    struct ADCInterruptTraits;

#define DEFINE_ADC_INT_TRAIT(tkIntType, ll_name, should_clear)                                     \
    template <ADCControllerID tkControllerID>                                                      \
    struct ADCInterruptTraits<tkControllerID, (tkIntType)>                                         \
    {                                                                                              \
        static constexpr bool skShouldClear = (should_clear);                                      \
                                                                                                   \
        static inline void enable()                                                                \
        {                                                                                          \
            LL_ADC_EnableIT_##ll_name(ADCControllerTraits<tkControllerID>::skInstance);            \
        }                                                                                          \
        static inline void disable()                                                               \
        {                                                                                          \
            LL_ADC_DisableIT_##ll_name(ADCControllerTraits<tkControllerID>::skInstance);           \
        }                                                                                          \
        static inline bool is_enabled()                                                            \
        {                                                                                          \
            return LL_ADC_IsEnabledIT_##ll_name(ADCControllerTraits<tkControllerID>::skInstance);  \
        }                                                                                          \
        static inline bool flag_active()                                                           \
        {                                                                                          \
            return LL_ADC_IsActiveFlag_##ll_name(ADCControllerTraits<tkControllerID>::skInstance); \
        }                                                                                          \
                                                                                                   \
        static inline bool is_pending()                                                            \
        {                                                                                          \
            return flag_active() && is_enabled();                                                  \
        }                                                                                          \
        static inline void ack()                                                                   \
        {                                                                                          \
            LL_ADC_ClearFlag_##ll_name(ADCControllerTraits<tkControllerID>::skInstance);           \
        }                                                                                          \
    };

    DEFINE_ADC_INT_TRAIT(ADCInterruptType::kReady, ADRDY, true);
    DEFINE_ADC_INT_TRAIT(ADCInterruptType::kRegularEndOfConversion, EOC, true);
    DEFINE_ADC_INT_TRAIT(ADCInterruptType::kRegularEndOfSequence, EOS, true);
    DEFINE_ADC_INT_TRAIT(ADCInterruptType::kRegularEndOfSampling, EOSMP, true);
    DEFINE_ADC_INT_TRAIT(ADCInterruptType::kInjectEndOfConversion, JEOC, true);
    DEFINE_ADC_INT_TRAIT(ADCInterruptType::kInjectEndOfSequence, JEOS, true);
    DEFINE_ADC_INT_TRAIT(ADCInterruptType::kInjectContextQueueOverflow, JQOVF, true);
    DEFINE_ADC_INT_TRAIT(ADCInterruptType::kOverrun, OVR, true);
    DEFINE_ADC_INT_TRAIT(ADCInterruptType::kAnalogWatchdog1, AWD1, true);
    DEFINE_ADC_INT_TRAIT(ADCInterruptType::kAnalogWatchdog2, AWD2, true);
    DEFINE_ADC_INT_TRAIT(ADCInterruptType::kAnalogWatchdog3, AWD3, true);
#undef DEFINE_ADC_INT_TRAIT

    // ============================================================================
    // Interrupt Controller
    // ============================================================================
    struct ADCInterruptMask
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

    struct ADCInterruptConfig
    {
        uint32_t         priority = 5;  // NVIC Priority (0 = highest) // NOLINT(readability-magic-numbers)
        ADCInterruptMask interrupts;    // Which interrupts to enable
    };

    template <ADCControllerID tkControllerID>
    class ADCInterruptController
    {
    public:
        static constexpr ADCControllerID skControllerID = tkControllerID;
        using ControllerTraitsT                         = ADCControllerTraits<tkControllerID>;

        static void enable_interrupts(const ADCInterruptConfig& config)
        {
            if (config.interrupts.mask == 0)
            {
                return;  // No interrupts to enable
            }

            if (config.interrupts.ready)
            {
                ADCInterruptTraits<tkControllerID, ADCInterruptType::kReady>::ack();
                ADCInterruptTraits<tkControllerID, ADCInterruptType::kReady>::enable();
            }

            if (config.interrupts.reg_eoc)
            {
                ADCInterruptTraits<tkControllerID, ADCInterruptType::kRegularEndOfConversion>::ack();
                ADCInterruptTraits<tkControllerID, ADCInterruptType::kRegularEndOfConversion>::enable();
            }

            if (config.interrupts.reg_eos)
            {
                ADCInterruptTraits<tkControllerID, ADCInterruptType::kRegularEndOfSequence>::ack();
                ADCInterruptTraits<tkControllerID, ADCInterruptType::kRegularEndOfSequence>::enable();
            }

            if (config.interrupts.reg_eosmp)
            {
                ADCInterruptTraits<tkControllerID, ADCInterruptType::kRegularEndOfSampling>::ack();
                ADCInterruptTraits<tkControllerID, ADCInterruptType::kRegularEndOfSampling>::enable();
            }

            if (config.interrupts.inj_eoc)
            {
                ADCInterruptTraits<tkControllerID, ADCInterruptType::kInjectEndOfConversion>::ack();
                ADCInterruptTraits<tkControllerID, ADCInterruptType::kInjectEndOfConversion>::enable();
            }

            if (config.interrupts.inj_eos)
            {
                ADCInterruptTraits<tkControllerID, ADCInterruptType::kInjectEndOfSequence>::ack();
                ADCInterruptTraits<tkControllerID, ADCInterruptType::kInjectEndOfSequence>::enable();
            }

            if (config.interrupts.inj_cqovf)
            {
                ADCInterruptTraits<tkControllerID, ADCInterruptType::kInjectContextQueueOverflow>::ack();
                ADCInterruptTraits<tkControllerID, ADCInterruptType::kInjectContextQueueOverflow>::enable();
            }

            if (config.interrupts.ovr)
            {
                ADCInterruptTraits<tkControllerID, ADCInterruptType::kOverrun>::ack();
                ADCInterruptTraits<tkControllerID, ADCInterruptType::kOverrun>::enable();
            }

            if (config.interrupts.awd1)
            {
                ADCInterruptTraits<tkControllerID, ADCInterruptType::kAnalogWatchdog1>::ack();
                ADCInterruptTraits<tkControllerID, ADCInterruptType::kAnalogWatchdog1>::enable();
            }

            if (config.interrupts.awd2)
            {
                ADCInterruptTraits<tkControllerID, ADCInterruptType::kAnalogWatchdog2>::ack();
                ADCInterruptTraits<tkControllerID, ADCInterruptType::kAnalogWatchdog2>::enable();
            }

            if (config.interrupts.awd3)
            {
                ADCInterruptTraits<tkControllerID, ADCInterruptType::kAnalogWatchdog3>::ack();
                ADCInterruptTraits<tkControllerID, ADCInterruptType::kAnalogWatchdog3>::enable();
            }

            NVIC_SetPriority(ControllerTraitsT::skIRQn, config.priority);
            NVIC_EnableIRQ(ControllerTraitsT::skIRQn);
        }

        void disable_interrupts()
        {
            NVIC_DisableIRQ(ControllerTraitsT::skIRQn);
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
     * @tparam tkControllerID ADC Controller ID (1-5)
     */
    template <ADCControllerID tkControllerID>
        requires(kValidADCControllerID<tkControllerID>)
    struct ADCGlobalISRRouter
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
     * @tparam tkControllerID The ADC peripheral index the interrupt belongs to.
     * @tparam tkIntType The interrupt type triggered.
     */
    template <ADCControllerID tkControllerID, ADCInterruptType tkIntType>
        requires(kValidADCControllerID<tkControllerID>)
    struct ADCISRRouter
    {
        using UnboundIsrHandlerTag = void;
        static void handle()
        {
            // Default: Do nothing (Optimized away)
        }
    };

}  // namespace valle