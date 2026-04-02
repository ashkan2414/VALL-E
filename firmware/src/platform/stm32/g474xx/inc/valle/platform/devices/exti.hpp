#pragma once

#include "valle/platform/hdi/exti.hpp"

namespace valle::platform
{
    // ============================================================================
    // EXTI LINE DEVICE (UNIQUE)
    // ============================================================================

    // -----------------------------------------------------------------------------
    // CONFIGURATION
    // -----------------------------------------------------------------------------
    enum class ExtiLineTrigger
    {
        kNone = 0,
        kRising,
        kFalling,
        kRisingFalling,
    };

    enum class ExtiLineTriggerAction
    {
        kNone = 0,
        kInterrupt,
        kEvent,
        kInterruptAndEvent,
    };

    struct ExtiLineConfig
    {
        ExtiLineSource source =
            ExtiLineSource::kPortA;  // Only used for EXTI lines 0-15. Ignored for lines 16 and above.
        ExtiLineTrigger       trigger        = ExtiLineTrigger::kRising;
        ExtiLineTriggerAction trigger_action = ExtiLineTriggerAction::kInterrupt;
    };

    // -----------------------------------------------------------------------------
    // DEVICE
    // -----------------------------------------------------------------------------
    template <ExtiLineId tkLineId>
    class ExtiLine
    {
    public:
        struct Descriptor : public UniqueDeviceDescriptor
        {
        };

        static constexpr ExtiLineId skLineId = tkLineId;

        using LineTraitsT              = ExtiLineTraits<tkLineId>;
        using LineInterruptControllerT = ExtiLineInterruptController<tkLineId>;
        using LineHdiT                 = ExtiLineHdi<tkLineId>;

        using InjectDevices = TypeList<LineHdiT>;

    private:
        [[no_unique_address]] DeviceRef<LineHdiT> m_line_hw;

    public:
        explicit ExtiLine(DeviceRef<LineHdiT>&& hardware_key) : m_line_hw(std::move(hardware_key))
        {
        }

        [[nodiscard]] bool init(const ExtiLineConfig& config)
        {
            if constexpr (LineTraitsT::skHasConfigurableSource)
            {
                m_line_hw->set_source(config.source);
            }

            if (config.trigger_action == ExtiLineTriggerAction::kInterrupt ||
                config.trigger_action == ExtiLineTriggerAction::kInterruptAndEvent)
            {
                m_line_hw->enable_interrupt();
            }

            if (config.trigger_action == ExtiLineTriggerAction::kEvent ||
                config.trigger_action == ExtiLineTriggerAction::kInterruptAndEvent)
            {
                m_line_hw->enable_event();
            }

            if (config.trigger == ExtiLineTrigger::kRising || config.trigger == ExtiLineTrigger::kRisingFalling)
            {
                m_line_hw->enable_rising_trigger();
            }

            if (config.trigger == ExtiLineTrigger::kFalling || config.trigger == ExtiLineTrigger::kRisingFalling)
            {
                m_line_hw->enable_falling_trigger();
            }

            return true;
        }

        void enable_interrupts(const ExtiLineInterruptControllerConfig& config)
        {
            LineInterruptControllerT::enable_interrupts(config);
        }

        void disable_interrupts()
        {
            LineInterruptControllerT::disable_interrupts();
        }
    };

