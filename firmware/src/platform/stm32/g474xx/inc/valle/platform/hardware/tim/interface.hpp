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

    enum class TIMControllerCounterMode : uint32_t
    {
        kUp           = LL_TIM_COUNTERMODE_UP,
        kDown         = LL_TIM_COUNTERMODE_DOWN,
        kCenterDown   = LL_TIM_COUNTERMODE_CENTER_DOWN,
        kCenterUp     = LL_TIM_COUNTERMODE_CENTER_UP,
        kCenterUpDown = LL_TIM_COUNTERMODE_CENTER_UP_DOWN
    };

    enum class TIMControllerDirection : uint32_t
    {
        kUp   = LL_TIM_COUNTERDIRECTION_UP,
        kDown = LL_TIM_COUNTERDIRECTION_DOWN
    };

    enum class TIMControllerClockDivision : uint32_t
    {
        kDiv1 = LL_TIM_CLOCKDIVISION_DIV1,
        kDiv2 = LL_TIM_CLOCKDIVISION_DIV2,
        kDiv4 = LL_TIM_CLOCKDIVISION_DIV4
    };

    enum class TIMControllerUpdateSource : uint32_t
    {
        kRegular = LL_TIM_UPDATESOURCE_REGULAR,
        kCounter = LL_TIM_UPDATESOURCE_COUNTER
    };

    enum class TIMControllerDMABurstBase : uint32_t
    {
        kControlRegister1     = LL_TIM_DMABURST_BASEADDR_CR1,
        kControlRegister2     = LL_TIM_DMABURST_BASEADDR_CR2,
        kSlaveModeControl     = LL_TIM_DMABURST_BASEADDR_SMCR,
        kDMAInterruptEnable   = LL_TIM_DMABURST_BASEADDR_DIER,
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

    enum class TIMControllerDMABurstLength : uint32_t
    {
        k1Transfer   = LL_TIM_DMABURST_LENGTH_1TRANSFER,
        k2Transfers  = LL_TIM_DMABURST_LENGTH_2TRANSFERS,
        k3Transfers  = LL_TIM_DMABURST_LENGTH_3TRANSFERS,
        k4Transfers  = LL_TIM_DMABURST_LENGTH_4TRANSFERS,
        k18Transfers = LL_TIM_DMABURST_LENGTH_18TRANSFERS
    };

    enum class TIMControllerOnePulseMode : uint32_t
    {
        kRepetitive = LL_TIM_ONEPULSEMODE_REPETITIVE,
        kSingle     = LL_TIM_ONEPULSEMODE_SINGLE
    };

    enum class TIMControllerUpdateMode : uint32_t
    {
        kImmediate = 0,
        kPreload   = 1
    };

    // -------------------------------------------------------------------------
    // ENCODER
    // -------------------------------------------------------------------------

    enum class TIMControllerEncoderMode : uint32_t
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

    enum class TIMChannelInputCapturePolarity : uint32_t
    {
        kRising    = LL_TIM_IC_POLARITY_RISING,
        kFalling   = LL_TIM_IC_POLARITY_FALLING,
        kBothEdges = LL_TIM_IC_POLARITY_BOTHEDGE
    };

    enum class TIMChannelInputCaptureActiveInput : uint32_t
    {
        kDirectTimerInput    = LL_TIM_ACTIVEINPUT_DIRECTTI,
        kIndirectTimerInput  = LL_TIM_ACTIVEINPUT_INDIRECTTI,
        kTimerReferenceClock = LL_TIM_ACTIVEINPUT_TRC
    };

    enum class TIMChannelInputCapturePrescaler : uint32_t
    {
        kDiv1 = LL_TIM_ICPSC_DIV1,
        kDiv2 = LL_TIM_ICPSC_DIV2,
        kDiv4 = LL_TIM_ICPSC_DIV4,
        kDiv8 = LL_TIM_ICPSC_DIV8
    };

    enum class TIMChannelInputCaptureFilter : uint32_t
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

    enum class TIMChannelOutputCaptureMode : uint32_t
    {
        kFrozen                     = LL_TIM_OCMODE_FROZEN,
        kActive                     = LL_TIM_OCMODE_ACTIVE,
        kInactive                   = LL_TIM_OCMODE_INACTIVE,
        kToggle                     = LL_TIM_OCMODE_TOGGLE,
        kForcedInactive             = LL_TIM_OCMODE_FORCED_INACTIVE,
        kForcedActive               = LL_TIM_OCMODE_FORCED_ACTIVE,
        kPWM1                       = LL_TIM_OCMODE_PWM1,
        kPWM2                       = LL_TIM_OCMODE_PWM2,
        kRetriggerableOnePulseMode1 = LL_TIM_OCMODE_RETRIG_OPM1,
        kRetriggerableOnePulseMode2 = LL_TIM_OCMODE_RETRIG_OPM2,
        kCombinedPWM1               = LL_TIM_OCMODE_COMBINED_PWM1,
        kCombinedPWM2               = LL_TIM_OCMODE_COMBINED_PWM2,
        kAsymmetricPWM1             = LL_TIM_OCMODE_ASYMMETRIC_PWM1,
        kAsymmetricPWM2             = LL_TIM_OCMODE_ASYMMETRIC_PWM2,
        kDirectionOutput            = LL_TIM_OCMODE_DIRECTION_OUTPUT
    };

    enum class TIMChannelOutputCapturePolarity : uint32_t
    {
        kHigh = LL_TIM_OCPOLARITY_HIGH,
        kLow  = LL_TIM_OCPOLARITY_LOW
    };

    enum class TIMChannelOutputCaptureIdleState : uint32_t
    {
        kLow  = LL_TIM_OCIDLESTATE_LOW,
        kHigh = LL_TIM_OCIDLESTATE_HIGH
    };

    // -------------------------------------------------------------------------
    // MASTER / SLAVE / TRIGGER ENUMS
    // -------------------------------------------------------------------------

    enum class TIMControllerTriggerOutputSource : uint32_t
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

    enum class TIMControllerTriggerOutput2Source : uint32_t
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

    enum class TIMControllerSlaveMode : uint32_t
    {
        kDisabled     = LL_TIM_SLAVEMODE_DISABLED,
        kReset        = LL_TIM_SLAVEMODE_RESET,
        kGated        = LL_TIM_SLAVEMODE_GATED,
        kTrigger      = LL_TIM_SLAVEMODE_TRIGGER,
        kResetTrigger = LL_TIM_SLAVEMODE_COMBINED_RESETTRIGGER,
        kGatedReset   = LL_TIM_SLAVEMODE_COMBINED_GATEDRESET
    };

    enum class TIMControllerTriggerSource : uint32_t
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

    enum class TIMControllerBreakPolarity : uint32_t
    {
        kLow  = LL_TIM_BREAK_POLARITY_LOW,
        kHigh = LL_TIM_BREAK_POLARITY_HIGH
    };

    enum class TIMControllerBreakAFMode : uint32_t
    {
        kInput         = LL_TIM_BREAK_AFMODE_INPUT,
        kBidirectional = LL_TIM_BREAK_AFMODE_BIDIRECTIONAL
    };

    enum class TIMControllerOffState : uint32_t
    {
        kRun  = LL_TIM_OSSR_ENABLE,
        kIdle = LL_TIM_OSSI_ENABLE
    };

    enum class TIMControllerLockLevel : uint32_t
    {
        kOff = LL_TIM_LOCKLEVEL_OFF,
        k1   = LL_TIM_LOCKLEVEL_1,
        k2   = LL_TIM_LOCKLEVEL_2,
        k3   = LL_TIM_LOCKLEVEL_3
    };

    enum class TIMControllerBreakInput : uint32_t
    {
        kBreakInput  = LL_TIM_BREAK_INPUT_BKIN,
        kBreakInput2 = LL_TIM_BREAK_INPUT_BKIN2
    };

    enum class TIMControllerBreakSource : uint32_t
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

    template <TIMControllerID tkControllerID>
    struct TIMControllerTraits;

#define DEFINE_TIM_TRAITS(tkControllerID, num, bus, counter_type)         \
    template <>                                                           \
    struct TIMControllerTraits<tkControllerID>                            \
    {                                                                     \
        using CounterValueT                         = counter_type;       \
        static inline TIM_TypeDef* const skInstance = TIM##num;           \
        static inline void               enable_clock()                   \
        {                                                                 \
            LL_##bus##_GRP1_EnableClock(LL_##bus##_GRP1_PERIPH_TIM##num); \
        }                                                                 \
    };

    DEFINE_TIM_TRAITS(TIMControllerID::kTim1, 1, APB2, uint16_t);    // Advanced 16-bit
    DEFINE_TIM_TRAITS(TIMControllerID::kTim2, 2, APB1, uint32_t);    // General Purpose 32-bit
    DEFINE_TIM_TRAITS(TIMControllerID::kTim3, 3, APB1, uint16_t);    // General Purpose 16-bit
    DEFINE_TIM_TRAITS(TIMControllerID::kTim4, 4, APB1, uint16_t);    // General Purpose 16-bit
    DEFINE_TIM_TRAITS(TIMControllerID::kTim5, 5, APB1, uint32_t);    // General Purpose 32-bit
    DEFINE_TIM_TRAITS(TIMControllerID::kTim6, 6, APB1, uint16_t);    // Basic 16-bit
    DEFINE_TIM_TRAITS(TIMControllerID::kTim7, 7, APB1, uint16_t);    // Basic 16-bit
    DEFINE_TIM_TRAITS(TIMControllerID::kTim8, 8, APB2, uint16_t);    // Advanced 16-bit
    DEFINE_TIM_TRAITS(TIMControllerID::kTim15, 15, APB2, uint16_t);  // General Purpose 16-bit
    DEFINE_TIM_TRAITS(TIMControllerID::kTim16, 16, APB2, uint16_t);  // General Purpose 16-bit
    DEFINE_TIM_TRAITS(TIMControllerID::kTim17, 17, APB2, uint16_t);  // General Purpose 16-bit
    DEFINE_TIM_TRAITS(TIMControllerID::kTim20, 20, APB2, uint16_t);  // Advanced 16-bit

#undef DEFINE_TIM_TRAITS

    // -------------------------------------------------------------------------
    // TIMER TRAITS
    // -------------------------------------------------------------------------
    template <TIMControllerID tkControllerID, TIMChannelID tkChannelID>
    struct TIMChannelTraits;

    template <TIMControllerID tkControllerID>
    struct TIMChannelTraits<tkControllerID, TIMChannelID::kCh1>
    {
        static constexpr uint32_t skLLChannelID              = LL_TIM_CHANNEL_CH1;
        static constexpr bool     skHasComplementaryChannel  = true;
        static constexpr uint32_t skLLComplementaryChannelID = LL_TIM_CHANNEL_CH1N;
    };

    template <TIMControllerID tkControllerID>
    struct TIMChannelTraits<tkControllerID, TIMChannelID::kCh2>
    {
        static constexpr uint32_t skLLChannelID              = LL_TIM_CHANNEL_CH2;
        static constexpr bool     skHasComplementaryChannel  = true;
        static constexpr uint32_t skLLComplementaryChannelID = LL_TIM_CHANNEL_CH2N;
    };

    template <TIMControllerID tkControllerID>
    struct TIMChannelTraits<tkControllerID, TIMChannelID::kCh3>
    {
        static constexpr uint32_t skLLChannelID              = LL_TIM_CHANNEL_CH3;
        static constexpr bool     skHasComplementaryChannel  = true;
        static constexpr uint32_t skLLComplementaryChannelID = LL_TIM_CHANNEL_CH3N;
    };

    template <TIMControllerID tkControllerID>
    struct TIMChannelTraits<tkControllerID, TIMChannelID::kCh4>
    {
        static constexpr uint32_t skLLChannelID              = LL_TIM_CHANNEL_CH4;
        static constexpr bool     skHasComplementaryChannel  = true;
        static constexpr uint32_t skLLComplementaryChannelID = LL_TIM_CHANNEL_CH4N;
    };

    template <TIMControllerID tkControllerID>
    struct TIMChannelTraits<tkControllerID, TIMChannelID::kCh5>
    {
        static constexpr uint32_t skLLChannelID             = LL_TIM_CHANNEL_CH5;
        static constexpr bool     skHasComplementaryChannel = true;
    };

    template <TIMControllerID tkControllerID>
    struct TIMChannelTraits<tkControllerID, TIMChannelID::kCh6>
    {
        static constexpr uint32_t skLLChannelID             = LL_TIM_CHANNEL_CH6;
        static constexpr bool     skHasComplementaryChannel = true;
    };

    // =========================================================================
    // Interface
    // =========================================================================

    template <TIMControllerID tkControllerID>
    struct TIMControllerInterface
    {
        using ControllerTraitsT = TIMControllerTraits<tkControllerID>;
        using CounterValueT     = typename ControllerTraitsT::CounterValueT;

        // ---------------------------------------------------------------------
        // COUNTER CONTROL
        // ---------------------------------------------------------------------

        static void enable_counter()
        {
            LL_TIM_EnableCounter(ControllerTraitsT::skInstance);
        }
        static void disable_counter()
        {
            LL_TIM_DisableCounter(ControllerTraitsT::skInstance);
        }
        static CounterValueT get_counter()
        {
            return LL_TIM_GetCounter(ControllerTraitsT::skInstance);
        }
        static void set_counter(const CounterValueT val)
        {
            LL_TIM_SetCounter(ControllerTraitsT::skInstance, val);
        }
        static void set_prescaler(const uint32_t val)
        {
            LL_TIM_SetPrescaler(ControllerTraitsT::skInstance, val);
        }
        static void set_auto_reload(const uint32_t val)
        {
            LL_TIM_SetAutoReload(ControllerTraitsT::skInstance, val);
        }
        static void set_counter_mode(const TIMControllerCounterMode mode)
        {
            LL_TIM_SetCounterMode(ControllerTraitsT::skInstance, static_cast<uint32_t>(mode));
        }

        static void set_one_pulse_mode(const TIMControllerOnePulseMode mode)
        {
            LL_TIM_SetOnePulseMode(ControllerTraitsT::skInstance, static_cast<uint32_t>(mode));
        }

        static void set_update_source(const TIMControllerUpdateSource source)
        {
            LL_TIM_SetUpdateSource(ControllerTraitsT::skInstance, static_cast<uint32_t>(source));
        }

        static void enable_uiev_event()
        {
            LL_TIM_EnableUpdateEvent(ControllerTraitsT::skInstance);
        }

        static void disable_uiev_event()
        {
            LL_TIM_DisableUpdateEvent(ControllerTraitsT::skInstance);
        }

        static void enable_arr_preload()
        {
            LL_TIM_EnableARRPreload(ControllerTraitsT::skInstance);
        }

        static void disable_arr_preload()
        {
            LL_TIM_DisableARRPreload(ControllerTraitsT::skInstance);
        }

        static void set_repetition_counter(const uint32_t val)
        {
            LL_TIM_SetRepetitionCounter(ControllerTraitsT::skInstance, val);
        }

        // ---------------------------------------------------------------------
        // HALL SENSOR / XOR CONTROL
        // ---------------------------------------------------------------------

        static void enable_xor_combination()
        {
            LL_TIM_IC_EnableXORCombination(ControllerTraitsT::skInstance);
        }

        static void disable_xor_combination()
        {
            LL_TIM_IC_DisableXORCombination(ControllerTraitsT::skInstance);
        }

        // ---------------------------------------------------------------------
        // ENCODER CONFIGURATION
        // ---------------------------------------------------------------------

#ifdef USE_FULL_LL_DRIVER
        static void config_encoder(const TIMControllerEncoderMode       mode,
                                   const TIMChannelInputCapturePolarity ti1_pol,
                                   const TIMChannelInputCapturePolarity ti2_pol)
        {
            LL_TIM_ENCODER_InitTypeDef config;
            LL_TIM_ENCODER_StructInit(&config);
            config.EncoderMode = static_cast<uint32_t>(mode);
            config.IC1Polarity = static_cast<uint32_t>(ti1_pol);
            config.IC2Polarity = static_cast<uint32_t>(ti2_pol);
            LL_TIM_ENCODER_Init(ControllerTraitsT::skInstance, &config);
        }
#endif

        static void set_encoder_mode(const TIMControllerEncoderMode mode)
        {
            LL_TIM_SetEncoderMode(ControllerTraitsT::skInstance, static_cast<uint32_t>(mode));
        }

        // ---------------------------------------------------------------------
        // MASTER / SLAVE CONTROL
        // ---------------------------------------------------------------------

        static void set_slave_mode(const TIMControllerSlaveMode mode)
        {
            LL_TIM_SetSlaveMode(ControllerTraitsT::skInstance, static_cast<uint32_t>(mode));
        }

        static void set_trigger_source(const TIMControllerTriggerSource source)
        {
            LL_TIM_SetTriggerSource(ControllerTraitsT::skInstance, static_cast<uint32_t>(source));
        }

        static void set_trgo_source(const TIMControllerTriggerOutputSource source)
        {
            LL_TIM_SetTriggerOutput(ControllerTraitsT::skInstance, static_cast<uint32_t>(source));
        }

        static void set_trgo2_source(const TIMControllerTriggerOutput2Source source)
        {
            LL_TIM_SetTriggerOutput2(ControllerTraitsT::skInstance, static_cast<uint32_t>(source));
        }

        // ---------------------------------------------------------------------
        // EVENT GENERATION
        // ---------------------------------------------------------------------

        static void generate_update()
        {
            LL_TIM_GenerateEvent_UPDATE(ControllerTraitsT::skInstance);
        }

        // ---------------------------------------------------------------------
        // DMA BURST CONTROL
        // ---------------------------------------------------------------------

        static void config_dma_burst(const TIMControllerDMABurstBase base, const TIMControllerDMABurstLength len)
        {
            LL_TIM_ConfigDMABurst(
                ControllerTraitsT::skInstance, static_cast<uint32_t>(base), static_cast<uint32_t>(len));
        }

        // ---------------------------------------------------------------------
        // BDTR / BREAK CONTROL
        // ---------------------------------------------------------------------

        static void enable_main_output()
        {
            LL_TIM_EnableAllOutputs(ControllerTraitsT::skInstance);
        }

        static void disable_main_output()
        {
            LL_TIM_DisableAllOutputs(ControllerTraitsT::skInstance);
        }

        static void set_deadtime(const uint32_t val)
        {
            LL_TIM_OC_SetDeadTime(ControllerTraitsT::skInstance, val);
        }

        static void enable_asymmetric_deadtime()
        {
            LL_TIM_OC_EnableAsymmetricalDeadTime(ControllerTraitsT::skInstance);
        }

        static void disable_asymmetric_deadtime()
        {
            LL_TIM_OC_DisableAsymmetricalDeadTime(ControllerTraitsT::skInstance);
        }

        static void set_falling_deadtime(const uint32_t val)
        {
            LL_TIM_OC_SetFallingDeadTime(ControllerTraitsT::skInstance, val);
        }

        // ---------------------------------------------------------------------
        // BREAK CONTROL
        // ---------------------------------------------------------------------

        static void enable_break()
        {
            LL_TIM_EnableBRK(ControllerTraitsT::skInstance);
        }

        static void disable_break()
        {
            LL_TIM_DisableBRK(ControllerTraitsT::skInstance);
        }

        static void enable_break2()
        {
            LL_TIM_EnableBRK2(ControllerTraitsT::skInstance);
        }

        static void disable_break2()
        {
            LL_TIM_DisableBRK2(ControllerTraitsT::skInstance);
        }

        static void set_break_polarity(const TIMControllerBreakPolarity pol)
        {
            LL_TIM_ConfigBRK(ControllerTraitsT::skInstance, static_cast<uint32_t>(pol));
        }

        static void set_break2_polarity(const TIMControllerBreakPolarity pol)
        {
            LL_TIM_ConfigBRK2(ControllerTraitsT::skInstance, static_cast<uint32_t>(pol));
        }

        static void set_break_filter(const uint32_t filter)
        {
            LL_TIM_SetBreakFilter(ControllerTraitsT::skInstance, filter);
        }

        static void set_break2_filter(const uint32_t filter)
        {
            LL_TIM_SetBreak2Filter(ControllerTraitsT::skInstance, filter);
        }

        static void enable_break_source(const TIMControllerBreakInput input, const TIMControllerBreakSource source)
        {
            LL_TIM_EnableBreakInputSource(
                ControllerTraitsT::skInstance, static_cast<uint32_t>(input), static_cast<uint32_t>(source));
        }

        static void disable_break_source(const TIMControllerBreakInput input, const TIMControllerBreakSource source)
        {
            LL_TIM_DisableBreakInputSource(
                ControllerTraitsT::skInstance, static_cast<uint32_t>(input), static_cast<uint32_t>(source));
        }

        static void disarm_break()
        {
            LL_TIM_DisarmBRK(ControllerTraitsT::skInstance);
        }

        static void rearm_break()
        {
            LL_TIM_ReArmBRK(ControllerTraitsT::skInstance);
        }

#ifdef USE_FULL_LL_DRIVER
        static void config_bdtr(const TIMControllerOffState      ossr,
                                const TIMControllerOffState      ossi,
                                const TIMControllerLockLevel     lock,
                                const uint32_t                   deadtime,
                                const TIMControllerBreakPolarity break_pol,
                                const TIMControllerBreakAFMode   break_af)
        {
            LL_TIM_BDTR_InitTypeDef config;
            LL_TIM_BDTR_StructInit(&config);
            config.OSSRState     = (ossr == TIMControllerOffState::kRun) ? LL_TIM_OSSR_ENABLE : LL_TIM_OSSR_DISABLE;
            config.OSSIState     = (ossi == TIMControllerOffState::kIdle) ? LL_TIM_OSSI_ENABLE : LL_TIM_OSSI_DISABLE;
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

    template <TIMControllerID tkControllerID, TIMChannelID tkChannel>
    struct TIMChannelInterface
    {
        using ControllerTraitsT = TIMControllerTraits<tkControllerID>;
        using ChannelTraitsT    = TIMChannelTraits<tkControllerID, tkChannel>;

        // ---------------------------------------------------------------------
        // CHANNEL ENABLE
        // ---------------------------------------------------------------------

        static void enable()
        {
            LL_TIM_CC_EnableChannel(ControllerTraitsT::skInstance, ChannelTraitsT::skLLChannelID);
        }
        static void disable()
        {
            LL_TIM_CC_DisableChannel(ControllerTraitsT::skInstance, ChannelTraitsT::skLLChannelID);
        }

        static void enable_complementary()
        {
            LL_TIM_CC_EnableChannel(ControllerTraitsT::skInstance, ChannelTraitsT::skLLComplementaryChannelID);
        }

        static void disable_complementary()
        {
            LL_TIM_CC_DisableChannel(ControllerTraitsT::skInstance, ChannelTraitsT::skLLComplementaryChannelID);
        }

        // ---------------------------------------------------------------------
        // OUTPUT COMPARE / PWM
        // ---------------------------------------------------------------------

        static void set_compare(const uint32_t val)
        {
            switch (tkChannel)
            {
                case TIMChannelID::kCh1:
                    LL_TIM_OC_SetCompareCH1(ControllerTraitsT::skInstance, val);
                    break;
                case TIMChannelID::kCh2:
                    LL_TIM_OC_SetCompareCH2(ControllerTraitsT::skInstance, val);
                    break;
                case TIMChannelID::kCh3:
                    LL_TIM_OC_SetCompareCH3(ControllerTraitsT::skInstance, val);
                    break;
                case TIMChannelID::kCh4:
                    LL_TIM_OC_SetCompareCH4(ControllerTraitsT::skInstance, val);
                    break;
                case TIMChannelID::kCh5:
                    LL_TIM_OC_SetCompareCH5(ControllerTraitsT::skInstance, val);
                    break;
                case TIMChannelID::kCh6:
                    LL_TIM_OC_SetCompareCH6(ControllerTraitsT::skInstance, val);
                    break;
                default:
                    break;
            }
        }

        static uint32_t get_compare()
        {
            return get_capture();  // Alias
        }

        static void set_mode(const TIMChannelOutputCaptureMode mode)
        {
            LL_TIM_OC_SetMode(
                ControllerTraitsT::skInstance, ChannelTraitsT::skLLChannelID, static_cast<uint32_t>(mode));
        }

        static void set_polarity(const TIMChannelOutputCapturePolarity pol)
        {
            LL_TIM_OC_SetPolarity(
                ControllerTraitsT::skInstance, ChannelTraitsT::skLLChannelID, static_cast<uint32_t>(pol));
        }

        static void set_idle_state(const TIMChannelOutputCaptureIdleState state)
        {
            LL_TIM_OC_SetIdleState(
                ControllerTraitsT::skInstance, ChannelTraitsT::skLLChannelID, static_cast<uint32_t>(state));
        }

        static void enable_output_capture_preload()
        {
            LL_TIM_OC_EnablePreload(ControllerTraitsT::skInstance, ChannelTraitsT::skLLChannelID);
        }

        static void disable_output_capture_preload()
        {
            LL_TIM_OC_DisablePreload(ControllerTraitsT::skInstance, ChannelTraitsT::skLLChannelID);
        }

        static void set_output_capture_preload(const bool enable)
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

        static void enable_output_capture_fast_mode()
        {
            LL_TIM_OC_EnableFast(ControllerTraitsT::skInstance, ChannelTraitsT::skLLChannelID);
        }

        static void disable_output_capture_fast_mode()
        {
            LL_TIM_OC_DisableFast(ControllerTraitsT::skInstance, ChannelTraitsT::skLLChannelID);
        }

        static void set_output_capture_fast_mode(const bool enable)
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

        static void enable_output_capture_clear()
        {
            LL_TIM_OC_EnableClear(ControllerTraitsT::skInstance, ChannelTraitsT::skLLChannelID);
        }

        static void disable_output_capture_clear()
        {
            LL_TIM_OC_DisableClear(ControllerTraitsT::skInstance, ChannelTraitsT::skLLChannelID);
        }

        static void set_output_capture_clear(const bool enable)
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

        static void set_complementary_output_capture_polarity(const TIMChannelOutputCapturePolarity pol)
        {
            LL_TIM_OC_SetPolarity(
                ControllerTraitsT::skInstance, ChannelTraitsT::skLLComplementaryChannelID, static_cast<uint32_t>(pol));
        }

        static void set_complementary_output_capture_idle_state(const TIMChannelOutputCaptureIdleState state)
        {
            LL_TIM_OC_SetIdleState(ControllerTraitsT::skInstance,
                                   ChannelTraitsT::skLLComplementaryChannelID,
                                   static_cast<uint32_t>(state));
        }

#ifdef USE_FULL_LL_DRIVER
        static void config_output_capture(const TIMChannelOutputCaptureMode      mode,
                                          const uint32_t                         compare,
                                          const TIMChannelOutputCapturePolarity  pol,
                                          const TIMChannelOutputCaptureIdleState idle)
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
            LL_TIM_OC_Init(ControllerTraitsT::skInstance, ChannelTraitsT::skLLChannelID, &config);
        }
#endif

        // ---------------------------------------------------------------------
        // INPUT CAPTURE
        // ---------------------------------------------------------------------

        static void set_input_capture_active_input(const TIMChannelInputCaptureActiveInput input)
        {
            LL_TIM_IC_SetActiveInput(
                ControllerTraitsT::skInstance, ChannelTraitsT::skLLChannelID, static_cast<uint32_t>(input));
        }

        static void set_input_capture_prescaler(const TIMChannelInputCapturePrescaler psc)
        {
            LL_TIM_IC_SetPrescaler(
                ControllerTraitsT::skInstance, ChannelTraitsT::skLLChannelID, static_cast<uint32_t>(psc));
        }

        static void set_input_capture_filter(const TIMChannelInputCaptureFilter filter)
        {
            LL_TIM_IC_SetFilter(
                ControllerTraitsT::skInstance, ChannelTraitsT::skLLChannelID, static_cast<uint32_t>(filter));
        }

        static void set_input_capture_polarity(const TIMChannelInputCapturePolarity polarity)
        {
            LL_TIM_IC_SetPolarity(
                ControllerTraitsT::skInstance, ChannelTraitsT::skLLChannelID, static_cast<uint32_t>(polarity));
        }

#ifdef USE_FULL_LL_DRIVER
        static void config_input_capture(const TIMChannelInputCaptureActiveInput input,
                                         const TIMChannelInputCapturePrescaler   psc,
                                         const TIMChannelInputCaptureFilter      filter,
                                         const TIMChannelInputCapturePolarity    polarity)
        {
            LL_TIM_IC_InitTypeDef config;
            LL_TIM_IC_StructInit(&config);
            config.ICActiveInput = static_cast<uint32_t>(input);
            config.ICPrescaler   = static_cast<uint32_t>(psc);
            config.ICFilter      = static_cast<uint32_t>(filter);
            config.ICPolarity    = static_cast<uint32_t>(polarity);
            LL_TIM_IC_Init(ControllerTraitsT::skInstance, ChannelTraitsT::skLLChannelID, &config);
        }
#endif

        static uint32_t get_capture()
        {
            switch (tkChannel)
            {
                case TIMChannelID::kCh1:
                    return LL_TIM_OC_GetCompareCH1(ControllerTraitsT::skInstance);
                case TIMChannelID::kCh2:
                    return LL_TIM_OC_GetCompareCH2(ControllerTraitsT::skInstance);
                case TIMChannelID::kCh3:
                    return LL_TIM_OC_GetCompareCH3(ControllerTraitsT::skInstance);
                case TIMChannelID::kCh4:
                    return LL_TIM_OC_GetCompareCH4(ControllerTraitsT::skInstance);
                case TIMChannelID::kCh5:
                    return LL_TIM_OC_GetCompareCH5(ControllerTraitsT::skInstance);
                case TIMChannelID::kCh6:
                    return LL_TIM_OC_GetCompareCH6(ControllerTraitsT::skInstance);
                default:
                    return 0;
            }
        }
    };
}  // namespace valle::platform