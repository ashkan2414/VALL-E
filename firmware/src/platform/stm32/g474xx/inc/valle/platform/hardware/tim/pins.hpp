#pragma once

#include <concepts>
#include <optional>

#include "valle/platform/hardware/gpio.hpp"
#include "valle/platform/hardware/tim/id.hpp"

namespace valle::platform
{
    // ---------------------------------------------------------------------------
    // TIMER PIN TYPES
    // ---------------------------------------------------------------------------
    enum class TIMGPIOPinType : uint8_t
    {
        kCh1,
        kCh2,
        kCh3,
        kCh4,
        kCh1N,
        kCh2N,
        kCh3N,
        kCh4N,
        kETR,
        kBKIN,
        kBKIN2
    };

    // The G474 has high pin density; some timer channels can be mapped to 4+ locations.
    constexpr static uint8_t kTIMMaxOutputIndex = 5;

    template <TIMControllerID tkControllerID, TIMGPIOPinType tkPinType, uint8_t tkOutputIdx = 0>
        requires(tkOutputIdx < kTIMMaxOutputIndex)
    struct TIMPinMap;

#define DECLARE_TIM_PIN_MAP(id, type, idx, port, pin, af)           \
    template <>                                                     \
    struct TIMPinMap<(id), (type), (idx)>                           \
    {                                                               \
        constexpr static GPIOPortID              skPortID = (port); \
        constexpr static GPIOPinID               skPinID  = (pin);  \
        constexpr static GPIOAlternativeFunction skAF     = (af);   \
    };

    // ===========================================================================
    // ADVANCED TIMERS (TIM1, TIM8, TIM20)
    // ===========================================================================