    // -----------------------------------------------------------------------------
    // DEVICE ALIASES
    // -----------------------------------------------------------------------------
    using ExtiLine0  = ExtiLine<ExtiLineId::kLine0>;
    using ExtiLine1  = ExtiLine<ExtiLineId::kLine1>;
    using ExtiLine2  = ExtiLine<ExtiLineId::kLine2>;
    using ExtiLine3  = ExtiLine<ExtiLineId::kLine3>;
    using ExtiLine4  = ExtiLine<ExtiLineId::kLine4>;
    using ExtiLine5  = ExtiLine<ExtiLineId::kLine5>;
    using ExtiLine6  = ExtiLine<ExtiLineId::kLine6>;
    using ExtiLine7  = ExtiLine<ExtiLineId::kLine7>;
    using ExtiLine8  = ExtiLine<ExtiLineId::kLine8>;
    using ExtiLine9  = ExtiLine<ExtiLineId::kLine9>;
    using ExtiLine10 = ExtiLine<ExtiLineId::kLine10>;
    using ExtiLine11 = ExtiLine<ExtiLineId::kLine11>;
    using ExtiLine12 = ExtiLine<ExtiLineId::kLine12>;
    using ExtiLine13 = ExtiLine<ExtiLineId::kLine13>;
    using ExtiLine14 = ExtiLine<ExtiLineId::kLine14>;
    using ExtiLine15 = ExtiLine<ExtiLineId::kLine15>;
#ifdef VALLE_PLATFORM_EXTI_HAS_LINE_16
    using ExtiLine16 = ExtiLine<ExtiLineId::kLine16>;
#endif /* EXTI_IMR1_IM16 */
    using ExtiLine17 = ExtiLine<ExtiLineId::kLine17>;
#ifdef VALLE_PLATFORM_EXTI_HAS_LINE_18
    using ExtiLine18 = ExtiLine<ExtiLineId::kLine18>;
#endif /* EXTI_IMR1_IM18 */
    using ExtiLine19 = ExtiLine<ExtiLineId::kLine19>;
#ifdef VALLE_PLATFORM_EXTI_HAS_LINE_20
    using ExtiLine20 = ExtiLine<ExtiLineId::kLine20>;
#endif /* EXTI_IMR1_IM20 */
#ifdef VALLE_PLATFORM_EXTI_HAS_LINE_21
    using ExtiLine21 = ExtiLine<ExtiLineId::kLine21>;
#endif /* EXTI_IMR1_IM21 */
#ifdef VALLE_PLATFORM_EXTI_HAS_LINE_22
    using ExtiLine22 = ExtiLine<ExtiLineId::kLine22>;
#endif /* EXTI_IMR1_IM22 */
    using ExtiLine23 = ExtiLine<ExtiLineId::kLine23>;
#ifdef VALLE_PLATFORM_EXTI_HAS_LINE_24
    using ExtiLine24 = ExtiLine<ExtiLineId::kLine24>;
#endif /* EXTI_IMR1_IM24 */
#ifdef VALLE_PLATFORM_EXTI_HAS_LINE_25
    using ExtiLine25 = ExtiLine<ExtiLineId::kLine25>;
#endif /* EXTI_IMR1_IM25 */
#ifdef VALLE_PLATFORM_EXTI_HAS_LINE_26
    using ExtiLine26 = ExtiLine<ExtiLineId::kLine26>;
#endif /* EXTI_IMR1_IM26 */
#ifdef VALLE_PLATFORM_EXTI_HAS_LINE_27
    using ExtiLine27 = ExtiLine<ExtiLineId::kLine27>;
#endif /* EXTI_IMR1_IM27 */
#ifdef VALLE_PLATFORM_EXTI_HAS_LINE_28
    using ExtiLine28 = ExtiLine<ExtiLineId::kLine28>;
#endif /* EXTI_IMR1_IM28 */
#ifdef VALLE_PLATFORM_EXTI_HAS_LINE_29
    using ExtiLine29 = ExtiLine<ExtiLineId::kLine29>;
#endif /* EXTI_IMR1_IM29 */
#ifdef VALLE_PLATFORM_EXTI_HAS_LINE_30
    using ExtiLine30 = ExtiLine<ExtiLineId::kLine30>;
#endif /* EXTI_IMR1_IM30 */
#ifdef VALLE_PLATFORM_EXTI_HAS_LINE_31
    using ExtiLine31 = ExtiLine<ExtiLineId::kLine31>;
#endif /* EXTI_IMR1_IM31 */

#ifdef VALLE_PLATFORM_EXTI_HAS_LINE_32
    using ExtiLine32 = ExtiLine<ExtiLineId::kLine32>;
#endif /* EXTI_IMR2_IM32 */
#ifdef VALLE_PLATFORM_EXTI_HAS_LINE_33
    using ExtiLine33 = ExtiLine<ExtiLineId::kLine33>;
#endif /* EXTI_IMR2_IM33 */
#ifdef VALLE_PLATFORM_EXTI_HAS_LINE_34
    using ExtiLine34 = ExtiLine<ExtiLineId::kLine34>;
#endif /* EXTI_IMR2_IM34 */
#ifdef VALLE_PLATFORM_EXTI_HAS_LINE_35
    using ExtiLine35 = ExtiLine<ExtiLineId::kLine35>;
#endif /* EXTI_IMR2_IM35 */
#ifdef VALLE_PLATFORM_EXTI_HAS_LINE_36
    using ExtiLine36 = ExtiLine<ExtiLineId::kLine36>;
#endif /* EXTI_IMR2_IM36 */
#ifdef VALLE_PLATFORM_EXTI_HAS_LINE_37
    using ExtiLine37 = ExtiLine<ExtiLineId::kLine37>;
#endif /* EXTI_IMR2_IM37 */
#ifdef VALLE_PLATFORM_EXTI_HAS_LINE_38
    using ExtiLine38 = ExtiLine<ExtiLineId::kLine38>;
#endif /* EXTI_IMR2_IM38 */
#ifdef VALLE_PLATFORM_EXTI_HAS_LINE_39
    using ExtiLine39 = ExtiLine<ExtiLineId::kLine39>;
#endif /* EXTI_IMR2_IM39 */
#ifdef VALLE_PLATFORM_EXTI_HAS_LINE_40
    using ExtiLine40 = ExtiLine<ExtiLineId::kLine40>;
#endif /* EXTI_IMR2_IM40 */
#ifdef VALLE_PLATFORM_EXTI_HAS_LINE_41
    using ExtiLine41 = ExtiLine<ExtiLineId::kLine41>;
#endif /* EXTI_IMR2_IM41 */
#ifdef VALLE_PLATFORM_EXTI_HAS_LINE_42
    using ExtiLine42 = ExtiLine<ExtiLineId::kLine42>;
#endif /* EXTI_IMR2_IM42 */

}  // namespace valle::platform