#pragma once
#include "stm32g4xx_ll_bus.h"
#include "stm32g4xx_ll_tim.h"
#include "valle/platform/hardware/tim/id.hpp"

namespace valle::platform
{
    // =========================================================================
    // ENUMERATIONS
    // =========================================================================

    // -------------------------------------------------------------------------
    // COUNTER ENUMS
    // -------------------------------------------------------------------------

    enum class TimControllerCounterMode : uint32_t
    {
        kUp           = LL_TIM_COUNTERMODE_UP,
        kDown         = LL_TIM_COUNTERMODE_DOWN,
        kCenterDown   = LL_TIM_COUNTERMODE_CENTER_DOWN,
        kCenterUp     = LL_TIM_COUNTERMODE_CENTER_UP,
        kCenterUpDown = LL_TIM_COUNTERMODE_CENTER_UP_DOWN
    };

    enum class TimControllerDirection : uint32_t
    {
        kUp   = LL_TIM_COUNTERDIRECTION_UP,
        kDown = LL_TIM_COUNTERDIRECTION_DOWN
    };

    enum class TimControllerClockDivision : uint32_t
    {
        kDiv1 = LL_TIM_CLOCKDIVISION_DIV1,
        kDiv2 = LL_TIM_CLOCKDIVISION_DIV2,
        kDiv4 = LL_TIM_CLOCKDIVISION_DIV4
    };

    enum class TimControllerUpdateSource : uint32_t
    {
        kRegular = LL_TIM_UPDATESOURCE_REGULAR,
        kCounter = LL_TIM_UPDATESOURCE_COUNTER
    };

    enum class TimControllerDmaBurstBase : uint32_t
    {
        kControlRegister1     = LL_TIM_DMABURST_BASEADDR_CR1,
        kControlRegister2     = LL_TIM_DMABURST_BASEADDR_CR2,
        kSlaveModeControl     = LL_TIM_DMABURST_BASEADDR_SMCR,
        kDmaInterruptEnable   = LL_TIM_DMABURST_BASEADDR_DIER,
        kStatus               = LL_TIM_DMABURST_BASEADDR_SR,
        kEventGeneration      = LL_TIM_DMABURST_BASEADDR_EGR,
        kCaptureCompareMode1  = LL_TIM_DMABURST_BASEADDR_CCMR1,
        kCaptureCompareMode2  = LL_TIM_DMABURST_BASEADDR_CCMR2,
        kCaptureCompareEnable = LL_TIM_DMABURST_BASEADDR_CCER,
        kCounter              = LL_TIM_DMABURST_BASEADDR_CNT,
        kPrescaler            = LL_TIM_DMABURST_BASEADDR_PSC,
        kAutoReload           = LL_TIM_DMABURST_BASEADDR_ARR,
        kRepetitionCounter    = LL_TIM_DMABURST_BASEADDR_RCR,
        kCaptureCompare1      = LL_TIM_DMABURST_BASEADDR_CCR1,
        kCaptureCompare2      = LL_TIM_DMABURST_BASEADDR_CCR2,
        kCaptureCompare3      = LL_TIM_DMABURST_BASEADDR_CCR3,
        kCaptureCompare4      = LL_TIM_DMABURST_BASEADDR_CCR4,
        kBreakDeadTime        = LL_TIM_DMABURST_BASEADDR_BDTR,
        kOptionRegister       = LL_TIM_DMABURST_BASEADDR_OR,
        kCaptureCompareMode3  = LL_TIM_DMABURST_BASEADDR_CCMR3,
        kCaptureCompare5      = LL_TIM_DMABURST_BASEADDR_CCR5,
        kCaptureCompare6      = LL_TIM_DMABURST_BASEADDR_CCR6,
        kAlternateFunction1   = LL_TIM_DMABURST_BASEADDR_AF1,
        kAlternateFunction2   = LL_TIM_DMABURST_BASEADDR_AF2,
        kTimerInputSelection  = LL_TIM_DMABURST_BASEADDR_TISEL
    };

    enum class TimControllerDmaBurstLength : uint32_t
    {
        k1Transfer   = LL_TIM_DMABURST_LENGTH_1TRANSFER,
        k2Transfers  = LL_TIM_DMABURST_LENGTH_2TRANSFERS,
        k3Transfers  = LL_TIM_DMABURST_LENGTH_3TRANSFERS,
        k4Transfers  = LL_TIM_DMABURST_LENGTH_4TRANSFERS,
        k18Transfers = LL_TIM_DMABURST_LENGTH_18TRANSFERS
    };

    enum class TimControllerOnePulseMode : uint32_t
    {
        kRepetitive = LL_TIM_ONEPULSEMODE_REPETITIVE,
        kSingle     = LL_TIM_ONEPULSEMODE_SINGLE
    };

    enum class TimControllerUpdateMode : uint32_t
    {
        kImmediate = 0,
        kPreload   = 1
    };

    // -------------------------------------------------------------------------
    // ENCODER
    // -------------------------------------------------------------------------