    // --- TIM1 ---
    DECLARE_TIM_PIN_MAP(TIMControllerID::kTim1,
                        TIMGPIOPinType::kCh1,
                        0,
                        GPIOPortID::kPortA,
                        GPIOPinID::kPin8,
                        GPIOAlternativeFunction::kAF6);
    DECLARE_TIM_PIN_MAP(TIMControllerID::kTim1,
                        TIMGPIOPinType::kCh1,
                        1,
                        GPIOPortID::kPortC,
                        GPIOPinID::kPin0,
                        GPIOAlternativeFunction::kAF2);
    DECLARE_TIM_PIN_MAP(TIMControllerID::kTim1,
                        TIMGPIOPinType::kCh1,
                        2,
                        GPIOPortID::kPortE,
                        GPIOPinID::kPin9,
                        GPIOAlternativeFunction::kAF2);
    DECLARE_TIM_PIN_MAP(TIMControllerID::kTim1,
                        TIMGPIOPinType::kCh1N,
                        0,
                        GPIOPortID::kPortA,
                        GPIOPinID::kPin7,
                        GPIOAlternativeFunction::kAF6);
    DECLARE_TIM_PIN_MAP(TIMControllerID::kTim1,
                        TIMGPIOPinType::kCh1N,
                        1,
                        GPIOPortID::kPortA,
                        GPIOPinID::kPin11,
                        GPIOAlternativeFunction::kAF6);
    DECLARE_TIM_PIN_MAP(TIMControllerID::kTim1,
                        TIMGPIOPinType::kCh1N,
                        2,
                        GPIOPortID::kPortB,
                        GPIOPinID::kPin13,
                        GPIOAlternativeFunction::kAF6);
    DECLARE_TIM_PIN_MAP(TIMControllerID::kTim1,
                        TIMGPIOPinType::kCh1N,
                        3,
                        GPIOPortID::kPortC,
                        GPIOPinID::kPin13,
                        GPIOAlternativeFunction::kAF4);
    DECLARE_TIM_PIN_MAP(TIMControllerID::kTim1,
                        TIMGPIOPinType::kCh1N,
                        4,
                        GPIOPortID::kPortE,
                        GPIOPinID::kPin8,
                        GPIOAlternativeFunction::kAF2);
    DECLARE_TIM_PIN_MAP(TIMControllerID::kTim1,
                        TIMGPIOPinType::kCh2,
                        0,
                        GPIOPortID::kPortA,
                        GPIOPinID::kPin9,
                        GPIOAlternativeFunction::kAF6);
    DECLARE_TIM_PIN_MAP(TIMControllerID::kTim1,
                        TIMGPIOPinType::kCh2,
                        1,
                        GPIOPortID::kPortC,
                        GPIOPinID::kPin1,
                        GPIOAlternativeFunction::kAF2);
    DECLARE_TIM_PIN_MAP(TIMControllerID::kTim1,
                        TIMGPIOPinType::kCh2,
                        2,
                        GPIOPortID::kPortE,
                        GPIOPinID::kPin11,
                        GPIOAlternativeFunction::kAF2);
    DECLARE_TIM_PIN_MAP(TIMControllerID::kTim1,
                        TIMGPIOPinType::kCh2N,
                        0,
                        GPIOPortID::kPortB,
                        GPIOPinID::kPin0,
                        GPIOAlternativeFunction::kAF6);
    DECLARE_TIM_PIN_MAP(TIMControllerID::kTim1,
                        TIMGPIOPinType::kCh2N,
                        1,
                        GPIOPortID::kPortB,
                        GPIOPinID::kPin14,
                        GPIOAlternativeFunction::kAF6);
    DECLARE_TIM_PIN_MAP(TIMControllerID::kTim1,
                        TIMGPIOPinType::kCh2N,
                        2,
                        GPIOPortID::kPortE,
                        GPIOPinID::kPin10,
                        GPIOAlternativeFunction::kAF2);
    DECLARE_TIM_PIN_MAP(TIMControllerID::kTim1,
                        TIMGPIOPinType::kCh3,
                        0,
                        GPIOPortID::kPortA,
                        GPIOPinID::kPin10,
                        GPIOAlternativeFunction::kAF6);
    DECLARE_TIM_PIN_MAP(TIMControllerID::kTim1,
                        TIMGPIOPinType::kCh3,
                        1,
                        GPIOPortID::kPortC,
                        GPIOPinID::kPin2,
                        GPIOAlternativeFunction::kAF2);
    DECLARE_TIM_PIN_MAP(TIMControllerID::kTim1,
                        TIMGPIOPinType::kCh3,
                        2,
                        GPIOPortID::kPortE,
                        GPIOPinID::kPin13,
                        GPIOAlternativeFunction::kAF2);
    DECLARE_TIM_PIN_MAP(TIMControllerID::kTim1,
                        TIMGPIOPinType::kCh3N,
                        0,
                        GPIOPortID::kPortB,
                        GPIOPinID::kPin1,
                        GPIOAlternativeFunction::kAF6);
    DECLARE_TIM_PIN_MAP(TIMControllerID::kTim1,
                        TIMGPIOPinType::kCh3N,
                        1,
                        GPIOPortID::kPortB,
                        GPIOPinID::kPin15,
                        GPIOAlternativeFunction::kAF4);
    DECLARE_TIM_PIN_MAP(TIMControllerID::kTim1,
                        TIMGPIOPinType::kCh3N,
                        2,
                        GPIOPortID::kPortE,
                        GPIOPinID::kPin12,
                        GPIOAlternativeFunction::kAF2);
    DECLARE_TIM_PIN_MAP(TIMControllerID::kTim1,
                        TIMGPIOPinType::kCh4,
                        0,
                        GPIOPortID::kPortA,
                        GPIOPinID::kPin11,
                        GPIOAlternativeFunction::kAF11);
    DECLARE_TIM_PIN_MAP(TIMControllerID::kTim1,
                        TIMGPIOPinType::kCh4,
                        1,
                        GPIOPortID::kPortC,
                        GPIOPinID::kPin3,
                        GPIOAlternativeFunction::kAF2);
    DECLARE_TIM_PIN_MAP(TIMControllerID::kTim1,
                        TIMGPIOPinType::kCh4,
                        2,
                        GPIOPortID::kPortE,
                        GPIOPinID::kPin14,
                        GPIOAlternativeFunction::kAF2);
    DECLARE_TIM_PIN_MAP(TIMControllerID::kTim1,
                        TIMGPIOPinType::kETR,
                        0,
                        GPIOPortID::kPortA,
                        GPIOPinID::kPin12,
                        GPIOAlternativeFunction::kAF6);
    DECLARE_TIM_PIN_MAP(TIMControllerID::kTim1,
                        TIMGPIOPinType::kETR,
                        1,
                        GPIOPortID::kPortE,
                        GPIOPinID::kPin7,
                        GPIOAlternativeFunction::kAF2);
    DECLARE_TIM_PIN_MAP(TIMControllerID::kTim1,
                        TIMGPIOPinType::kBKIN,
                        0,
                        GPIOPortID::kPortA,
                        GPIOPinID::kPin6,
                        GPIOAlternativeFunction::kAF6);
    DECLARE_TIM_PIN_MAP(TIMControllerID::kTim1,
                        TIMGPIOPinType::kBKIN,
                        1,
                        GPIOPortID::kPortB,
                        GPIOPinID::kPin12,
                        GPIOAlternativeFunction::kAF6);
    DECLARE_TIM_PIN_MAP(TIMControllerID::kTim1,
                        TIMGPIOPinType::kBKIN,
                        2,
                        GPIOPortID::kPortE,
                        GPIOPinID::kPin15,
                        GPIOAlternativeFunction::kAF2);
    DECLARE_TIM_PIN_MAP(TIMControllerID::kTim1,
                        TIMGPIOPinType::kBKIN2,
                        0,
                        GPIOPortID::kPortA,
                        GPIOPinID::kPin15,
                        GPIOAlternativeFunction::kAF9);
    DECLARE_TIM_PIN_MAP(TIMControllerID::kTim1,
                        TIMGPIOPinType::kBKIN2,
                        1,
                        GPIOPortID::kPortC,
                        GPIOPinID::kPin4,
                        GPIOAlternativeFunction::kAF2);
    DECLARE_TIM_PIN_MAP(TIMControllerID::kTim1,
                        TIMGPIOPinType::kBKIN2,
                        2,
                        GPIOPortID::kPortE,
                        GPIOPinID::kPin15,
                        GPIOAlternativeFunction::kAF4);

