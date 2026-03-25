#pragma once

#include "valle/platform/hardware/ADC/interface.hpp"

namespace valle::platform
{
    // ============================================================================
    // INTERRUPT TRAITS
    // ============================================================================
    enum class AdcInterruptType : uint8_t
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

    template <AdcPeripheralId tkPeripheralId, AdcInterruptType tkIntType>
    struct AdcInterruptTraits;

#define DEFINE_ADC_INT_TRAIT(tkIntType, ll_name, should_clear)                                     \
    template <AdcPeripheralId tkPeripheralId>                                                      \
    struct AdcInterruptTraits<tkPeripheralId, (tkIntType)>                                         \
    {                                                                                              \
        static constexpr bool skShouldClear = (should_clear);                                      \
                                                                                                   \
        static inline void enable()                                                                \
        {                                                                                          \
            LL_ADC_EnableIT_##ll_name(AdcControllerTraits<tkPeripheralId>::skInstance);            \
        }                                                                                          \
        static inline void disable()                                                               \
        {                                                                                          \
            LL_ADC_DisableIT_##ll_name(AdcControllerTraits<tkPeripheralId>::skInstance);           \
        }                                                                                          \
        static inline bool is_enabled()                                                            \
        {                                                                                          \
            return LL_ADC_IsEnabledIT_##ll_name(AdcControllerTraits<tkPeripheralId>::skInstance);  \
        }                                                                                          \
        static inline bool flag_active()                                                           \
        {                                                                                          \
            return LL_ADC_IsActiveFlag_##ll_name(AdcControllerTraits<tkPeripheralId>::skInstance); \
        }                                                                                          \
                                                                                                   \
        static inline bool is_pending()                                                            \
        {                                                                                          \
            return flag_active() && is_enabled();                                                  \
        }                                                                                          \
        static inline void ack()                                                                   \
        {                                                                                          \
            LL_ADC_ClearFlag_##ll_name(AdcControllerTraits<tkPeripheralId>::skInstance);           \
        }                                                                                          \
    };

    DEFINE_ADC_INT_TRAIT(AdcInterruptType::kReady, ADRDY, true);
    DEFINE_ADC_INT_TRAIT(AdcInterruptType::kRegularEndOfConversion, EOC, true);
    DEFINE_ADC_INT_TRAIT(AdcInterruptType::kRegularEndOfSequence, EOS, true);
    DEFINE_ADC_INT_TRAIT(AdcInterruptType::kRegularEndOfSampling, EOSMP, true);
    DEFINE_ADC_INT_TRAIT(AdcInterruptType::kInjectEndOfConversion, JEOC, true);
    DEFINE_ADC_INT_TRAIT(AdcInterruptType::kInjectEndOfSequence, JEOS, true);
    DEFINE_ADC_INT_TRAIT(AdcInterruptType::kInjectContextQueueOverflow, JQOVF, true);
    DEFINE_ADC_INT_TRAIT(AdcInterruptType::kOverrun, OVR, true);
    DEFINE_ADC_INT_TRAIT(AdcInterruptType::kAnalogWatchdog1, AWD1, true);
    DEFINE_ADC_INT_TRAIT(AdcInterruptType::kAnalogWatchdog2, AWD2, true);
    DEFINE_ADC_INT_TRAIT(AdcInterruptType::kAnalogWatchdog3, AWD3, true);
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

    template <AdcPeripheralId tkPeripheralId>
    class AdcInterruptController
    {
    public:
        static constexpr AdcPeripheralId skPeripheralId = tkPeripheralId;
        using ControllerTraitsT                         = AdcControllerTraits<tkPeripheralId>;

        static void enable_interrupts(const AdcInterruptConfig& config)
        {
            if (config.interrupts.mask == 0)
            {
                return;  // No interrupts to enable
            }

            if (config.interrupts.ready)
            {
                AdcInterruptTraits<tkPeripheralId, AdcInterruptType::kReady>::ack();
                AdcInterruptTraits<tkPeripheralId, AdcInterruptType::kReady>::enable();
            }

            if (config.interrupts.reg_eoc)
            {
                AdcInterruptTraits<tkPeripheralId, AdcInterruptType::kRegularEndOfConversion>::ack();
                AdcInterruptTraits<tkPeripheralId, AdcInterruptType::kRegularEndOfConversion>::enable();
            }

            if (config.interrupts.reg_eos)
            {
                AdcInterruptTraits<tkPeripheralId, AdcInterruptType::kRegularEndOfSequence>::ack();
                AdcInterruptTraits<tkPeripheralId, AdcInterruptType::kRegularEndOfSequence>::enable();
            }

            if (config.interrupts.reg_eosmp)
            {
                AdcInterruptTraits<tkPeripheralId, AdcInterruptType::kRegularEndOfSampling>::ack();
                AdcInterruptTraits<tkPeripheralId, AdcInterruptType::kRegularEndOfSampling>::enable();
            }

            if (config.interrupts.inj_eoc)
            {
                AdcInterruptTraits<tkPeripheralId, AdcInterruptType::kInjectEndOfConversion>::ack();
                AdcInterruptTraits<tkPeripheralId, AdcInterruptType::kInjectEndOfConversion>::enable();
            }

            if (config.interrupts.inj_eos)
            {
                AdcInterruptTraits<tkPeripheralId, AdcInterruptType::kInjectEndOfSequence>::ack();
                AdcInterruptTraits<tkPeripheralId, AdcInterruptType::kInjectEndOfSequence>::enable();
            }

            if (config.interrupts.inj_cqovf)
            {
                AdcInterruptTraits<tkPeripheralId, AdcInterruptType::kInjectContextQueueOverflow>::ack();
                AdcInterruptTraits<tkPeripheralId, AdcInterruptType::kInjectContextQueueOverflow>::enable();
            }

            if (config.interrupts.ovr)
            {
                AdcInterruptTraits<tkPeripheralId, AdcInterruptType::kOverrun>::ack();
                AdcInterruptTraits<tkPeripheralId, AdcInterruptType::kOverrun>::enable();
            }

            if (config.interrupts.awd1)
            {
                AdcInterruptTraits<tkPeripheralId, AdcInterruptType::kAnalogWatchdog1>::ack();
                AdcInterruptTraits<tkPeripheralId, AdcInterruptType::kAnalogWatchdog1>::enable();
            }

            if (config.interrupts.awd2)
            {
                AdcInterruptTraits<tkPeripheralId, AdcInterruptType::kAnalogWatchdog2>::ack();
                AdcInterruptTraits<tkPeripheralId, AdcInterruptType::kAnalogWatchdog2>::enable();
            }

            if (config.interrupts.awd3)
            {
                AdcInterruptTraits<tkPeripheralId, AdcInterruptType::kAnalogWatchdog3>::ack();
                AdcInterruptTraits<tkPeripheralId, AdcInterruptType::kAnalogWatchdog3>::enable();
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
     * @tparam tkPeripheralId ADC Peripheral ID (1-5)
     */
    template <AdcPeripheralId tkPeripheralId>
    struct AdcGlobalIsrRouter
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
     * @tparam tkPeripheralId The ADC peripheral index the interrupt belongs to.
     * @tparam tkIntType The interrupt type triggered.
     */
    template <AdcPeripheralId tkPeripheralId, AdcInterruptType tkIntType>
    struct AdcIsrRouter
    {
        using UnboundIsrHandlerTag = void;
        static void handle()
        {
            // Default: Do nothing (Optimized away)
        }
    };

}  // namespace valle::platform