    enum class TimControllerEncoderMode : uint32_t
    {
        kX2TimerInput1                  = LL_TIM_ENCODERMODE_X2_TI1,
        kX2TimerInput2                  = LL_TIM_ENCODERMODE_X2_TI2,
        kX4TimerInput12                 = LL_TIM_ENCODERMODE_X4_TI12,
        kClockPlusDirectionX2           = LL_TIM_ENCODERMODE_CLOCKPLUSDIRECTION_X2,
        kClockPlusDirectionX1           = LL_TIM_ENCODERMODE_CLOCKPLUSDIRECTION_X1,
        kDirectionalClockX2             = LL_TIM_ENCODERMODE_DIRECTIONALCLOCK_X2,
        kDirectionalClockX1TimerInput12 = LL_TIM_ENCODERMODE_DIRECTIONALCLOCK_X1_TI12,
        kX1TimerInput1                  = LL_TIM_ENCODERMODE_X1_TI1,
        kX1TimerInput2                  = LL_TIM_ENCODERMODE_X1_TI2
    };

    // -------------------------------------------------------------------------
    // INPUT CAPTURE
    // -------------------------------------------------------------------------

    enum class TimChannelInputCapturePolarity : uint32_t
    {
        kRising    = LL_TIM_IC_POLARITY_RISING,
        kFalling   = LL_TIM_IC_POLARITY_FALLING,
        kBothEdges = LL_TIM_IC_POLARITY_BOTHEDGE
    };

    enum class TimChannelInputCaptureActiveInput : uint32_t
    {
        kDirectTimerInput    = LL_TIM_ACTIVEINPUT_DIRECTTI,
        kIndirectTimerInput  = LL_TIM_ACTIVEINPUT_INDIRECTTI,
        kTimerReferenceClock = LL_TIM_ACTIVEINPUT_TRC
    };

    enum class TimChannelInputCapturePrescaler : uint32_t
    {
        kDiv1 = LL_TIM_ICPSC_DIV1,
        kDiv2 = LL_TIM_ICPSC_DIV2,
        kDiv4 = LL_TIM_ICPSC_DIV4,
        kDiv8 = LL_TIM_ICPSC_DIV8
    };

    enum class TimChannelInputCaptureFilter : uint32_t
    {
        kNone               = LL_TIM_IC_FILTER_FDIV1,
        kFreqDiv1N2Samples  = LL_TIM_IC_FILTER_FDIV1_N2,
        kFreqDiv1N4Samples  = LL_TIM_IC_FILTER_FDIV1_N4,
        kFreqDiv1N8Samples  = LL_TIM_IC_FILTER_FDIV1_N8,
        kFreqDiv2N6Samples  = LL_TIM_IC_FILTER_FDIV2_N6,
        kFreqDiv2N8Samples  = LL_TIM_IC_FILTER_FDIV2_N8,
        kFreqDiv4N6Samples  = LL_TIM_IC_FILTER_FDIV4_N6,
        kFreqDiv4N8Samples  = LL_TIM_IC_FILTER_FDIV4_N8,
        kFreqDiv8N6Samples  = LL_TIM_IC_FILTER_FDIV8_N6,
        kFreqDiv8N8Samples  = LL_TIM_IC_FILTER_FDIV8_N8,
        kFreqDiv16N5Samples = LL_TIM_IC_FILTER_FDIV16_N5,
        kFreqDiv16N6Samples = LL_TIM_IC_FILTER_FDIV16_N6,
        kFreqDiv16N8Samples = LL_TIM_IC_FILTER_FDIV16_N8,
        kFreqDiv32N5Samples = LL_TIM_IC_FILTER_FDIV32_N5,
        kFreqDiv32N6Samples = LL_TIM_IC_FILTER_FDIV32_N6,
        kFreqDiv32N8Samples = LL_TIM_IC_FILTER_FDIV32_N8
    };

    // -------------------------------------------------------------------------
    // OUTPUT COMPARE
    // -------------------------------------------------------------------------

    enum class TimChannelOutputCaptureMode : uint32_t
    {
        kFrozen                     = LL_TIM_OCMODE_FROZEN,
        kActive                     = LL_TIM_OCMODE_ACTIVE,
        kInactive                   = LL_TIM_OCMODE_INACTIVE,
        kToggle                     = LL_TIM_OCMODE_TOGGLE,
        kForcedInactive             = LL_TIM_OCMODE_FORCED_INACTIVE,
        kForcedActive               = LL_TIM_OCMODE_FORCED_ACTIVE,
        kPwm1                       = LL_TIM_OCMODE_PWM1,
        kPwm2                       = LL_TIM_OCMODE_PWM2,
        kRetriggerableOnePulseMode1 = LL_TIM_OCMODE_RETRIG_OPM1,
        kRetriggerableOnePulseMode2 = LL_TIM_OCMODE_RETRIG_OPM2,
        kCombinedPwm1               = LL_TIM_OCMODE_COMBINED_PWM1,
        kCombinedPwm2               = LL_TIM_OCMODE_COMBINED_PWM2,
        kAsymmetricPwm1             = LL_TIM_OCMODE_ASYMMETRIC_PWM1,
        kAsymmetricPwm2             = LL_TIM_OCMODE_ASYMMETRIC_PWM2,
        kDirectionOutput            = LL_TIM_OCMODE_DIRECTION_OUTPUT
    };

