#pragma once

#include <cstdint>

#include "stm32g4xx_ll_exti.h"
#include "valle/platform/hardware/exti/id.hpp"

namespace valle::platform
{
    // ============================================================================
    // ENUMERATIONS
    // =============================================================================
    struct ExtiLineSource : public LLDriverEnumValue<ExtiLineSource>
    {
        static const ExtiLineSource kPortA;
        static const ExtiLineSource kPortB;
        static const ExtiLineSource kPortC;
        static const ExtiLineSource kPortD;
        static const ExtiLineSource kPortE;
        static const ExtiLineSource kPortF;
        static const ExtiLineSource kPortG;
    };

    inline constexpr ExtiLineSource ExtiLineSource::kPortA = ExtiLineSource::from_ll<LL_SYSCFG_EXTI_PORTA>();
    inline constexpr ExtiLineSource ExtiLineSource::kPortB = ExtiLineSource::from_ll<LL_SYSCFG_EXTI_PORTB>();
    inline constexpr ExtiLineSource ExtiLineSource::kPortC = ExtiLineSource::from_ll<LL_SYSCFG_EXTI_PORTC>();
    inline constexpr ExtiLineSource ExtiLineSource::kPortD = ExtiLineSource::from_ll<LL_SYSCFG_EXTI_PORTD>();
    inline constexpr ExtiLineSource ExtiLineSource::kPortE = ExtiLineSource::from_ll<LL_SYSCFG_EXTI_PORTE>();
    inline constexpr ExtiLineSource ExtiLineSource::kPortF = ExtiLineSource::from_ll<LL_SYSCFG_EXTI_PORTF>();
    inline constexpr ExtiLineSource ExtiLineSource::kPortG = ExtiLineSource::from_ll<LL_SYSCFG_EXTI_PORTG>();


