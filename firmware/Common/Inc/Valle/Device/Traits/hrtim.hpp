#pragma once

#include "stm32g4xx_ll_bus.h"
#include "stm32g4xx_ll_hrtim.h"

namespace valle
{

    // ============================================================================
    // CONSTANTS & CHECKS
    // ============================================================================

    using HRTIMControllerID = uint8_t;

    template <HRTIMControllerID tkControllerID>
    constexpr bool kValidHRTIMID = (tkControllerID == 1);

    enum class HRTIMTimerID : uint8_t
    {
        kA = 0,
        kB = 1,
        kC = 2,
        kD = 3,
        kE = 4,
        kF = 5,
    };

    // ============================================================================
    // HARDWARE TRAITS (Map tkHRTIMTimerID -> Registers)
    // ============================================================================
    template <HRTIMControllerID tkControllerID>
        requires(kValidHRTIMID<tkControllerID>)
    struct HRTIMControllerTraits;

    template <>
    struct HRTIMControllerTraits<1>
    {
        static inline HRTIM_TypeDef* const skInstance = HRTIM1;
        static constexpr uint32_t          skClock    = LL_APB2_GRP1_PERIPH_HRTIM1;
    };

    template <HRTIMControllerID tkControllerID, HRTIMTimerID tkTimerID>
    struct HRTIMTimerTraits;

    // HRTIM1 Timer A (tkHRTIMTimerID 0)
    template <>
    struct HRTIMTimerTraits<1, HRTIMTimerID::kA>
    {
        static constexpr uint32_t  skTimerIdx = HRTIM_TIMERINDEX_TIMER_A;
        static constexpr uint32_t  skOutput1  = HRTIM_OUTPUT_TA1;
        static constexpr uint32_t  skOutput2  = HRTIM_OUTPUT_TA2;
        static constexpr IRQn_Type skIRQn     = HRTIM1_TIMA_IRQn;
    };

    // HRTIM1 Timer B
    template <>
    struct HRTIMTimerTraits<1, HRTIMTimerID::kB>
    {
        static constexpr uint32_t  skTimerIdx = HRTIM_TIMERINDEX_TIMER_B;
        static constexpr uint32_t  skOutput1  = HRTIM_OUTPUT_TB1;
        static constexpr uint32_t  skOutput2  = HRTIM_OUTPUT_TB2;
        static constexpr IRQn_Type skIRQn     = HRTIM1_TIMB_IRQn;
    };

    // HRTIM1 Timer C
    template <>
    struct HRTIMTimerTraits<1, HRTIMTimerID::kC>
    {
        static constexpr uint32_t  skTimerIdx = HRTIM_TIMERINDEX_TIMER_C;
        static constexpr uint32_t  skOutput1  = HRTIM_OUTPUT_TC1;
        static constexpr uint32_t  skOutput2  = HRTIM_OUTPUT_TC2;
        static constexpr IRQn_Type skIRQn     = HRTIM1_TIMC_IRQn;
    };

    // HRTIM1 Timer D
    template <>
    struct HRTIMTimerTraits<1, HRTIMTimerID::kD>
    {
        static constexpr uint32_t  skTimerIdx = HRTIM_TIMERINDEX_TIMER_D;
        static constexpr uint32_t  skOutput1  = HRTIM_OUTPUT_TD1;
        static constexpr uint32_t  skOutput2  = HRTIM_OUTPUT_TD2;
        static constexpr IRQn_Type skIRQn     = HRTIM1_TIMD_IRQn;
    };

    // HRTIM1 Timer E
    template <>
    struct HRTIMTimerTraits<1, HRTIMTimerID::kE>
    {
        static constexpr uint32_t  skTimerIdx = HRTIM_TIMERINDEX_TIMER_E;
        static constexpr uint32_t  skOutput1  = HRTIM_OUTPUT_TE1;
        static constexpr uint32_t  skOutput2  = HRTIM_OUTPUT_TE2;
        static constexpr IRQn_Type skIRQn     = HRTIM1_TIME_IRQn;
    };

    // HRTIM1 Timer F
    template <>
    struct HRTIMTimerTraits<1, HRTIMTimerID::kF>
    {
        static constexpr uint32_t  skTimerIdx = HRTIM_TIMERINDEX_TIMER_F;
        static constexpr uint32_t  skOutput1  = HRTIM_OUTPUT_TF1;
        static constexpr uint32_t  skOutput2  = HRTIM_OUTPUT_TF2;
        static constexpr IRQn_Type skIRQn     = HRTIM1_TIMF_IRQn;
    };

    // ============================================================================
    // ENUMERATIONS
    // ============================================================================
    enum class HRTIMOutputPolarity
    {
        kPositive = LL_HRTIM_OUT_POSITIVE_POLARITY,  /// Output active high
        kNegative = LL_HRTIM_OUT_NEGATIVE_POLARITY   /// Output active low
    };

    enum class HRTIMOutputIdleLevel
    {
        kInactive = LL_HRTIM_OUT_IDLELEVEL_INACTIVE,  /// Inactive when idle
        kActive   = LL_HRTIM_OUT_IDLELEVEL_ACTIVE     /// Active when idle
    };

    enum class HRTIMOutputIdleMode
    {
        kNoIdle = LL_HRTIM_OUT_NO_IDLE,          /// Never idle
        kBurst  = LL_HRTIM_OUT_IDLE_WHEN_BURST,  /// Idle during burst mode
    };

    enum class HRTIMOutputFaultState
    {
        kInactive = LL_HRTIM_OUT_FAULTSTATE_INACTIVE,   /// Output inactive on fault
        kActive   = LL_HRTIM_OUT_FAULTSTATE_ACTIVE,     /// Output active on fault
        kNoAction = LL_HRTIM_OUT_FAULTSTATE_NO_ACTION,  /// No action on fault
        kHighZ    = LL_HRTIM_OUT_FAULTSTATE_HIGHZ       /// High impedance on fault
    };

}  // namespace valle