    enum class TimChannelOutputCapturePolarity : uint32_t
    {
        kHigh = LL_TIM_OCPOLARITY_HIGH,
        kLow  = LL_TIM_OCPOLARITY_LOW
    };

    enum class TimChannelOutputCaptureIdleState : uint32_t
    {
        kLow  = LL_TIM_OCIdLESTATE_LOW,
        kHigh = LL_TIM_OCIdLESTATE_HIGH
    };

    // -------------------------------------------------------------------------
    // MASTER / SLAVE / TRIGGER ENUMS
    // -------------------------------------------------------------------------

    enum class TimControllerTriggerOutputSource : uint32_t
    {
        kReset               = LL_TIM_TRGO_RESET,
        kEnable              = LL_TIM_TRGO_ENABLE,
        kUpdate              = LL_TIM_TRGO_UPDATE,
        kCompareChannel1Flag = LL_TIM_TRGO_CC1IF,
        kOutputCompare1Ref   = LL_TIM_TRGO_OC1REF,
        kOutputCompare2Ref   = LL_TIM_TRGO_OC2REF,
        kOutputCompare3Ref   = LL_TIM_TRGO_OC3REF,
        kOutputCompare4Ref   = LL_TIM_TRGO_OC4REF,
        kEncoderClockSource  = LL_TIM_TRGO_ENCODERCLK
    };

    enum class TimControllerTriggerOutput2Source : uint32_t
    {
        kReset                                     = LL_TIM_TRGO2_RESET,
        kEnable                                    = LL_TIM_TRGO2_ENABLE,
        kUpdate                                    = LL_TIM_TRGO2_UPDATE,
        kCompareChannel1Flag                       = LL_TIM_TRGO2_CC1F,
        kOutputCompare1Ref                         = LL_TIM_TRGO2_OC1,
        kOutputCompare2Ref                         = LL_TIM_TRGO2_OC2,
        kOutputCompare3Ref                         = LL_TIM_TRGO2_OC3,
        kOutputCompare4Ref                         = LL_TIM_TRGO2_OC4,
        kOutputCompare5Ref                         = LL_TIM_TRGO2_OC5,
        kOutputCompare6Ref                         = LL_TIM_TRGO2_OC6,
        kOutputCompare4RisingFallingEdge           = LL_TIM_TRGO2_OC4_RISINGFALLING,
        kOutputCompare6RisingFallingEdge           = LL_TIM_TRGO2_OC6_RISINGFALLING,
        kOutputCompare4RisingOutputCompare6Rising  = LL_TIM_TRGO2_OC4_RISING_OC6_RISING,
        kOutputCompare4RisingOutputCompare6Falling = LL_TIM_TRGO2_OC4_RISING_OC6_FALLING,
        kOutputCompare5RisingOutputCompare6Rising  = LL_TIM_TRGO2_OC5_RISING_OC6_RISING,
        kOutputCompare5RisingOutputCompare6Falling = LL_TIM_TRGO2_OC5_RISING_OC6_FALLING
    };

    enum class TimControllerSlaveMode : uint32_t
    {
        kDisabled     = LL_TIM_SLAVEMODE_DISABLED,
        kReset        = LL_TIM_SLAVEMODE_RESET,
        kGated        = LL_TIM_SLAVEMODE_GATED,
        kTrigger      = LL_TIM_SLAVEMODE_TRIGGER,
        kResetTrigger = LL_TIM_SLAVEMODE_COMBINED_RESETTRIGGER,
        kGatedReset   = LL_TIM_SLAVEMODE_COMBINED_GATEDRESET
    };

    enum class TimControllerTriggerSource : uint32_t
    {
        kInternalTrigger0                = LL_TIM_TS_ITR0,
        kInternalTrigger1                = LL_TIM_TS_ITR1,
        kInternalTrigger2                = LL_TIM_TS_ITR2,
        kInternalTrigger3                = LL_TIM_TS_ITR3,
        kInternalTrigger4                = LL_TIM_TS_ITR4,
        kInternalTrigger5                = LL_TIM_TS_ITR5,
        kInternalTrigger6                = LL_TIM_TS_ITR6,
        kInternalTrigger7                = LL_TIM_TS_ITR7,
        kInternalTrigger8                = LL_TIM_TS_ITR8,
        kInternalTrigger9                = LL_TIM_TS_ITR9,
        kInternalTrigger10               = LL_TIM_TS_ITR10,
        kInternalTrigger11               = LL_TIM_TS_ITR11,
        kTimerInput1FallingEdgeDetection = LL_TIM_TS_TI1F_ED,
        kTimerInput1FilteredInput1       = LL_TIM_TS_TI1FP1,
        kTimerInput2FilteredInput2       = LL_TIM_TS_TI2FP2,
        kExternalTriggerFiltered         = LL_TIM_TS_ETRF
    };

    // -------------------------------------------------------------------------
    // BREAK ENUMS
    // -------------------------------------------------------------------------

    enum class TimControllerBreakPolarity : uint32_t
    {
        kLow  = LL_TIM_BREAK_POLARITY_LOW,
        kHigh = LL_TIM_BREAK_POLARITY_HIGH
    };

    enum class TimControllerBreakAFMode : uint32_t
    {
        kInput         = LL_TIM_BREAK_AFMODE_INPUT,
        kBidirectional = LL_TIM_BREAK_AFMODE_BIdIRECTIONAL
    };