    // ============================================================================
    // HARDWARE TRAITS
    // ============================================================================
    template <ExtiLineId tkLineId>
    struct ExtiLineTraits
    {
        static constexpr uint32_t skLineLLIdMap[] = {
            LL_EXTI_LINE_0,  LL_EXTI_LINE_1,  LL_EXTI_LINE_2,  LL_EXTI_LINE_3,  LL_EXTI_LINE_4,  LL_EXTI_LINE_5,
            LL_EXTI_LINE_6,  LL_EXTI_LINE_7,  LL_EXTI_LINE_8,  LL_EXTI_LINE_9,  LL_EXTI_LINE_10, LL_EXTI_LINE_11,
            LL_EXTI_LINE_12, LL_EXTI_LINE_13, LL_EXTI_LINE_14, LL_EXTI_LINE_15,
#ifdef VALLE_PLATFORM_EXTI_HAS_LINE_16
            LL_EXTI_LINE_16,
#endif /* EXTI_IMR1_IM16 */
            LL_EXTI_LINE_17,
#ifdef VALLE_PLATFORM_EXTI_HAS_LINE_18
            LL_EXTI_LINE_18,
#endif /* EXTI_IMR1_IM18 */
            LL_EXTI_LINE_19,
#ifdef VALLE_PLATFORM_EXTI_HAS_LINE_20
            LL_EXTI_LINE_20,
#endif /* EXTI_IMR1_IM20 */
#ifdef VALLE_PLATFORM_EXTI_HAS_LINE_21
            LL_EXTI_LINE_21,
#endif /* EXTI_IMR1_IM21 */
#ifdef VALLE_PLATFORM_EXTI_HAS_LINE_22
            LL_EXTI_LINE_22,
#endif /* EXTI_IMR1_IM22 */
            LL_EXTI_LINE_23,
#ifdef VALLE_PLATFORM_EXTI_HAS_LINE_24
            LL_EXTI_LINE_24,
#endif /* EXTI_IMR1_IM24 */
#ifdef VALLE_PLATFORM_EXTI_HAS_LINE_25
            LL_EXTI_LINE_25,
#endif /* EXTI_IMR1_IM25 */
#ifdef VALLE_PLATFORM_EXTI_HAS_LINE_26
            LL_EXTI_LINE_26,
#endif /* EXTI_IMR1_IM26 */
#ifdef VALLE_PLATFORM_EXTI_HAS_LINE_27
            LL_EXTI_LINE_27,
#endif /* EXTI_IMR1_IM27 */
#ifdef VALLE_PLATFORM_EXTI_HAS_LINE_28
            LL_EXTI_LINE_28,
#endif /* EXTI_IMR1_IM28 */
#ifdef VALLE_PLATFORM_EXTI_HAS_LINE_29
            LL_EXTI_LINE_29,
#endif /* EXTI_IMR1_IM29 */
#ifdef VALLE_PLATFORM_EXTI_HAS_LINE_30
            LL_EXTI_LINE_30,
#endif /* EXTI_IMR1_IM30 */
#ifdef VALLE_PLATFORM_EXTI_HAS_LINE_31
            LL_EXTI_LINE_31,
#endif /* EXTI_IMR1_IM31 */

#ifdef VALLE_PLATFORM_EXTI_HAS_LINE_32
            LL_EXTI_LINE_32,
#endif /* EXTI_IMR2_IM32 */
#ifdef VALLE_PLATFORM_EXTI_HAS_LINE_33
            LL_EXTI_LINE_33,
#endif /* EXTI_IMR2_IM33 */
#ifdef VALLE_PLATFORM_EXTI_HAS_LINE_34
            LL_EXTI_LINE_34,
#endif /* EXTI_IMR2_IM34 */
#ifdef VALLE_PLATFORM_EXTI_HAS_LINE_35
            LL_EXTI_LINE_35,
#endif /* EXTI_IMR2_IM35 */
#ifdef VALLE_PLATFORM_EXTI_HAS_LINE_36
            LL_EXTI_LINE_36,
#endif /* EXTI_IMR2_IM36 */
#ifdef VALLE_PLATFORM_EXTI_HAS_LINE_37
            LL_EXTI_LINE_37,
#endif /* EXTI_IMR2_IM37 */
#ifdef VALLE_PLATFORM_EXTI_HAS_LINE_38
            LL_EXTI_LINE_38,
#endif /* EXTI_IMR2_IM38 */
#ifdef VALLE_PLATFORM_EXTI_HAS_LINE_39
            LL_EXTI_LINE_39,
#endif /* EXTI_IMR2_IM39 */
#ifdef VALLE_PLATFORM_EXTI_HAS_LINE_40
            LL_EXTI_LINE_40,
#endif /* EXTI_IMR2_IM40 */
#ifdef VALLE_PLATFORM_EXTI_HAS_LINE_41
            LL_EXTI_LINE_41,
#endif /* EXTI_IMR2_IM41 */
#ifdef VALLE_PLATFORM_EXTI_HAS_LINE_42
            LL_EXTI_LINE_42,
#endif /* EXTI_IMR2_IM42 */
        };

        static constexpr IRQn_Type skIrqTable[] = {EXTI0_IRQn,
                                                   EXTI1_IRQn,
                                                   EXTI2_IRQn,
                                                   EXTI3_IRQn,
                                                   EXTI4_IRQn,
                                                   EXTI9_5_IRQn,     // 5
                                                   EXTI9_5_IRQn,     // 6
                                                   EXTI9_5_IRQn,     // 7
                                                   EXTI9_5_IRQn,     // 8
                                                   EXTI9_5_IRQn,     // 9
                                                   EXTI15_10_IRQn,   // 10
                                                   EXTI15_10_IRQn,   // 11
                                                   EXTI15_10_IRQn,   // 12
                                                   EXTI15_10_IRQn,   // 13
                                                   EXTI15_10_IRQn,   // 14
                                                   EXTI15_10_IRQn};  // 15

