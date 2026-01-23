#pragma once

#include "stm32g4xx_ll_hrtim.h"

// ============================================================================
// CONSTANTS & CHECKS
// ============================================================================

template <uint8_t tkIndex>
constexpr bool kValidHRTIMIndex = (0 <= tkIndex && tkIndex <= 5);

// ============================================================================
// HARDWARE TRAITS (Map tkIndex -> Registers)
// ============================================================================
template <uint8_t tkIndex>
    requires(kValidHRTIMIndex<tkIndex>)
struct HRTIMTraits;

// Timer A (tkIndex 0)
template <>
struct HRTIMTraits<0>
{
    static constexpr uint32_t  skTimerIdx = HRTIM_TIMERINDEX_TIMER_A;
    static constexpr uint32_t  skOutput1  = HRTIM_OUTPUT_TA1;
    static constexpr uint32_t  skOutput2  = HRTIM_OUTPUT_TA2;
    static constexpr IRQn_Type skIRQn     = HRTIM1_TIMA_IRQn;
};

// Timer B (tkIndex 1)
template <>
struct HRTIMTraits<1>
{
    static constexpr uint32_t  skTimerIdx = HRTIM_TIMERINDEX_TIMER_B;
    static constexpr uint32_t  skOutput1  = HRTIM_OUTPUT_TB1;
    static constexpr uint32_t  skOutput2  = HRTIM_OUTPUT_TB2;
    static constexpr IRQn_Type skIRQn     = HRTIM1_TIMB_IRQn;
};

// Timer C (tkIndex 2)
template <>
struct HRTIMTraits<2>
{
    static constexpr uint32_t  skTimerIdx = HRTIM_TIMERINDEX_TIMER_C;
    static constexpr uint32_t  skOutput1  = HRTIM_OUTPUT_TC1;
    static constexpr uint32_t  skOutput2  = HRTIM_OUTPUT_TC2;
    static constexpr IRQn_Type skIRQn     = HRTIM1_TIMC_IRQn;
};

// Timer D (tkIndex 3)
template <>
struct HRTIMTraits<3>
{
    static constexpr uint32_t  skTimerIdx = HRTIM_TIMERINDEX_TIMER_D;
    static constexpr uint32_t  skOutput1  = HRTIM_OUTPUT_TD1;
    static constexpr uint32_t  skOutput2  = HRTIM_OUTPUT_TD2;
    static constexpr IRQn_Type skIRQn     = HRTIM1_TIMD_IRQn;
};

// Timer E (tkIndex 4)
template <>
struct HRTIMTraits<4>
{
    static constexpr uint32_t  skTimerIdx = HRTIM_TIMERINDEX_TIMER_E;
    static constexpr uint32_t  skOutput1  = HRTIM_OUTPUT_TE1;
    static constexpr uint32_t  skOutput2  = HRTIM_OUTPUT_TE2;
    static constexpr IRQn_Type skIRQn     = HRTIM1_TIME_IRQn;
};

// Timer F (tkIndex 5)
template <>
struct HRTIMTraits<5>
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