    enum class TimControllerOffState : uint32_t
    {
        kRun  = LL_TIM_OSSR_ENABLE,
        kIdle = LL_TIM_OSSI_ENABLE
    };

    enum class TimControllerLockLevel : uint32_t
    {
        kOff = LL_TIM_LOCKLEVEL_OFF,
        k1   = LL_TIM_LOCKLEVEL_1,
        k2   = LL_TIM_LOCKLEVEL_2,
        k3   = LL_TIM_LOCKLEVEL_3
    };

    enum class TimControllerBreakInput : uint32_t
    {
        kBreakInput  = LL_TIM_BREAK_INPUT_BKIN,
        kBreakInput2 = LL_TIM_BREAK_INPUT_BKIN2
    };

    enum class TimControllerBreakSource : uint32_t
    {
        kBreakInput  = LL_TIM_BKIN_SOURCE_BKIN,
        kComparator1 = LL_TIM_BKIN_SOURCE_BKCOMP1,
        kComparator2 = LL_TIM_BKIN_SOURCE_BKCOMP2,
        kComparator3 = LL_TIM_BKIN_SOURCE_BKCOMP3,
        kComparator4 = LL_TIM_BKIN_SOURCE_BKCOMP4,
        kComparator5 = LL_TIM_BKIN_SOURCE_BKCOMP5,
        kComparator6 = LL_TIM_BKIN_SOURCE_BKCOMP6,
        kComparator7 = LL_TIM_BKIN_SOURCE_BKCOMP7
    };

    // =========================================================================
    // TRAITS
    // =========================================================================

    // -------------------------------------------------------------------------
    // CONTROLLER
    // -------------------------------------------------------------------------

    template <TimPeripheralId tkPeripheralId>
    struct TimControllerTraits;

#define DEFINE_TIM_TRAITS(tkPeripheralId, num, bus, counter_type)         \
    template <>                                                           \
    struct TimControllerTraits<tkPeripheralId>                            \
    {                                                                     \
        using CounterValueT                         = counter_type;       \
        static inline TIM_TypeDef* const skInstance = TIM##num;           \
        inline void                      enable_clock() const             \
        {                                                                 \
            LL_##bus##_GRP1_EnableClock(LL_##bus##_GRP1_PERIPH_TIM##num); \
        }                                                                 \
    };

    DEFINE_TIM_TRAITS(TimPeripheralId::kTim1, 1, APB2, uint16_t);    // Advanced 16-bit
    DEFINE_TIM_TRAITS(TimPeripheralId::kTim2, 2, APB1, uint32_t);    // General Purpose 32-bit
    DEFINE_TIM_TRAITS(TimPeripheralId::kTim3, 3, APB1, uint16_t);    // General Purpose 16-bit
    DEFINE_TIM_TRAITS(TimPeripheralId::kTim4, 4, APB1, uint16_t);    // General Purpose 16-bit
    DEFINE_TIM_TRAITS(TimPeripheralId::kTim5, 5, APB1, uint32_t);    // General Purpose 32-bit
    DEFINE_TIM_TRAITS(TimPeripheralId::kTim6, 6, APB1, uint16_t);    // Basic 16-bit
    DEFINE_TIM_TRAITS(TimPeripheralId::kTim7, 7, APB1, uint16_t);    // Basic 16-bit
    DEFINE_TIM_TRAITS(TimPeripheralId::kTim8, 8, APB2, uint16_t);    // Advanced 16-bit
    DEFINE_TIM_TRAITS(TimPeripheralId::kTim15, 15, APB2, uint16_t);  // General Purpose 16-bit
    DEFINE_TIM_TRAITS(TimPeripheralId::kTim16, 16, APB2, uint16_t);  // General Purpose 16-bit
    DEFINE_TIM_TRAITS(TimPeripheralId::kTim17, 17, APB2, uint16_t);  // General Purpose 16-bit
    DEFINE_TIM_TRAITS(TimPeripheralId::kTim20, 20, APB2, uint16_t);  // Advanced 16-bit

#undef DEFINE_TIM_TRAITS

    // -------------------------------------------------------------------------
    // TIMER TRAITS
    // -------------------------------------------------------------------------
    template <TimPeripheralId tkPeripheralId, TimChannelId tkChannelId>
    struct TimChannelTraits;

    template <TimPeripheralId tkPeripheralId>
    struct TimChannelTraits<tkPeripheralId, TimChannelId::kCh1>
    {
        static constexpr uint32_t skLLChannelId              = LL_TIM_CHANNEL_CH1;
        static constexpr bool     skHasComplementaryChannel  = true;
        static constexpr uint32_t skLLComplementaryChannelId = LL_TIM_CHANNEL_CH1N;
    };

    template <TimPeripheralId tkPeripheralId>
    struct TimChannelTraits<tkPeripheralId, TimChannelId::kCh2>
    {
        static constexpr uint32_t skLLChannelId              = LL_TIM_CHANNEL_CH2;
        static constexpr bool     skHasComplementaryChannel  = true;
        static constexpr uint32_t skLLComplementaryChannelId = LL_TIM_CHANNEL_CH2N;
    };