        static constexpr uint32_t skLineSyscfgLLIdTable[] = {LL_SYSCFG_EXTI_LINE0,
                                                             LL_SYSCFG_EXTI_LINE1,
                                                             LL_SYSCFG_EXTI_LINE2,
                                                             LL_SYSCFG_EXTI_LINE3,
                                                             LL_SYSCFG_EXTI_LINE4,
                                                             LL_SYSCFG_EXTI_LINE5,
                                                             LL_SYSCFG_EXTI_LINE6,
                                                             LL_SYSCFG_EXTI_LINE7,
                                                             LL_SYSCFG_EXTI_LINE8,
                                                             LL_SYSCFG_EXTI_LINE9,
                                                             LL_SYSCFG_EXTI_LINE10,
                                                             LL_SYSCFG_EXTI_LINE11,
                                                             LL_SYSCFG_EXTI_LINE12,
                                                             LL_SYSCFG_EXTI_LINE13,
                                                             LL_SYSCFG_EXTI_LINE14,
                                                             LL_SYSCFG_EXTI_LINE15};

        static constexpr uint8_t  skLineIdx               = static_cast<uint8_t>(tkLineId);
        static constexpr uint32_t skLineLLId              = skLineLLIdMap[skLineIdx];
        static constexpr bool     skHasConfigurableSource = (skLineIdx <= 15);

        static constexpr std::optional<uint32_t> skLineSyscfgLLId =
            skHasConfigurableSource ? std::optional<uint32_t>(skLineSyscfgLLIdTable[skLineIdx]) : std::nullopt;

        static constexpr std::optional<IRQn_Type> skIrqNum =
            skHasConfigurableSource ? std::optional<IRQn_Type>(skIrqTable[skLineIdx]) : std::nullopt;
    };

    // ============================================================================
    // INTERFACES
    // ============================================================================
    template <ExtiLineId tkLineId>
    struct ExtiLineInterface
    {
        using LineTraitsT = ExtiLineTraits<tkLineId>;

        void enable_interrupt() const
        {
            if constexpr (LineTraitsT::skLineIdx < 32)  // NOLINT(readability-magic-numbers)
            {
                LL_EXTI_EnableIT_0_31(LineTraitsT::skLineLLId);
            }
            else
            {
                LL_EXTI_EnableIT_32_63(LineTraitsT::skLineLLId);
            }
        }

        void disable_interrupt() const
        {
            if constexpr (LineTraitsT::skLineIdx < 32)  // NOLINT(readability-magic-numbers)
            {
                LL_EXTI_DisableIT_0_31(LineTraitsT::skLineLLId);
            }
            else
            {
                LL_EXTI_DisableIT_32_63(LineTraitsT::skLineLLId);
            }
        }

        [[nodiscard]] uint32_t is_interrupt_enabled() const
        {
            if constexpr (LineTraitsT::skLineIdx < 32)  // NOLINT(readability-magic-numbers)
            {
                return LL_EXTI_IsEnabledIT_0_31(LineTraitsT::skLineLLId);
            }
            else
            {
                return LL_EXTI_IsEnabledIT_32_63(LineTraitsT::skLineLLId);
            }
        }

        void enable_event() const
        {
            if constexpr (LineTraitsT::skLineIdx < 32)  // NOLINT(readability-magic-numbers)
            {
                LL_EXTI_EnableEvent_0_31(LineTraitsT::skLineLLId);
            }
            else
            {
                LL_EXTI_EnableEvent_32_63(LineTraitsT::skLineLLId);
            }
        }

        void disable_event() const
        {
            if constexpr (LineTraitsT::skLineIdx < 32)  // NOLINT(readability-magic-numbers)
            {
                LL_EXTI_DisableEvent_0_31(LineTraitsT::skLineLLId);
            }
            else
            {
                LL_EXTI_DisableEvent_32_63(LineTraitsT::skLineLLId);
            }
        }

        [[nodiscard]] uint32_t is_event_enabled() const
        {
            if constexpr (LineTraitsT::skLineIdx < 32)  // NOLINT(readability-magic-numbers)
            {
                return LL_EXTI_IsEnabledEvent_0_31(LineTraitsT::skLineLLId);
            }
            else
            {
                return LL_EXTI_IsEnabledEvent_32_63(LineTraitsT::skLineLLId);
            }
        }

        void enable_rising_trigger() const
        {
            if constexpr (LineTraitsT::skLineIdx < 32)  // NOLINT(readability-magic-numbers)
            {
                LL_EXTI_EnableRisingTrig_0_31(LineTraitsT::skLineLLId);
            }
            else
            {
                LL_EXTI_EnableRisingTrig_32_63(LineTraitsT::skLineLLId);
            }
        }