    // --- TIM8 ---
    DECLARE_TIM_PIN_MAP(TIMControllerID::kTim8,
                        TIMGPIOPinType::kCh1,
                        0,
                        GPIOPortID::kPortC,
                        GPIOPinID::kPin6,
                        GPIOAlternativeFunction::kAF4);
    DECLARE_TIM_PIN_MAP(TIMControllerID::kTim8,
                        TIMGPIOPinType::kCh1N,
                        0,
                        GPIOPortID::kPortA,
                        GPIOPinID::kPin7,
                        GPIOAlternativeFunction::kAF4);
    DECLARE_TIM_PIN_MAP(TIMControllerID::kTim8,
                        TIMGPIOPinType::kCh1N,
                        1,
                        GPIOPortID::kPortB,
                        GPIOPinID::kPin3,
                        GPIOAlternativeFunction::kAF3);
    DECLARE_TIM_PIN_MAP(TIMControllerID::kTim8,
                        TIMGPIOPinType::kCh1N,
                        2,
                        GPIOPortID::kPortC,
                        GPIOPinID::kPin10,
                        GPIOAlternativeFunction::kAF4);
    DECLARE_TIM_PIN_MAP(TIMControllerID::kTim8,
                        TIMGPIOPinType::kCh2,
                        0,
                        GPIOPortID::kPortC,
                        GPIOPinID::kPin7,
                        GPIOAlternativeFunction::kAF4);
    DECLARE_TIM_PIN_MAP(TIMControllerID::kTim8,
                        TIMGPIOPinType::kCh2,
                        1,
                        GPIOPortID::kPortB,
                        GPIOPinID::kPin3,
                        GPIOAlternativeFunction::kAF4);
    DECLARE_TIM_PIN_MAP(TIMControllerID::kTim8,
                        TIMGPIOPinType::kCh2N,
                        0,
                        GPIOPortID::kPortB,
                        GPIOPinID::kPin0,
                        GPIOAlternativeFunction::kAF4);
    DECLARE_TIM_PIN_MAP(TIMControllerID::kTim8,
                        TIMGPIOPinType::kCh2N,
                        1,
                        GPIOPortID::kPortC,
                        GPIOPinID::kPin11,
                        GPIOAlternativeFunction::kAF4);
    DECLARE_TIM_PIN_MAP(TIMControllerID::kTim8,
                        TIMGPIOPinType::kCh3,
                        0,
                        GPIOPortID::kPortC,
                        GPIOPinID::kPin8,
                        GPIOAlternativeFunction::kAF4);
    DECLARE_TIM_PIN_MAP(TIMControllerID::kTim8,
                        TIMGPIOPinType::kCh3N,
                        0,
                        GPIOPortID::kPortB,
                        GPIOPinID::kPin1,
                        GPIOAlternativeFunction::kAF4);
    DECLARE_TIM_PIN_MAP(TIMControllerID::kTim8,
                        TIMGPIOPinType::kCh3N,
                        1,
                        GPIOPortID::kPortC,
                        GPIOPinID::kPin12,
                        GPIOAlternativeFunction::kAF4);
    DECLARE_TIM_PIN_MAP(TIMControllerID::kTim8,
                        TIMGPIOPinType::kCh4,
                        0,
                        GPIOPortID::kPortC,
                        GPIOPinID::kPin9,
                        GPIOAlternativeFunction::kAF4);
    DECLARE_TIM_PIN_MAP(TIMControllerID::kTim8,
                        TIMGPIOPinType::kETR,
                        0,
                        GPIOPortID::kPortA,
                        GPIOPinID::kPin0,
                        GPIOAlternativeFunction::kAF6);
    DECLARE_TIM_PIN_MAP(TIMControllerID::kTim8,
                        TIMGPIOPinType::kBKIN,
                        0,
                        GPIOPortID::kPortA,
                        GPIOPinID::kPin6,
                        GPIOAlternativeFunction::kAF4);
    DECLARE_TIM_PIN_MAP(TIMControllerID::kTim8,
                        TIMGPIOPinType::kBKIN,
                        1,
                        GPIOPortID::kPortB,
                        GPIOPinID::kPin7,
                        GPIOAlternativeFunction::kAF10);
    DECLARE_TIM_PIN_MAP(TIMControllerID::kTim8,
                        TIMGPIOPinType::kBKIN,
                        2,
                        GPIOPortID::kPortC,
                        GPIOPinID::kPin5,
                        GPIOAlternativeFunction::kAF6);
    DECLARE_TIM_PIN_MAP(TIMControllerID::kTim8,
                        TIMGPIOPinType::kBKIN,
                        3,
                        GPIOPortID::kPortC,
                        GPIOPinID::kPin9,
                        GPIOAlternativeFunction::kAF6);
    DECLARE_TIM_PIN_MAP(TIMControllerID::kTim8,
                        TIMGPIOPinType::kBKIN2,
                        0,
                        GPIOPortID::kPortB,
                        GPIOPinID::kPin6,
                        GPIOAlternativeFunction::kAF10);
    DECLARE_TIM_PIN_MAP(TIMControllerID::kTim8,
                        TIMGPIOPinType::kBKIN2,
                        1,
                        GPIOPortID::kPortC,
                        GPIOPinID::kPin13,
                        GPIOAlternativeFunction::kAF6);