    template <TimPeripheralId tkPeripheralId>
    struct TimChannelTraits<tkPeripheralId, TimChannelId::kCh3>
    {
        static constexpr uint32_t skLLChannelId              = LL_TIM_CHANNEL_CH3;
        static constexpr bool     skHasComplementaryChannel  = true;
        static constexpr uint32_t skLLComplementaryChannelId = LL_TIM_CHANNEL_CH3N;
    };

    template <TimPeripheralId tkPeripheralId>
    struct TimChannelTraits<tkPeripheralId, TimChannelId::kCh4>
    {
        static constexpr uint32_t skLLChannelId              = LL_TIM_CHANNEL_CH4;
        static constexpr bool     skHasComplementaryChannel  = true;
        static constexpr uint32_t skLLComplementaryChannelId = LL_TIM_CHANNEL_CH4N;
    };

    template <TimPeripheralId tkPeripheralId>
    struct TimChannelTraits<tkPeripheralId, TimChannelId::kCh5>
    {
        static constexpr uint32_t skLLChannelId             = LL_TIM_CHANNEL_CH5;
        static constexpr bool     skHasComplementaryChannel = true;
    };

    template <TimPeripheralId tkPeripheralId>
    struct TimChannelTraits<tkPeripheralId, TimChannelId::kCh6>
    {
        static constexpr uint32_t skLLChannelId             = LL_TIM_CHANNEL_CH6;
        static constexpr bool     skHasComplementaryChannel = true;
    };

    // ============================================================================
    // HARDWARE INTERFACES
    // ============================================================================

    template <TimPeripheralId tkPeripheralId>
    struct TimControllerInterface
    {
        using ControllerTraitsT = TimControllerTraits<tkPeripheralId>;
        using CounterValueT     = typename ControllerTraitsT::CounterValueT;

        // ---------------------------------------------------------------------
        // COUNTER CONTROL
        // ---------------------------------------------------------------------

        void enable_counter() const
        {
            LL_TIM_EnableCounter(ControllerTraitsT::skInstance);
        }
        void disable_counter() const
        {
            LL_TIM_DisableCounter(ControllerTraitsT::skInstance);
        }
        CounterValueT get_counter() const
        {
            return LL_TIM_GetCounter(ControllerTraitsT::skInstance);
        }
        void set_counter(const CounterValueT val) const
        {
            LL_TIM_SetCounter(ControllerTraitsT::skInstance, val);
        }
        void set_prescaler(const uint32_t val) const
        {
            LL_TIM_SetPrescaler(ControllerTraitsT::skInstance, val);
        }
        void set_auto_reload(const uint32_t val) const
        {
            LL_TIM_SetAutoReload(ControllerTraitsT::skInstance, val);
        }
        void set_counter_mode(const TimControllerCounterMode mode) const
        {
            LL_TIM_SetCounterMode(ControllerTraitsT::skInstance, static_cast<uint32_t>(mode));
        }

        void set_one_pulse_mode(const TimControllerOnePulseMode mode) const
        {
            LL_TIM_SetOnePulseMode(ControllerTraitsT::skInstance, static_cast<uint32_t>(mode));
        }

        void set_update_source(const TimControllerUpdateSource source) const
        {
            LL_TIM_SetUpdateSource(ControllerTraitsT::skInstance, static_cast<uint32_t>(source));
        }

        void enable_uiev_event() const
        {
            LL_TIM_EnableUpdateEvent(ControllerTraitsT::skInstance);
        }

        void disable_uiev_event() const
        {
            LL_TIM_DisableUpdateEvent(ControllerTraitsT::skInstance);
        }

        void enable_arr_preload() const
        {
            LL_TIM_EnableARRPreload(ControllerTraitsT::skInstance);
        }

        void disable_arr_preload() const
        {
            LL_TIM_DisableARRPreload(ControllerTraitsT::skInstance);
        }

        void set_repetition_counter(const uint32_t val) const
        {
            LL_TIM_SetRepetitionCounter(ControllerTraitsT::skInstance, val);
        }

        // ---------------------------------------------------------------------
        // HALL SENSOR / XOR CONTROL
        // ---------------------------------------------------------------------

        void enable_xor_combination() const
        {
            LL_TIM_IC_EnableXORCombination(ControllerTraitsT::skInstance);
        }

        void disable_xor_combination() const
        {
            LL_TIM_IC_DisableXORCombination(ControllerTraitsT::skInstance);
        }

        // ---------------------------------------------------------------------
        // ENCODER CONFIGURATION
        // ---------------------------------------------------------------------

#ifdef USE_FULL_LL_DRIVER
        static void config_encoder(const TimControllerEncoderMode       mode,
                                   const TimChannelInputCapturePolarity ti1_pol,
                                   const TimChannelInputCapturePolarity ti2_pol)
        {
            LL_TIM_ENCODER_InitTypeDef config;
            LL_TIM_ENCODER_StructInit(&config);
            config.EncoderMode = static_cast<uint32_t>(mode);
            config.IC1Polarity = static_cast<uint32_t>(ti1_pol);
            config.IC2Polarity = static_cast<uint32_t>(ti2_pol);
            LL_TIM_ENCODER_Init(ControllerTraitsT::skInstance, &config);
        }
#endif