        void disable_rising_trigger() const
        {
            if constexpr (LineTraitsT::skLineIdx < 32)  // NOLINT(readability-magic-numbers)
            {
                LL_EXTI_DisableRisingTrig_0_31(LineTraitsT::skLineLLId);
            }
            else
            {
                LL_EXTI_DisableRisingTrig_32_63(LineTraitsT::skLineLLId);
            }
        }
        [[nodiscard]] uint32_t is_rising_trigger_enabled() const
        {
            if constexpr (LineTraitsT::skLineIdx < 32)  // NOLINT(readability-magic-numbers)
            {
                return LL_EXTI_IsEnabledRisingTrig_0_31(LineTraitsT::skLineLLId);
            }
            else
            {
                return LL_EXTI_IsEnabledRisingTrig_32_63(LineTraitsT::skLineLLId);
            }
        }

        void enable_falling_trigger() const
        {
            if constexpr (LineTraitsT::skLineIdx < 32)  // NOLINT(readability-magic-numbers)
            {
                LL_EXTI_EnableFallingTrig_0_31(LineTraitsT::skLineLLId);
            }
            else
            {
                LL_EXTI_EnableFallingTrig_32_63(LineTraitsT::skLineLLId);
            }
        }

        void disable_falling_trigger() const
        {
            if constexpr (LineTraitsT::skLineIdx < 32)  // NOLINT(readability-magic-numbers)
            {
                LL_EXTI_DisableFallingTrig_0_31(LineTraitsT::skLineLLId);
            }
            else
            {
                LL_EXTI_DisableFallingTrig_32_63(LineTraitsT::skLineLLId);
            }
        }

        [[nodiscard]] uint32_t is_falling_trigger_enabled() const
        {
            if constexpr (LineTraitsT::skLineIdx < 32)  // NOLINT(readability-magic-numbers)
            {
                return LL_EXTI_IsEnabledFallingTrig_0_31(LineTraitsT::skLineLLId);
            }
            else
            {
                return LL_EXTI_IsEnabledFallingTrig_32_63(LineTraitsT::skLineLLId);
            }
        }

        void generate_software_interrupt() const
        {
            if constexpr (LineTraitsT::skLineIdx < 32)  // NOLINT(readability-magic-numbers)
            {
                LL_EXTI_GenerateSWI_0_31(LineTraitsT::skLineLLId);
            }
            else
            {
                LL_EXTI_GenerateSWI_32_63(LineTraitsT::skLineLLId);
            }
        }

        [[nodiscard]] uint32_t is_flag_active() const
        {
            if constexpr (LineTraitsT::skLineIdx < 32)  // NOLINT(readability-magic-numbers)
            {
                return LL_EXTI_IsActiveFlag_0_31(LineTraitsT::skLineLLId);
            }
            else
            {
                return LL_EXTI_IsActiveFlag_32_63(LineTraitsT::skLineLLId);
            }
        }

        void clear_flag() const
        {
            if constexpr (LineTraitsT::skLineIdx < 32)  // NOLINT(readability-magic-numbers)
            {
                LL_EXTI_ClearFlag_0_31(LineTraitsT::skLineLLId);
            }
            else
            {
                LL_EXTI_ClearFlag_32_63(LineTraitsT::skLineLLId);
            }
        }

        void set_source(const ExtiLineSource& source) const
            requires(LineTraitsT::skHasConfigurableSource)  // Only EXTI lines 0-15 have configurable sources
        {
            LL_SYSCFG_SetEXTISource(static_cast<uint32_t>(source), LineTraitsT::skLineSyscfgLLId.value());
        }

        [[nodiscard]] ExtiLineSource get_source() const
            requires(LineTraitsT::skHasConfigurableSource)  // Only EXTI lines 0-15 have configurable sources
        {
            return static_cast<ExtiLineSource>(LL_SYSCFG_GetEXTISource(LineTraitsT::skLineSyscfgLLId.value()));
        }
    };

}  // namespace valle::platform