    // --- TIM20 ---
    DECLARE_TIM_PIN_MAP(TIMControllerID::kTim20,
                        TIMGPIOPinType::kCh1,
                        0,
                        GPIOPortID::kPortB,
                        GPIOPinID::kPin2,
                        GPIOAlternativeFunction::kAF3);
    DECLARE_TIM_PIN_MAP(TIMControllerID::kTim20,
                        TIMGPIOPinType::kCh1,
                        1,
                        GPIOPortID::kPortC,
                        GPIOPinID::kPin2,
                        GPIOAlternativeFunction::kAF3);
    DECLARE_TIM_PIN_MAP(TIMControllerID::kTim20,
                        TIMGPIOPinType::kCh1,
                        2,
                        GPIOPortID::kPortF,
                        GPIOPinID::kPin0,
                        GPIOAlternativeFunction::kAF3);
    DECLARE_TIM_PIN_MAP(TIMControllerID::kTim20,
                        TIMGPIOPinType::kCh2,
                        0,
                        GPIOPortID::kPortC,
                        GPIOPinID::kPin3,
                        GPIOAlternativeFunction::kAF3);
    DECLARE_TIM_PIN_MAP(TIMControllerID::kTim20,
                        TIMGPIOPinType::kCh2,
                        1,
                        GPIOPortID::kPortF,
                        GPIOPinID::kPin1,
                        GPIOAlternativeFunction::kAF3);
    DECLARE_TIM_PIN_MAP(TIMControllerID::kTim20,
                        TIMGPIOPinType::kCh3,
                        0,
                        GPIOPortID::kPortC,
                        GPIOPinID::kPin4,
                        GPIOAlternativeFunction::kAF6);
    DECLARE_TIM_PIN_MAP(TIMControllerID::kTim20,
                        TIMGPIOPinType::kCh3,
                        1,
                        GPIOPortID::kPortF,
                        GPIOPinID::kPin2,
                        GPIOAlternativeFunction::kAF3);
    DECLARE_TIM_PIN_MAP(TIMControllerID::kTim20,
                        TIMGPIOPinType::kCh4,
                        0,
                        GPIOPortID::kPortC,
                        GPIOPinID::kPin5,
                        GPIOAlternativeFunction::kAF3);
    DECLARE_TIM_PIN_MAP(TIMControllerID::kTim20,
                        TIMGPIOPinType::kCh4,
                        1,
                        GPIOPortID::kPortG,
                        GPIOPinID::kPin4,
                        GPIOAlternativeFunction::kAF6);
    DECLARE_TIM_PIN_MAP(TIMControllerID::kTim20,
                        TIMGPIOPinType::kBKIN,
                        0,
                        GPIOPortID::kPortB,
                        GPIOPinID::kPin5,
                        GPIOAlternativeFunction::kAF10);
    DECLARE_TIM_PIN_MAP(TIMControllerID::kTim20,
                        TIMGPIOPinType::kBKIN,
                        1,
                        GPIOPortID::kPortC,
                        GPIOPinID::kPin3,
                        GPIOAlternativeFunction::kAF3);
    DECLARE_TIM_PIN_MAP(TIMControllerID::kTim20,
                        TIMGPIOPinType::kBKIN2,
                        0,
                        GPIOPortID::kPortC,
                        GPIOPinID::kPin0,
                        GPIOAlternativeFunction::kAF6);
    DECLARE_TIM_PIN_MAP(TIMControllerID::kTim20,
                        TIMGPIOPinType::kBKIN2,
                        1,
                        GPIOPortID::kPortF,
                        GPIOPinID::kPin3,
                        GPIOAlternativeFunction::kAF3);

    // ===========================================================================
    // GENERAL PURPOSE 32-BIT (TIM2, TIM5) - PREFERRED FOR ENCODERS
    // ===========================================================================

    // --- TIM2 ---
    DECLARE_TIM_PIN_MAP(TIMControllerID::kTim2,
                        TIMGPIOPinType::kCh1,
                        0,
                        GPIOPortID::kPortA,
                        GPIOPinID::kPin0,
                        GPIOAlternativeFunction::kAF1);
    DECLARE_TIM_PIN_MAP(TIMControllerID::kTim2,
                        TIMGPIOPinType::kCh1,
                        1,
                        GPIOPortID::kPortA,
                        GPIOPinID::kPin5,
                        GPIOAlternativeFunction::kAF1);
    DECLARE_TIM_PIN_MAP(TIMControllerID::kTim2,
                        TIMGPIOPinType::kCh1,
                        2,
                        GPIOPortID::kPortA,
                        GPIOPinID::kPin15,
                        GPIOAlternativeFunction::kAF1);
    DECLARE_TIM_PIN_MAP(TIMControllerID::kTim2,
                        TIMGPIOPinType::kCh1,
                        3,
                        GPIOPortID::kPortD,
                        GPIOPinID::kPin3,
                        GPIOAlternativeFunction::kAF2);
    DECLARE_TIM_PIN_MAP(TIMControllerID::kTim2,
                        TIMGPIOPinType::kCh2,
                        0,
                        GPIOPortID::kPortA,
                        GPIOPinID::kPin1,
                        GPIOAlternativeFunction::kAF1);
    DECLARE_TIM_PIN_MAP(TIMControllerID::kTim2,
                        TIMGPIOPinType::kCh2,
                        1,
                        GPIOPortID::kPortB,
                        GPIOPinID::kPin3,
                        GPIOAlternativeFunction::kAF1);
    DECLARE_TIM_PIN_MAP(TIMControllerID::kTim2,
                        TIMGPIOPinType::kCh2,
                        2,
                        GPIOPortID::kPortD,
                        GPIOPinID::kPin4,
                        GPIOAlternativeFunction::kAF2);
    DECLARE_TIM_PIN_MAP(TIMControllerID::kTim2,
                        TIMGPIOPinType::kCh3,
                        0,
                        GPIOPortID::kPortA,
                        GPIOPinID::kPin2,
                        GPIOAlternativeFunction::kAF1);
    DECLARE_TIM_PIN_MAP(TIMControllerID::kTim2,
                        TIMGPIOPinType::kCh3,
                        1,
                        GPIOPortID::kPortB,
                        GPIOPinID::kPin10,
                        GPIOAlternativeFunction::kAF1);
    DECLARE_TIM_PIN_MAP(TIMControllerID::kTim2,
                        TIMGPIOPinType::kCh3,
                        2,
                        GPIOPortID::kPortD,
                        GPIOPinID::kPin7,
                        GPIOAlternativeFunction::kAF2);
    DECLARE_TIM_PIN_MAP(TIMControllerID::kTim2,
                        TIMGPIOPinType::kCh4,
                        0,
                        GPIOPortID::kPortA,
                        GPIOPinID::kPin3,
                        GPIOAlternativeFunction::kAF1);
    DECLARE_TIM_PIN_MAP(TIMControllerID::kTim2,
                        TIMGPIOPinType::kCh4,
                        1,
                        GPIOPortID::kPortB,
                        GPIOPinID::kPin11,
                        GPIOAlternativeFunction::kAF1);
    DECLARE_TIM_PIN_MAP(TIMControllerID::kTim2,
                        TIMGPIOPinType::kCh4,
                        2,
                        GPIOPortID::kPortD,
                        GPIOPinID::kPin6,
                        GPIOAlternativeFunction::kAF2);
    DECLARE_TIM_PIN_MAP(TIMControllerID::kTim2,
                        TIMGPIOPinType::kETR,
                        0,
                        GPIOPortID::kPortA,
                        GPIOPinID::kPin0,
                        GPIOAlternativeFunction::kAF14);
    DECLARE_TIM_PIN_MAP(TIMControllerID::kTim2,
                        TIMGPIOPinType::kETR,
                        1,
                        GPIOPortID::kPortA,
                        GPIOPinID::kPin5,
                        GPIOAlternativeFunction::kAF2);
    DECLARE_TIM_PIN_MAP(TIMControllerID::kTim2,
                        TIMGPIOPinType::kETR,
                        2,
                        GPIOPortID::kPortA,
                        GPIOPinID::kPin15,
                        GPIOAlternativeFunction::kAF2);
    DECLARE_TIM_PIN_MAP(TIMControllerID::kTim2,
                        TIMGPIOPinType::kETR,
                        3,
                        GPIOPortID::kPortD,
                        GPIOPinID::kPin3,
                        GPIOAlternativeFunction::kAF2);