        void set_encoder_mode(const TimControllerEncoderMode mode) const
        {
            LL_TIM_SetEncoderMode(ControllerTraitsT::skInstance, static_cast<uint32_t>(mode));
        }

        // ---------------------------------------------------------------------
        // MASTER / SLAVE CONTROL
        // ---------------------------------------------------------------------

        void set_slave_mode(const TimControllerSlaveMode mode) const
        {
            LL_TIM_SetSlaveMode(ControllerTraitsT::skInstance, static_cast<uint32_t>(mode));
        }

        void set_trigger_source(const TimControllerTriggerSource source) const
        {
            LL_TIM_SetTriggerSource(ControllerTraitsT::skInstance, static_cast<uint32_t>(source));
        }

        void set_trgo_source(const TimControllerTriggerOutputSource source) const
        {
            LL_TIM_SetTriggerOutput(ControllerTraitsT::skInstance, static_cast<uint32_t>(source));
        }

        void set_trgo2_source(const TimControllerTriggerOutput2Source source) const
        {
            LL_TIM_SetTriggerOutput2(ControllerTraitsT::skInstance, static_cast<uint32_t>(source));
        }

        // ---------------------------------------------------------------------
        // EVENT GENERATION
        // ---------------------------------------------------------------------

        void generate_update() const
        {
            LL_TIM_GenerateEvent_UPDATE(ControllerTraitsT::skInstance);
        }

        // ---------------------------------------------------------------------
        // DMA BURST CONTROL
        // ---------------------------------------------------------------------

        void config_dma_burst(const TimControllerDmaBurstBase base, const TimControllerDmaBurstLength len) const
        {
            LL_TIM_ConfigDmaBurst(
                ControllerTraitsT::skInstance, static_cast<uint32_t>(base), static_cast<uint32_t>(len));
        }

        // ---------------------------------------------------------------------
        // BDTR / BREAK CONTROL
        // ---------------------------------------------------------------------

        void enable_main_output() const
        {
            LL_TIM_EnableAllOutputs(ControllerTraitsT::skInstance);
        }

        void disable_main_output() const
        {
            LL_TIM_DisableAllOutputs(ControllerTraitsT::skInstance);
        }

        void set_deadtime(const uint32_t val) const
        {
            LL_TIM_OC_SetDeadTime(ControllerTraitsT::skInstance, val);
        }

        void enable_asymmetric_deadtime() const
        {
            LL_TIM_OC_EnableAsymmetricalDeadTime(ControllerTraitsT::skInstance);
        }

        void disable_asymmetric_deadtime() const
        {
            LL_TIM_OC_DisableAsymmetricalDeadTime(ControllerTraitsT::skInstance);
        }

        void set_falling_deadtime(const uint32_t val) const
        {
            LL_TIM_OC_SetFallingDeadTime(ControllerTraitsT::skInstance, val);
        }

        // ---------------------------------------------------------------------
        // BREAK CONTROL
        // ---------------------------------------------------------------------

        void enable_break() const
        {
            LL_TIM_EnableBRK(ControllerTraitsT::skInstance);
        }

        void disable_break() const
        {
            LL_TIM_DisableBRK(ControllerTraitsT::skInstance);
        }

        void enable_break2() const
        {
            LL_TIM_EnableBRK2(ControllerTraitsT::skInstance);
        }

        void disable_break2() const
        {
            LL_TIM_DisableBRK2(ControllerTraitsT::skInstance);
        }

        void set_break_polarity(const TimControllerBreakPolarity pol) const
        {
            LL_TIM_ConfigBRK(ControllerTraitsT::skInstance, static_cast<uint32_t>(pol));
        }

        void set_break2_polarity(const TimControllerBreakPolarity pol) const
        {
            LL_TIM_ConfigBRK2(ControllerTraitsT::skInstance, static_cast<uint32_t>(pol));
        }

        void set_break_filter(const uint32_t filter) const
        {
            LL_TIM_SetBreakFilter(ControllerTraitsT::skInstance, filter);
        }

        void set_break2_filter(const uint32_t filter) const
        {
            LL_TIM_SetBreak2Filter(ControllerTraitsT::skInstance, filter);
        }

        void enable_break_source(const TimControllerBreakInput input, const TimControllerBreakSource source) const
        {
            LL_TIM_EnableBreakInputSource(
                ControllerTraitsT::skInstance, static_cast<uint32_t>(input), static_cast<uint32_t>(source));
        }

        void disable_break_source(const TimControllerBreakInput input, const TimControllerBreakSource source) const
        {
            LL_TIM_DisableBreakInputSource(
                ControllerTraitsT::skInstance, static_cast<uint32_t>(input), static_cast<uint32_t>(source));
        }

        void disarm_break() const
        {
            LL_TIM_DisarmBRK(ControllerTraitsT::skInstance);
        }

        void rearm_break() const
        {
            LL_TIM_ReArmBRK(ControllerTraitsT::skInstance);
        }

#ifdef USE_FULL_LL_DRIVER
        static void config_bdtr(const TimControllerOffState      ossr,
                                const TimControllerOffState      ossi,
                                const TimControllerLockLevel     lock,
                                const uint32_t                   deadtime,
                                const TimControllerBreakPolarity break_pol,
                                const TimControllerBreakAFMode   break_af)
        {
            LL_TIM_BDTR_InitTypeDef config;
            LL_TIM_BDTR_StructInit(&config);
            config.OSSRState     = (ossr == TimControllerOffState::kRun) ? LL_TIM_OSSR_ENABLE : LL_TIM_OSSR_DISABLE;
            config.OSSIState     = (ossi == TimControllerOffState::kIdle) ? LL_TIM_OSSI_ENABLE : LL_TIM_OSSI_DISABLE;
            config.LockLevel     = static_cast<uint32_t>(lock);
            config.DeadTime      = deadtime;
            config.BreakState    = LL_TIM_BREAK_ENABLE;
            config.BreakPolarity = static_cast<uint32_t>(break_pol);
            config.BreakAFMode   = static_cast<uint32_t>(break_af);
            LL_TIM_BDTR_Init(ControllerTraitsT::skInstance, &config);
        }
#endif
    };

    // =========================================================================
    // CHANNEL INTERFACE
    // =========================================================================

    template <TimPeripheralId tkPeripheralId, TimChannelId tkChannel>
    struct TimChannelInterface
    {
        using ControllerTraitsT = TimControllerTraits<tkPeripheralId>;
        using ChannelTraitsT    = TimChannelTraits<tkPeripheralId, tkChannel>;

        // ---------------------------------------------------------------------
        // CHANNEL ENABLE
        // ---------------------------------------------------------------------

        void enable() const
        {
            LL_TIM_CC_EnableChannel(ControllerTraitsT::skInstance, ChannelTraitsT::skLLChannelId);
        }
        void disable() const
        {
            LL_TIM_CC_DisableChannel(ControllerTraitsT::skInstance, ChannelTraitsT::skLLChannelId);
        }

        void enable_complementary() const
        {
            LL_TIM_CC_EnableChannel(ControllerTraitsT::skInstance, ChannelTraitsT::skLLComplementaryChannelId);
        }

        void disable_complementary() const
        {
            LL_TIM_CC_DisableChannel(ControllerTraitsT::skInstance, ChannelTraitsT::skLLComplementaryChannelId);
        }

        // ---------------------------------------------------------------------
        // OUTPUT COMPARE / PWM
        // ---------------------------------------------------------------------

        void set_compare(const uint32_t val) const
        {
            switch (tkChannel)
            {
                case TimChannelId::kCh1:
                    LL_TIM_OC_SetCompareCH1(ControllerTraitsT::skInstance, val);
                    break;
                case TimChannelId::kCh2:
                    LL_TIM_OC_SetCompareCH2(ControllerTraitsT::skInstance, val);
                    break;
                case TimChannelId::kCh3:
                    LL_TIM_OC_SetCompareCH3(ControllerTraitsT::skInstance, val);
                    break;
                case TimChannelId::kCh4:
                    LL_TIM_OC_SetCompareCH4(ControllerTraitsT::skInstance, val);
                    break;
                case TimChannelId::kCh5:
                    LL_TIM_OC_SetCompareCH5(ControllerTraitsT::skInstance, val);
                    break;
                case TimChannelId::kCh6:
                    LL_TIM_OC_SetCompareCH6(ControllerTraitsT::skInstance, val);
                    break;
                default:
                    break;
            }
        }

        uint32_t get_compare() const
        {
            return get_capture();  // Alias
        }

        void set_mode(const TimChannelOutputCaptureMode mode) const
        {
            LL_TIM_OC_SetMode(
                ControllerTraitsT::skInstance, ChannelTraitsT::skLLChannelId, static_cast<uint32_t>(mode));
        }

        void set_polarity(const TimChannelOutputCapturePolarity pol) const
        {
            LL_TIM_OC_SetPolarity(
                ControllerTraitsT::skInstance, ChannelTraitsT::skLLChannelId, static_cast<uint32_t>(pol));
        }

        void set_idle_state(const TimChannelOutputCaptureIdleState state) const
        {
            LL_TIM_OC_SetIdleState(
                ControllerTraitsT::skInstance, ChannelTraitsT::skLLChannelId, static_cast<uint32_t>(state));
        }

        void enable_output_capture_preload() const
        {
            LL_TIM_OC_EnablePreload(ControllerTraitsT::skInstance, ChannelTraitsT::skLLChannelId);
        }

        void disable_output_capture_preload() const
        {
            LL_TIM_OC_DisablePreload(ControllerTraitsT::skInstance, ChannelTraitsT::skLLChannelId);
        }

        void set_output_capture_preload(const bool enable) const
        {
            if (enable)
            {
                enable_output_capture_preload();
            }
            else
            {
                disable_output_capture_preload();
            }
        }

        void enable_output_capture_fast_mode() const
        {
            LL_TIM_OC_EnableFast(ControllerTraitsT::skInstance, ChannelTraitsT::skLLChannelId);
        }

        void disable_output_capture_fast_mode() const
        {
            LL_TIM_OC_DisableFast(ControllerTraitsT::skInstance, ChannelTraitsT::skLLChannelId);
        }

        void set_output_capture_fast_mode(const bool enable) const
        {
            if (enable)
            {
                enable_output_capture_fast_mode();
            }
            else
            {
                disable_output_capture_fast_mode();
            }
        }