    // --- TIM5 ---
    DECLARE_TIM_PIN_MAP(TIMControllerID::kTim5,
                        TIMGPIOPinType::kCh1,
                        0,
                        GPIOPortID::kPortA,
                        GPIOPinID::kPin0,
                        GPIOAlternativeFunction::kAF2);
    DECLARE_TIM_PIN_MAP(TIMControllerID::kTim5,
                        TIMGPIOPinType::kCh1,
                        1,
                        GPIOPortID::kPortF,
                        GPIOPinID::kPin6,
                        GPIOAlternativeFunction::kAF2);
    DECLARE_TIM_PIN_MAP(TIMControllerID::kTim5,
                        TIMGPIOPinType::kCh2,
                        0,
                        GPIOPortID::kPortA,
                        GPIOPinID::kPin1,
                        GPIOAlternativeFunction::kAF2);
    DECLARE_TIM_PIN_MAP(TIMControllerID::kTim5,
                        TIMGPIOPinType::kCh2,
                        1,
                        GPIOPortID::kPortF,
                        GPIOPinID::kPin7,
                        GPIOAlternativeFunction::kAF2);
    DECLARE_TIM_PIN_MAP(TIMControllerID::kTim5,
                        TIMGPIOPinType::kCh3,
                        0,
                        GPIOPortID::kPortA,
                        GPIOPinID::kPin2,
                        GPIOAlternativeFunction::kAF2);
    DECLARE_TIM_PIN_MAP(TIMControllerID::kTim5,
                        TIMGPIOPinType::kCh3,
                        1,
                        GPIOPortID::kPortF,
                        GPIOPinID::kPin8,
                        GPIOAlternativeFunction::kAF2);
    DECLARE_TIM_PIN_MAP(TIMControllerID::kTim5,
                        TIMGPIOPinType::kCh4,
                        0,
                        GPIOPortID::kPortA,
                        GPIOPinID::kPin3,
                        GPIOAlternativeFunction::kAF2);
    DECLARE_TIM_PIN_MAP(TIMControllerID::kTim5,
                        TIMGPIOPinType::kCh4,
                        1,
                        GPIOPortID::kPortF,
                        GPIOPinID::kPin9,
                        GPIOAlternativeFunction::kAF2);

    // ===========================================================================
    // GENERAL PURPOSE 16-BIT (TIM3, TIM4)
    // ===========================================================================