        void enable_output_capture_clear() const
        {
            LL_TIM_OC_EnableClear(ControllerTraitsT::skInstance, ChannelTraitsT::skLLChannelId);
        }

        void disable_output_capture_clear() const
        {
            LL_TIM_OC_DisableClear(ControllerTraitsT::skInstance, ChannelTraitsT::skLLChannelId);
        }

        void set_output_capture_clear(const bool enable) const
        {
            if (enable)
            {
                enable_output_capture_clear();
            }
            else
            {
                disable_output_capture_clear();
            }
        }

        void set_complementary_output_capture_polarity(const TimChannelOutputCapturePolarity pol) const
        {
            LL_TIM_OC_SetPolarity(
                ControllerTraitsT::skInstance, ChannelTraitsT::skLLComplementaryChannelId, static_cast<uint32_t>(pol));
        }

        void set_complementary_output_capture_idle_state(const TimChannelOutputCaptureIdleState state) const
        {
            LL_TIM_OC_SetIdleState(ControllerTraitsT::skInstance,
                                   ChannelTraitsT::skLLComplementaryChannelId,
                                   static_cast<uint32_t>(state));
        }

#ifdef USE_FULL_LL_DRIVER
        static void config_output_capture(const TimChannelOutputCaptureMode      mode,
                                          const uint32_t                         compare,
                                          const TimChannelOutputCapturePolarity  pol,
                                          const TimChannelOutputCaptureIdleState idle)
        {
            LL_TIM_OC_InitTypeDef config;
            LL_TIM_OC_StructInit(&config);
            config.OCMode       = static_cast<uint32_t>(mode);
            config.OCState      = LL_TIM_OCSTATE_ENABLE;
            config.OCNState     = LL_TIM_OCSTATE_ENABLE;
            config.CompareValue = compare;
            config.OCPolarity   = static_cast<uint32_t>(pol);
            config.OCNPolarity  = static_cast<uint32_t>(pol);
            config.OCIdleState  = static_cast<uint32_t>(idle);
            config.OCNIdleState = static_cast<uint32_t>(idle);
            LL_TIM_OC_Init(ControllerTraitsT::skInstance, ChannelTraitsT::skLLChannelId, &config);
        }
#endif

        // ---------------------------------------------------------------------
        // INPUT CAPTURE
        // ---------------------------------------------------------------------

        void set_input_capture_active_input(const TimChannelInputCaptureActiveInput input) const
        {
            LL_TIM_IC_SetActiveInput(
                ControllerTraitsT::skInstance, ChannelTraitsT::skLLChannelId, static_cast<uint32_t>(input));
        }

        void set_input_capture_prescaler(const TimChannelInputCapturePrescaler psc) const
        {
            LL_TIM_IC_SetPrescaler(
                ControllerTraitsT::skInstance, ChannelTraitsT::skLLChannelId, static_cast<uint32_t>(psc));
        }

        void set_input_capture_filter(const TimChannelInputCaptureFilter filter) const
        {
            LL_TIM_IC_SetFilter(
                ControllerTraitsT::skInstance, ChannelTraitsT::skLLChannelId, static_cast<uint32_t>(filter));
        }

        void set_input_capture_polarity(const TimChannelInputCapturePolarity polarity) const
        {
            LL_TIM_IC_SetPolarity(
                ControllerTraitsT::skInstance, ChannelTraitsT::skLLChannelId, static_cast<uint32_t>(polarity));
        }

#ifdef USE_FULL_LL_DRIVER
        static void config_input_capture(const TimChannelInputCaptureActiveInput input,
                                         const TimChannelInputCapturePrescaler   psc,
                                         const TimChannelInputCaptureFilter      filter,
                                         const TimChannelInputCapturePolarity    polarity)
        {
            LL_TIM_IC_InitTypeDef config;
            LL_TIM_IC_StructInit(&config);
            config.ICActiveInput = static_cast<uint32_t>(input);
            config.ICPrescaler   = static_cast<uint32_t>(psc);
            config.ICFilter      = static_cast<uint32_t>(filter);
            config.ICPolarity    = static_cast<uint32_t>(polarity);
            LL_TIM_IC_Init(ControllerTraitsT::skInstance, ChannelTraitsT::skLLChannelId, &config);
        }
#endif

        uint32_t get_capture() const
        {
            switch (tkChannel)
            {
                case TimChannelId::kCh1:
                    return LL_TIM_OC_GetCompareCH1(ControllerTraitsT::skInstance);
                case TimChannelId::kCh2:
                    return LL_TIM_OC_GetCompareCH2(ControllerTraitsT::skInstance);
                case TimChannelId::kCh3:
                    return LL_TIM_OC_GetCompareCH3(ControllerTraitsT::skInstance);
                case TimChannelId::kCh4:
                    return LL_TIM_OC_GetCompareCH4(ControllerTraitsT::skInstance);
                case TimChannelId::kCh5:
                    return LL_TIM_OC_GetCompareCH5(ControllerTraitsT::skInstance);
                case TimChannelId::kCh6:
                    return LL_TIM_OC_GetCompareCH6(ControllerTraitsT::skInstance);
                default:
                    return 0;
            }
        }
    };
}  // namespace valle::platform