    // --- TIM3 ---
    DECLARE_TIM_PIN_MAP(TIMControllerID::kTim3,
                        TIMGPIOPinType::kCh1,
                        0,
                        GPIOPortID::kPortA,
                        GPIOPinID::kPin6,
                        GPIOAlternativeFunction::kAF2);
    DECLARE_TIM_PIN_MAP(TIMControllerID::kTim3,
                        TIMGPIOPinType::kCh1,
                        1,
                        GPIOPortID::kPortB,
                        GPIOPinID::kPin4,
                        GPIOAlternativeFunction::kAF2);
    DECLARE_TIM_PIN_MAP(TIMControllerID::kTim3,
                        TIMGPIOPinType::kCh1,
                        2,
                        GPIOPortID::kPortC,
                        GPIOPinID::kPin6,
                        GPIOAlternativeFunction::kAF2);
    DECLARE_TIM_PIN_MAP(TIMControllerID::kTim3,
                        TIMGPIOPinType::kCh1,
                        3,
                        GPIOPortID::kPortE,
                        GPIOPinID::kPin2,
                        GPIOAlternativeFunction::kAF2);
    DECLARE_TIM_PIN_MAP(TIMControllerID::kTim3,
                        TIMGPIOPinType::kCh2,
                        0,
                        GPIOPortID::kPortA,
                        GPIOPinID::kPin7,
                        GPIOAlternativeFunction::kAF2);
    DECLARE_TIM_PIN_MAP(TIMControllerID::kTim3,
                        TIMGPIOPinType::kCh2,
                        1,
                        GPIOPortID::kPortB,
                        GPIOPinID::kPin5,
                        GPIOAlternativeFunction::kAF2);
    DECLARE_TIM_PIN_MAP(TIMControllerID::kTim3,
                        TIMGPIOPinType::kCh2,
                        2,
                        GPIOPortID::kPortC,
                        GPIOPinID::kPin7,
                        GPIOAlternativeFunction::kAF2);
    DECLARE_TIM_PIN_MAP(TIMControllerID::kTim3,
                        TIMGPIOPinType::kCh2,
                        3,
                        GPIOPortID::kPortE,
                        GPIOPinID::kPin3,
                        GPIOAlternativeFunction::kAF2);
    DECLARE_TIM_PIN_MAP(TIMControllerID::kTim3,
                        TIMGPIOPinType::kCh3,
                        0,
                        GPIOPortID::kPortB,
                        GPIOPinID::kPin0,
                        GPIOAlternativeFunction::kAF2);
    DECLARE_TIM_PIN_MAP(TIMControllerID::kTim3,
                        TIMGPIOPinType::kCh3,
                        1,
                        GPIOPortID::kPortC,
                        GPIOPinID::kPin8,
                        GPIOAlternativeFunction::kAF2);
    DECLARE_TIM_PIN_MAP(TIMControllerID::kTim3,
                        TIMGPIOPinType::kCh3,
                        2,
                        GPIOPortID::kPortE,
                        GPIOPinID::kPin4,
                        GPIOAlternativeFunction::kAF2);
    DECLARE_TIM_PIN_MAP(TIMControllerID::kTim3,
                        TIMGPIOPinType::kCh4,
                        0,
                        GPIOPortID::kPortB,
                        GPIOPinID::kPin1,
                        GPIOAlternativeFunction::kAF2);
    DECLARE_TIM_PIN_MAP(TIMControllerID::kTim3,
                        TIMGPIOPinType::kCh4,
                        1,
                        GPIOPortID::kPortC,
                        GPIOPinID::kPin9,
                        GPIOAlternativeFunction::kAF2);
    DECLARE_TIM_PIN_MAP(TIMControllerID::kTim3,
                        TIMGPIOPinType::kCh4,
                        2,
                        GPIOPortID::kPortE,
                        GPIOPinID::kPin5,
                        GPIOAlternativeFunction::kAF2);
    DECLARE_TIM_PIN_MAP(TIMControllerID::kTim3,
                        TIMGPIOPinType::kETR,
                        0,
                        GPIOPortID::kPortD,
                        GPIOPinID::kPin2,
                        GPIOAlternativeFunction::kAF2);

    // --- TIM4 ---
    DECLARE_TIM_PIN_MAP(TIMControllerID::kTim4,
                        TIMGPIOPinType::kCh1,
                        0,
                        GPIOPortID::kPortB,
                        GPIOPinID::kPin6,
                        GPIOAlternativeFunction::kAF2);
    DECLARE_TIM_PIN_MAP(TIMControllerID::kTim4,
                        TIMGPIOPinType::kCh1,
                        1,
                        GPIOPortID::kPortD,
                        GPIOPinID::kPin12,
                        GPIOAlternativeFunction::kAF2);
    DECLARE_TIM_PIN_MAP(TIMControllerID::kTim4,
                        TIMGPIOPinType::kCh1,
                        2,
                        GPIOPortID::kPortF,
                        GPIOPinID::kPin6,
                        GPIOAlternativeFunction::kAF5);
    DECLARE_TIM_PIN_MAP(TIMControllerID::kTim4,
                        TIMGPIOPinType::kCh2,
                        0,
                        GPIOPortID::kPortB,
                        GPIOPinID::kPin7,
                        GPIOAlternativeFunction::kAF2);
    DECLARE_TIM_PIN_MAP(TIMControllerID::kTim4,
                        TIMGPIOPinType::kCh2,
                        1,
                        GPIOPortID::kPortD,
                        GPIOPinID::kPin13,
                        GPIOAlternativeFunction::kAF2);
    DECLARE_TIM_PIN_MAP(TIMControllerID::kTim4,
                        TIMGPIOPinType::kCh2,
                        2,
                        GPIOPortID::kPortF,
                        GPIOPinID::kPin7,
                        GPIOAlternativeFunction::kAF5);
    DECLARE_TIM_PIN_MAP(TIMControllerID::kTim4,
                        TIMGPIOPinType::kCh3,
                        0,
                        GPIOPortID::kPortB,
                        GPIOPinID::kPin8,
                        GPIOAlternativeFunction::kAF2);
    DECLARE_TIM_PIN_MAP(TIMControllerID::kTim4,
                        TIMGPIOPinType::kCh3,
                        1,
                        GPIOPortID::kPortD,
                        GPIOPinID::kPin14,
                        GPIOAlternativeFunction::kAF2);
    DECLARE_TIM_PIN_MAP(TIMControllerID::kTim4,
                        TIMGPIOPinType::kCh3,
                        2,
                        GPIOPortID::kPortF,
                        GPIOPinID::kPin8,
                        GPIOAlternativeFunction::kAF5);
    DECLARE_TIM_PIN_MAP(TIMControllerID::kTim4,
                        TIMGPIOPinType::kCh4,
                        0,
                        GPIOPortID::kPortB,
                        GPIOPinID::kPin9,
                        GPIOAlternativeFunction::kAF2);
    DECLARE_TIM_PIN_MAP(TIMControllerID::kTim4,
                        TIMGPIOPinType::kCh4,
                        1,
                        GPIOPortID::kPortD,
                        GPIOPinID::kPin15,
                        GPIOAlternativeFunction::kAF2);
    DECLARE_TIM_PIN_MAP(TIMControllerID::kTim4,
                        TIMGPIOPinType::kCh4,
                        2,
                        GPIOPortID::kPortF,
                        GPIOPinID::kPin9,
                        GPIOAlternativeFunction::kAF5);
    DECLARE_TIM_PIN_MAP(TIMControllerID::kTim4,
                        TIMGPIOPinType::kETR,
                        0,
                        GPIOPortID::kPortF,
                        GPIOPinID::kPin6,
                        GPIOAlternativeFunction::kAF10);

    // ===========================================================================
    // LITE TIMERS (TIM15, 16, 17)
    // ===========================================================================

    // --- TIM15 ---
    DECLARE_TIM_PIN_MAP(TIMControllerID::kTim15,
                        TIMGPIOPinType::kCh1,
                        0,
                        GPIOPortID::kPortA,
                        GPIOPinID::kPin2,
                        GPIOAlternativeFunction::kAF9);
    DECLARE_TIM_PIN_MAP(TIMControllerID::kTim15,
                        TIMGPIOPinType::kCh1,
                        1,
                        GPIOPortID::kPortB,
                        GPIOPinID::kPin14,
                        GPIOAlternativeFunction::kAF1);
    DECLARE_TIM_PIN_MAP(TIMControllerID::kTim15,
                        TIMGPIOPinType::kCh1,
                        2,
                        GPIOPortID::kPortF,
                        GPIOPinID::kPin9,
                        GPIOAlternativeFunction::kAF3);
    DECLARE_TIM_PIN_MAP(TIMControllerID::kTim15,
                        TIMGPIOPinType::kCh1,
                        3,
                        GPIOPortID::kPortG,
                        GPIOPinID::kPin2,
                        GPIOAlternativeFunction::kAF3);
    DECLARE_TIM_PIN_MAP(TIMControllerID::kTim15,
                        TIMGPIOPinType::kCh1N,
                        0,
                        GPIOPortID::kPortA,
                        GPIOPinID::kPin1,
                        GPIOAlternativeFunction::kAF9);
    DECLARE_TIM_PIN_MAP(TIMControllerID::kTim15,
                        TIMGPIOPinType::kCh1N,
                        1,
                        GPIOPortID::kPortB,
                        GPIOPinID::kPin15,
                        GPIOAlternativeFunction::kAF1);
    DECLARE_TIM_PIN_MAP(TIMControllerID::kTim15,
                        TIMGPIOPinType::kCh2,
                        0,
                        GPIOPortID::kPortA,
                        GPIOPinID::kPin3,
                        GPIOAlternativeFunction::kAF9);
    DECLARE_TIM_PIN_MAP(TIMControllerID::kTim15,
                        TIMGPIOPinType::kCh2,
                        1,
                        GPIOPortID::kPortB,
                        GPIOPinID::kPin15,
                        GPIOAlternativeFunction::kAF1);
    DECLARE_TIM_PIN_MAP(TIMControllerID::kTim15,
                        TIMGPIOPinType::kCh2,
                        2,
                        GPIOPortID::kPortF,
                        GPIOPinID::kPin10,
                        GPIOAlternativeFunction::kAF3);
    DECLARE_TIM_PIN_MAP(TIMControllerID::kTim15,
                        TIMGPIOPinType::kCh2,
                        3,
                        GPIOPortID::kPortG,
                        GPIOPinID::kPin3,
                        GPIOAlternativeFunction::kAF3);
    DECLARE_TIM_PIN_MAP(TIMControllerID::kTim15,
                        TIMGPIOPinType::kBKIN,
                        0,
                        GPIOPortID::kPortA,
                        GPIOPinID::kPin9,
                        GPIOAlternativeFunction::kAF9);
    DECLARE_TIM_PIN_MAP(TIMControllerID::kTim15,
                        TIMGPIOPinType::kBKIN,
                        1,
                        GPIOPortID::kPortC,
                        GPIOPinID::kPin5,
                        GPIOAlternativeFunction::kAF2);

    // --- TIM16 ---
    DECLARE_TIM_PIN_MAP(TIMControllerID::kTim16,
                        TIMGPIOPinType::kCh1,
                        0,
                        GPIOPortID::kPortA,
                        GPIOPinID::kPin6,
                        GPIOAlternativeFunction::kAF1);
    DECLARE_TIM_PIN_MAP(TIMControllerID::kTim16,
                        TIMGPIOPinType::kCh1,
                        1,
                        GPIOPortID::kPortB,
                        GPIOPinID::kPin8,
                        GPIOAlternativeFunction::kAF1);
    DECLARE_TIM_PIN_MAP(TIMControllerID::kTim16,
                        TIMGPIOPinType::kCh1,
                        2,
                        GPIOPortID::kPortD,
                        GPIOPinID::kPin6,
                        GPIOAlternativeFunction::kAF1);
    DECLARE_TIM_PIN_MAP(TIMControllerID::kTim16,
                        TIMGPIOPinType::kCh1N,
                        0,
                        GPIOPortID::kPortB,
                        GPIOPinID::kPin6,
                        GPIOAlternativeFunction::kAF1);
    DECLARE_TIM_PIN_MAP(TIMControllerID::kTim16,
                        TIMGPIOPinType::kBKIN,
                        0,
                        GPIOPortID::kPortB,
                        GPIOPinID::kPin5,
                        GPIOAlternativeFunction::kAF1);

    // --- TIM17 ---
    DECLARE_TIM_PIN_MAP(TIMControllerID::kTim17,
                        TIMGPIOPinType::kCh1,
                        0,
                        GPIOPortID::kPortA,
                        GPIOPinID::kPin7,
                        GPIOAlternativeFunction::kAF1);
    DECLARE_TIM_PIN_MAP(TIMControllerID::kTim17,
                        TIMGPIOPinType::kCh1,
                        1,
                        GPIOPortID::kPortB,
                        GPIOPinID::kPin9,
                        GPIOAlternativeFunction::kAF1);
    DECLARE_TIM_PIN_MAP(TIMControllerID::kTim17,
                        TIMGPIOPinType::kCh1,
                        2,
                        GPIOPortID::kPortD,
                        GPIOPinID::kPin7,
                        GPIOAlternativeFunction::kAF1);
    DECLARE_TIM_PIN_MAP(TIMControllerID::kTim17,
                        TIMGPIOPinType::kCh1N,
                        0,
                        GPIOPortID::kPortB,
                        GPIOPinID::kPin7,
                        GPIOAlternativeFunction::kAF1);
    DECLARE_TIM_PIN_MAP(TIMControllerID::kTim17,
                        TIMGPIOPinType::kBKIN,
                        0,
                        GPIOPortID::kPortA,
                        GPIOPinID::kPin10,
                        GPIOAlternativeFunction::kAF1);

#undef DECLARE_TIM_PIN_MAP

    // ---------------------------------------------------------------------------
    // VALIDATION AND LOOKUP MACHINERY (CONCEPTUAL)
    // ---------------------------------------------------------------------------

    namespace detail
    {
        template <TIMControllerID tkID, TIMGPIOPinType tkPinType, uint8_t tkIdx, GPIOPortID tkPort, GPIOPinID tkPin>
        concept CValidSpecificTIMPin = requires {
            typename TIMPinMap<tkID, tkPinType, tkIdx>;
            TIMPinMap<tkID, tkPinType, tkIdx>::skPortID == tkPort;
            TIMPinMap<tkID, tkPinType, tkIdx>::skPinID == tkPin;
        };

        template <TIMControllerID tkID, TIMGPIOPinType tkPinType, GPIOPortID tkPort, GPIOPinID tkPin, uint8_t... tkIdxs>
        constexpr std::optional<uint8_t> get_timer_pin_index(std::integer_sequence<uint8_t, tkIdxs...>)
        {
            std::optional<uint8_t> result = std::nullopt;
            ((CValidSpecificTIMPin<tkID, tkPinType, tkIdxs, tkPort, tkPin> ? (result = tkIdxs, true) : false) || ...);
            return result;
        }

        template <TIMControllerID tkID, TIMGPIOPinType tkPinType, GPIOPortID tkPort, GPIOPinID tkPin, uint8_t... tkIdxs>
        constexpr bool check_any_valid_timer_pin(std::integer_sequence<uint8_t, tkIdxs...>)
        {
            return get_timer_pin_index<tkID, tkPinType, tkPort, tkPin>(std::integer_sequence<uint8_t, tkIdxs...>{})
                .has_value();
        }
    }  // namespace detail

    template <TIMControllerID tkID, TIMGPIOPinType tkPinType, GPIOPortID tkPort, GPIOPinID tkPin>
    concept CValidTIMPin = detail::check_any_valid_timer_pin<tkID, tkPinType, tkPort, tkPin>(
        std::make_integer_sequence<uint8_t, kTIMMaxOutputIndex>{});

    template <TIMControllerID tkID, TIMGPIOPinType tkPinType, GPIOPortID tkPort, GPIOPinID tkPin>
        requires(CValidTIMPin<tkID, tkPinType, tkPort, tkPin>)
    constexpr uint8_t kTIMPinOutputIndex = detail::get_timer_pin_index<tkID, tkPinType, tkPort, tkPin>(
                                               std::make_integer_sequence<uint8_t, kTIMMaxOutputIndex>{})
                                               .value();

    template <TIMControllerID tkID, TIMGPIOPinType tkPinType, GPIOPortID tkPort, GPIOPinID tkPin>
        requires(CValidTIMPin<tkID, tkPinType, tkPort, tkPin>)
    constexpr GPIOAlternativeFunction kTIMPinAF =
        TIMPinMap<tkID, tkPinType, kTIMPinOutputIndex<tkID, tkPinType, tkPort, tkPin>>::skAF;

}  // namespace valle::platform