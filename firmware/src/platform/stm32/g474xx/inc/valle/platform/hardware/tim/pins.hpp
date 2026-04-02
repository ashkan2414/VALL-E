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
    enum class TimGpioPinType : uint8_t
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
    constexpr static uint8_t kTimMaxOutputIndex = 5;

    template <TimControllerId tkControllerId, TimGpioPinType tkPinType, uint8_t tkOutputIdx = 0>
        requires(tkOutputIdx < kTimMaxOutputIndex)
    struct TimPinMap;

#define DECLARE_TIM_PIN_MAP(id, type, idx, port, pin, af)           \
    template <>                                                     \
    struct TimPinMap<(id), (type), (idx)>                           \
    {                                                               \
        constexpr static GpioPortId              skPortId = (port); \
        constexpr static GpioPinId               skPinId  = (pin);  \
        constexpr static GpioAlternativeFunction skAF     = (af);   \
    };

    // ===========================================================================
    // ADVANCED TIMERS (Tim1, Tim8, Tim20)
    // ===========================================================================

    // --- Tim1 ---
    DECLARE_TIM_PIN_MAP(TimControllerId::kTim1,
                        TimGpioPinType::kCh1,
                        0,
                        GpioPortId::kPortA,
                        GpioPinId::kPin8,
                        GpioAlternativeFunction::kAF6);
    DECLARE_TIM_PIN_MAP(TimControllerId::kTim1,
                        TimGpioPinType::kCh1,
                        1,
                        GpioPortId::kPortC,
                        GpioPinId::kPin0,
                        GpioAlternativeFunction::kAF2);
    DECLARE_TIM_PIN_MAP(TimControllerId::kTim1,
                        TimGpioPinType::kCh1,
                        2,
                        GpioPortId::kPortE,
                        GpioPinId::kPin9,
                        GpioAlternativeFunction::kAF2);
    DECLARE_TIM_PIN_MAP(TimControllerId::kTim1,
                        TimGpioPinType::kCh1N,
                        0,
                        GpioPortId::kPortA,
                        GpioPinId::kPin7,
                        GpioAlternativeFunction::kAF6);
    DECLARE_TIM_PIN_MAP(TimControllerId::kTim1,
                        TimGpioPinType::kCh1N,
                        1,
                        GpioPortId::kPortA,
                        GpioPinId::kPin11,
                        GpioAlternativeFunction::kAF6);
    DECLARE_TIM_PIN_MAP(TimControllerId::kTim1,
                        TimGpioPinType::kCh1N,
                        2,
                        GpioPortId::kPortB,
                        GpioPinId::kPin13,
                        GpioAlternativeFunction::kAF6);
    DECLARE_TIM_PIN_MAP(TimControllerId::kTim1,
                        TimGpioPinType::kCh1N,
                        3,
                        GpioPortId::kPortC,
                        GpioPinId::kPin13,
                        GpioAlternativeFunction::kAF4);
    DECLARE_TIM_PIN_MAP(TimControllerId::kTim1,
                        TimGpioPinType::kCh1N,
                        4,
                        GpioPortId::kPortE,
                        GpioPinId::kPin8,
                        GpioAlternativeFunction::kAF2);
    DECLARE_TIM_PIN_MAP(TimControllerId::kTim1,
                        TimGpioPinType::kCh2,
                        0,
                        GpioPortId::kPortA,
                        GpioPinId::kPin9,
                        GpioAlternativeFunction::kAF6);
    DECLARE_TIM_PIN_MAP(TimControllerId::kTim1,
                        TimGpioPinType::kCh2,
                        1,
                        GpioPortId::kPortC,
                        GpioPinId::kPin1,
                        GpioAlternativeFunction::kAF2);
    DECLARE_TIM_PIN_MAP(TimControllerId::kTim1,
                        TimGpioPinType::kCh2,
                        2,
                        GpioPortId::kPortE,
                        GpioPinId::kPin11,
                        GpioAlternativeFunction::kAF2);
    DECLARE_TIM_PIN_MAP(TimControllerId::kTim1,
                        TimGpioPinType::kCh2N,
                        0,
                        GpioPortId::kPortB,
                        GpioPinId::kPin0,
                        GpioAlternativeFunction::kAF6);
    DECLARE_TIM_PIN_MAP(TimControllerId::kTim1,
                        TimGpioPinType::kCh2N,
                        1,
                        GpioPortId::kPortB,
                        GpioPinId::kPin14,
                        GpioAlternativeFunction::kAF6);
    DECLARE_TIM_PIN_MAP(TimControllerId::kTim1,
                        TimGpioPinType::kCh2N,
                        2,
                        GpioPortId::kPortE,
                        GpioPinId::kPin10,
                        GpioAlternativeFunction::kAF2);
    DECLARE_TIM_PIN_MAP(TimControllerId::kTim1,
                        TimGpioPinType::kCh3,
                        0,
                        GpioPortId::kPortA,
                        GpioPinId::kPin10,
                        GpioAlternativeFunction::kAF6);
    DECLARE_TIM_PIN_MAP(TimControllerId::kTim1,
                        TimGpioPinType::kCh3,
                        1,
                        GpioPortId::kPortC,
                        GpioPinId::kPin2,
                        GpioAlternativeFunction::kAF2);
    DECLARE_TIM_PIN_MAP(TimControllerId::kTim1,
                        TimGpioPinType::kCh3,
                        2,
                        GpioPortId::kPortE,
                        GpioPinId::kPin13,
                        GpioAlternativeFunction::kAF2);
    DECLARE_TIM_PIN_MAP(TimControllerId::kTim1,
                        TimGpioPinType::kCh3N,
                        0,
                        GpioPortId::kPortB,
                        GpioPinId::kPin1,
                        GpioAlternativeFunction::kAF6);
    DECLARE_TIM_PIN_MAP(TimControllerId::kTim1,
                        TimGpioPinType::kCh3N,
                        1,
                        GpioPortId::kPortB,
                        GpioPinId::kPin15,
                        GpioAlternativeFunction::kAF4);
    DECLARE_TIM_PIN_MAP(TimControllerId::kTim1,
                        TimGpioPinType::kCh3N,
                        2,
                        GpioPortId::kPortE,
                        GpioPinId::kPin12,
                        GpioAlternativeFunction::kAF2);
    DECLARE_TIM_PIN_MAP(TimControllerId::kTim1,
                        TimGpioPinType::kCh4,
                        0,
                        GpioPortId::kPortA,
                        GpioPinId::kPin11,
                        GpioAlternativeFunction::kAF11);
    DECLARE_TIM_PIN_MAP(TimControllerId::kTim1,
                        TimGpioPinType::kCh4,
                        1,
                        GpioPortId::kPortC,
                        GpioPinId::kPin3,
                        GpioAlternativeFunction::kAF2);
    DECLARE_TIM_PIN_MAP(TimControllerId::kTim1,
                        TimGpioPinType::kCh4,
                        2,
                        GpioPortId::kPortE,
                        GpioPinId::kPin14,
                        GpioAlternativeFunction::kAF2);
    DECLARE_TIM_PIN_MAP(TimControllerId::kTim1,
                        TimGpioPinType::kETR,
                        0,
                        GpioPortId::kPortA,
                        GpioPinId::kPin12,
                        GpioAlternativeFunction::kAF6);
    DECLARE_TIM_PIN_MAP(TimControllerId::kTim1,
                        TimGpioPinType::kETR,
                        1,
                        GpioPortId::kPortE,
                        GpioPinId::kPin7,
                        GpioAlternativeFunction::kAF2);
    DECLARE_TIM_PIN_MAP(TimControllerId::kTim1,
                        TimGpioPinType::kBKIN,
                        0,
                        GpioPortId::kPortA,
                        GpioPinId::kPin6,
                        GpioAlternativeFunction::kAF6);
    DECLARE_TIM_PIN_MAP(TimControllerId::kTim1,
                        TimGpioPinType::kBKIN,
                        1,
                        GpioPortId::kPortB,
                        GpioPinId::kPin12,
                        GpioAlternativeFunction::kAF6);
    DECLARE_TIM_PIN_MAP(TimControllerId::kTim1,
                        TimGpioPinType::kBKIN,
                        2,
                        GpioPortId::kPortE,
                        GpioPinId::kPin15,
                        GpioAlternativeFunction::kAF2);
    DECLARE_TIM_PIN_MAP(TimControllerId::kTim1,
                        TimGpioPinType::kBKIN2,
                        0,
                        GpioPortId::kPortA,
                        GpioPinId::kPin15,
                        GpioAlternativeFunction::kAF9);
    DECLARE_TIM_PIN_MAP(TimControllerId::kTim1,
                        TimGpioPinType::kBKIN2,
                        1,
                        GpioPortId::kPortC,
                        GpioPinId::kPin4,
                        GpioAlternativeFunction::kAF2);
    DECLARE_TIM_PIN_MAP(TimControllerId::kTim1,
                        TimGpioPinType::kBKIN2,
                        2,
                        GpioPortId::kPortE,
                        GpioPinId::kPin15,
                        GpioAlternativeFunction::kAF4);

    // --- Tim8 ---
    DECLARE_TIM_PIN_MAP(TimControllerId::kTim8,
                        TimGpioPinType::kCh1,
                        0,
                        GpioPortId::kPortC,
                        GpioPinId::kPin6,
                        GpioAlternativeFunction::kAF4);
    DECLARE_TIM_PIN_MAP(TimControllerId::kTim8,
                        TimGpioPinType::kCh1N,
                        0,
                        GpioPortId::kPortA,
                        GpioPinId::kPin7,
                        GpioAlternativeFunction::kAF4);
    DECLARE_TIM_PIN_MAP(TimControllerId::kTim8,
                        TimGpioPinType::kCh1N,
                        1,
                        GpioPortId::kPortB,
                        GpioPinId::kPin3,
                        GpioAlternativeFunction::kAF3);
    DECLARE_TIM_PIN_MAP(TimControllerId::kTim8,
                        TimGpioPinType::kCh1N,
                        2,
                        GpioPortId::kPortC,
                        GpioPinId::kPin10,
                        GpioAlternativeFunction::kAF4);
    DECLARE_TIM_PIN_MAP(TimControllerId::kTim8,
                        TimGpioPinType::kCh2,
                        0,
                        GpioPortId::kPortC,
                        GpioPinId::kPin7,
                        GpioAlternativeFunction::kAF4);
    DECLARE_TIM_PIN_MAP(TimControllerId::kTim8,
                        TimGpioPinType::kCh2,
                        1,
                        GpioPortId::kPortB,
                        GpioPinId::kPin3,
                        GpioAlternativeFunction::kAF4);
    DECLARE_TIM_PIN_MAP(TimControllerId::kTim8,
                        TimGpioPinType::kCh2N,
                        0,
                        GpioPortId::kPortB,
                        GpioPinId::kPin0,
                        GpioAlternativeFunction::kAF4);
    DECLARE_TIM_PIN_MAP(TimControllerId::kTim8,
                        TimGpioPinType::kCh2N,
                        1,
                        GpioPortId::kPortC,
                        GpioPinId::kPin11,
                        GpioAlternativeFunction::kAF4);
    DECLARE_TIM_PIN_MAP(TimControllerId::kTim8,
                        TimGpioPinType::kCh3,
                        0,
                        GpioPortId::kPortC,
                        GpioPinId::kPin8,
                        GpioAlternativeFunction::kAF4);
    DECLARE_TIM_PIN_MAP(TimControllerId::kTim8,
                        TimGpioPinType::kCh3N,
                        0,
                        GpioPortId::kPortB,
                        GpioPinId::kPin1,
                        GpioAlternativeFunction::kAF4);
    DECLARE_TIM_PIN_MAP(TimControllerId::kTim8,
                        TimGpioPinType::kCh3N,
                        1,
                        GpioPortId::kPortC,
                        GpioPinId::kPin12,
                        GpioAlternativeFunction::kAF4);
    DECLARE_TIM_PIN_MAP(TimControllerId::kTim8,
                        TimGpioPinType::kCh4,
                        0,
                        GpioPortId::kPortC,
                        GpioPinId::kPin9,
                        GpioAlternativeFunction::kAF4);
    DECLARE_TIM_PIN_MAP(TimControllerId::kTim8,
                        TimGpioPinType::kETR,
                        0,
                        GpioPortId::kPortA,
                        GpioPinId::kPin0,
                        GpioAlternativeFunction::kAF6);
    DECLARE_TIM_PIN_MAP(TimControllerId::kTim8,
                        TimGpioPinType::kBKIN,
                        0,
                        GpioPortId::kPortA,
                        GpioPinId::kPin6,
                        GpioAlternativeFunction::kAF4);
    DECLARE_TIM_PIN_MAP(TimControllerId::kTim8,
                        TimGpioPinType::kBKIN,
                        1,
                        GpioPortId::kPortB,
                        GpioPinId::kPin7,
                        GpioAlternativeFunction::kAF10);
    DECLARE_TIM_PIN_MAP(TimControllerId::kTim8,
                        TimGpioPinType::kBKIN,
                        2,
                        GpioPortId::kPortC,
                        GpioPinId::kPin5,
                        GpioAlternativeFunction::kAF6);
    DECLARE_TIM_PIN_MAP(TimControllerId::kTim8,
                        TimGpioPinType::kBKIN,
                        3,
                        GpioPortId::kPortC,
                        GpioPinId::kPin9,
                        GpioAlternativeFunction::kAF6);
    DECLARE_TIM_PIN_MAP(TimControllerId::kTim8,
                        TimGpioPinType::kBKIN2,
                        0,
                        GpioPortId::kPortB,
                        GpioPinId::kPin6,
                        GpioAlternativeFunction::kAF10);
    DECLARE_TIM_PIN_MAP(TimControllerId::kTim8,
                        TimGpioPinType::kBKIN2,
                        1,
                        GpioPortId::kPortC,
                        GpioPinId::kPin13,
                        GpioAlternativeFunction::kAF6);

    // --- Tim20 ---
    DECLARE_TIM_PIN_MAP(TimControllerId::kTim20,
                        TimGpioPinType::kCh1,
                        0,
                        GpioPortId::kPortB,
                        GpioPinId::kPin2,
                        GpioAlternativeFunction::kAF3);
    DECLARE_TIM_PIN_MAP(TimControllerId::kTim20,
                        TimGpioPinType::kCh1,
                        1,
                        GpioPortId::kPortC,
                        GpioPinId::kPin2,
                        GpioAlternativeFunction::kAF3);
    DECLARE_TIM_PIN_MAP(TimControllerId::kTim20,
                        TimGpioPinType::kCh1,
                        2,
                        GpioPortId::kPortF,
                        GpioPinId::kPin0,
                        GpioAlternativeFunction::kAF3);
    DECLARE_TIM_PIN_MAP(TimControllerId::kTim20,
                        TimGpioPinType::kCh2,
                        0,
                        GpioPortId::kPortC,
                        GpioPinId::kPin3,
                        GpioAlternativeFunction::kAF3);
    DECLARE_TIM_PIN_MAP(TimControllerId::kTim20,
                        TimGpioPinType::kCh2,
                        1,
                        GpioPortId::kPortF,
                        GpioPinId::kPin1,
                        GpioAlternativeFunction::kAF3);
    DECLARE_TIM_PIN_MAP(TimControllerId::kTim20,
                        TimGpioPinType::kCh3,
                        0,
                        GpioPortId::kPortC,
                        GpioPinId::kPin4,
                        GpioAlternativeFunction::kAF6);
    DECLARE_TIM_PIN_MAP(TimControllerId::kTim20,
                        TimGpioPinType::kCh3,
                        1,
                        GpioPortId::kPortF,
                        GpioPinId::kPin2,
                        GpioAlternativeFunction::kAF3);
    DECLARE_TIM_PIN_MAP(TimControllerId::kTim20,
                        TimGpioPinType::kCh4,
                        0,
                        GpioPortId::kPortC,
                        GpioPinId::kPin5,
                        GpioAlternativeFunction::kAF3);
    DECLARE_TIM_PIN_MAP(TimControllerId::kTim20,
                        TimGpioPinType::kCh4,
                        1,
                        GpioPortId::kPortG,
                        GpioPinId::kPin4,
                        GpioAlternativeFunction::kAF6);
    DECLARE_TIM_PIN_MAP(TimControllerId::kTim20,
                        TimGpioPinType::kBKIN,
                        0,
                        GpioPortId::kPortB,
                        GpioPinId::kPin5,
                        GpioAlternativeFunction::kAF10);
    DECLARE_TIM_PIN_MAP(TimControllerId::kTim20,
                        TimGpioPinType::kBKIN,
                        1,
                        GpioPortId::kPortC,
                        GpioPinId::kPin3,
                        GpioAlternativeFunction::kAF3);
    DECLARE_TIM_PIN_MAP(TimControllerId::kTim20,
                        TimGpioPinType::kBKIN2,
                        0,
                        GpioPortId::kPortC,
                        GpioPinId::kPin0,
                        GpioAlternativeFunction::kAF6);
    DECLARE_TIM_PIN_MAP(TimControllerId::kTim20,
                        TimGpioPinType::kBKIN2,
                        1,
                        GpioPortId::kPortF,
                        GpioPinId::kPin3,
                        GpioAlternativeFunction::kAF3);

    // ===========================================================================
    // GENERAL PURPOSE 32-BIT (Tim2, Tim5) - PREFERRED FOR ENCODERS
    // ===========================================================================

    // --- Tim2 ---
    DECLARE_TIM_PIN_MAP(TimControllerId::kTim2,
                        TimGpioPinType::kCh1,
                        0,
                        GpioPortId::kPortA,
                        GpioPinId::kPin0,
                        GpioAlternativeFunction::kAF1);
    DECLARE_TIM_PIN_MAP(TimControllerId::kTim2,
                        TimGpioPinType::kCh1,
                        1,
                        GpioPortId::kPortA,
                        GpioPinId::kPin5,
                        GpioAlternativeFunction::kAF1);
    DECLARE_TIM_PIN_MAP(TimControllerId::kTim2,
                        TimGpioPinType::kCh1,
                        2,
                        GpioPortId::kPortA,
                        GpioPinId::kPin15,
                        GpioAlternativeFunction::kAF1);
    DECLARE_TIM_PIN_MAP(TimControllerId::kTim2,
                        TimGpioPinType::kCh1,
                        3,
                        GpioPortId::kPortD,
                        GpioPinId::kPin3,
                        GpioAlternativeFunction::kAF2);
    DECLARE_TIM_PIN_MAP(TimControllerId::kTim2,
                        TimGpioPinType::kCh2,
                        0,
                        GpioPortId::kPortA,
                        GpioPinId::kPin1,
                        GpioAlternativeFunction::kAF1);
    DECLARE_TIM_PIN_MAP(TimControllerId::kTim2,
                        TimGpioPinType::kCh2,
                        1,
                        GpioPortId::kPortB,
                        GpioPinId::kPin3,
                        GpioAlternativeFunction::kAF1);
    DECLARE_TIM_PIN_MAP(TimControllerId::kTim2,
                        TimGpioPinType::kCh2,
                        2,
                        GpioPortId::kPortD,
                        GpioPinId::kPin4,
                        GpioAlternativeFunction::kAF2);
    DECLARE_TIM_PIN_MAP(TimControllerId::kTim2,
                        TimGpioPinType::kCh3,
                        0,
                        GpioPortId::kPortA,
                        GpioPinId::kPin2,
                        GpioAlternativeFunction::kAF1);
    DECLARE_TIM_PIN_MAP(TimControllerId::kTim2,
                        TimGpioPinType::kCh3,
                        1,
                        GpioPortId::kPortB,
                        GpioPinId::kPin10,
                        GpioAlternativeFunction::kAF1);
    DECLARE_TIM_PIN_MAP(TimControllerId::kTim2,
                        TimGpioPinType::kCh3,
                        2,
                        GpioPortId::kPortD,
                        GpioPinId::kPin7,
                        GpioAlternativeFunction::kAF2);
    DECLARE_TIM_PIN_MAP(TimControllerId::kTim2,
                        TimGpioPinType::kCh4,
                        0,
                        GpioPortId::kPortA,
                        GpioPinId::kPin3,
                        GpioAlternativeFunction::kAF1);
    DECLARE_TIM_PIN_MAP(TimControllerId::kTim2,
                        TimGpioPinType::kCh4,
                        1,
                        GpioPortId::kPortB,
                        GpioPinId::kPin11,
                        GpioAlternativeFunction::kAF1);
    DECLARE_TIM_PIN_MAP(TimControllerId::kTim2,
                        TimGpioPinType::kCh4,
                        2,
                        GpioPortId::kPortD,
                        GpioPinId::kPin6,
                        GpioAlternativeFunction::kAF2);
    DECLARE_TIM_PIN_MAP(TimControllerId::kTim2,
                        TimGpioPinType::kETR,
                        0,
                        GpioPortId::kPortA,
                        GpioPinId::kPin0,
                        GpioAlternativeFunction::kAF14);
    DECLARE_TIM_PIN_MAP(TimControllerId::kTim2,
                        TimGpioPinType::kETR,
                        1,
                        GpioPortId::kPortA,
                        GpioPinId::kPin5,
                        GpioAlternativeFunction::kAF2);
    DECLARE_TIM_PIN_MAP(TimControllerId::kTim2,
                        TimGpioPinType::kETR,
                        2,
                        GpioPortId::kPortA,
                        GpioPinId::kPin15,
                        GpioAlternativeFunction::kAF2);
    DECLARE_TIM_PIN_MAP(TimControllerId::kTim2,
                        TimGpioPinType::kETR,
                        3,
                        GpioPortId::kPortD,
                        GpioPinId::kPin3,
                        GpioAlternativeFunction::kAF2);

    // --- Tim5 ---
    DECLARE_TIM_PIN_MAP(TimControllerId::kTim5,
                        TimGpioPinType::kCh1,
                        0,
                        GpioPortId::kPortA,
                        GpioPinId::kPin0,
                        GpioAlternativeFunction::kAF2);
    DECLARE_TIM_PIN_MAP(TimControllerId::kTim5,
                        TimGpioPinType::kCh1,
                        1,
                        GpioPortId::kPortF,
                        GpioPinId::kPin6,
                        GpioAlternativeFunction::kAF2);
    DECLARE_TIM_PIN_MAP(TimControllerId::kTim5,
                        TimGpioPinType::kCh2,
                        0,
                        GpioPortId::kPortA,
                        GpioPinId::kPin1,
                        GpioAlternativeFunction::kAF2);
    DECLARE_TIM_PIN_MAP(TimControllerId::kTim5,
                        TimGpioPinType::kCh2,
                        1,
                        GpioPortId::kPortF,
                        GpioPinId::kPin7,
                        GpioAlternativeFunction::kAF2);
    DECLARE_TIM_PIN_MAP(TimControllerId::kTim5,
                        TimGpioPinType::kCh3,
                        0,
                        GpioPortId::kPortA,
                        GpioPinId::kPin2,
                        GpioAlternativeFunction::kAF2);
    DECLARE_TIM_PIN_MAP(TimControllerId::kTim5,
                        TimGpioPinType::kCh3,
                        1,
                        GpioPortId::kPortF,
                        GpioPinId::kPin8,
                        GpioAlternativeFunction::kAF2);
    DECLARE_TIM_PIN_MAP(TimControllerId::kTim5,
                        TimGpioPinType::kCh4,
                        0,
                        GpioPortId::kPortA,
                        GpioPinId::kPin3,
                        GpioAlternativeFunction::kAF2);
    DECLARE_TIM_PIN_MAP(TimControllerId::kTim5,
                        TimGpioPinType::kCh4,
                        1,
                        GpioPortId::kPortF,
                        GpioPinId::kPin9,
                        GpioAlternativeFunction::kAF2);

    // ===========================================================================
    // GENERAL PURPOSE 16-BIT (Tim3, Tim4)
    // ===========================================================================

    // --- Tim3 ---
    DECLARE_TIM_PIN_MAP(TimControllerId::kTim3,
                        TimGpioPinType::kCh1,
                        0,
                        GpioPortId::kPortA,
                        GpioPinId::kPin6,
                        GpioAlternativeFunction::kAF2);
    DECLARE_TIM_PIN_MAP(TimControllerId::kTim3,
                        TimGpioPinType::kCh1,
                        1,
                        GpioPortId::kPortB,
                        GpioPinId::kPin4,
                        GpioAlternativeFunction::kAF2);
    DECLARE_TIM_PIN_MAP(TimControllerId::kTim3,
                        TimGpioPinType::kCh1,
                        2,
                        GpioPortId::kPortC,
                        GpioPinId::kPin6,
                        GpioAlternativeFunction::kAF2);
    DECLARE_TIM_PIN_MAP(TimControllerId::kTim3,
                        TimGpioPinType::kCh1,
                        3,
                        GpioPortId::kPortE,
                        GpioPinId::kPin2,
                        GpioAlternativeFunction::kAF2);
    DECLARE_TIM_PIN_MAP(TimControllerId::kTim3,
                        TimGpioPinType::kCh2,
                        0,
                        GpioPortId::kPortA,
                        GpioPinId::kPin7,
                        GpioAlternativeFunction::kAF2);
    DECLARE_TIM_PIN_MAP(TimControllerId::kTim3,
                        TimGpioPinType::kCh2,
                        1,
                        GpioPortId::kPortB,
                        GpioPinId::kPin5,
                        GpioAlternativeFunction::kAF2);
    DECLARE_TIM_PIN_MAP(TimControllerId::kTim3,
                        TimGpioPinType::kCh2,
                        2,
                        GpioPortId::kPortC,
                        GpioPinId::kPin7,
                        GpioAlternativeFunction::kAF2);
    DECLARE_TIM_PIN_MAP(TimControllerId::kTim3,
                        TimGpioPinType::kCh2,
                        3,
                        GpioPortId::kPortE,
                        GpioPinId::kPin3,
                        GpioAlternativeFunction::kAF2);
    DECLARE_TIM_PIN_MAP(TimControllerId::kTim3,
                        TimGpioPinType::kCh3,
                        0,
                        GpioPortId::kPortB,
                        GpioPinId::kPin0,
                        GpioAlternativeFunction::kAF2);
    DECLARE_TIM_PIN_MAP(TimControllerId::kTim3,
                        TimGpioPinType::kCh3,
                        1,
                        GpioPortId::kPortC,
                        GpioPinId::kPin8,
                        GpioAlternativeFunction::kAF2);
    DECLARE_TIM_PIN_MAP(TimControllerId::kTim3,
                        TimGpioPinType::kCh3,
                        2,
                        GpioPortId::kPortE,
                        GpioPinId::kPin4,
                        GpioAlternativeFunction::kAF2);
    DECLARE_TIM_PIN_MAP(TimControllerId::kTim3,
                        TimGpioPinType::kCh4,
                        0,
                        GpioPortId::kPortB,
                        GpioPinId::kPin1,
                        GpioAlternativeFunction::kAF2);
    DECLARE_TIM_PIN_MAP(TimControllerId::kTim3,
                        TimGpioPinType::kCh4,
                        1,
                        GpioPortId::kPortC,
                        GpioPinId::kPin9,
                        GpioAlternativeFunction::kAF2);
    DECLARE_TIM_PIN_MAP(TimControllerId::kTim3,
                        TimGpioPinType::kCh4,
                        2,
                        GpioPortId::kPortE,
                        GpioPinId::kPin5,
                        GpioAlternativeFunction::kAF2);
    DECLARE_TIM_PIN_MAP(TimControllerId::kTim3,
                        TimGpioPinType::kETR,
                        0,
                        GpioPortId::kPortD,
                        GpioPinId::kPin2,
                        GpioAlternativeFunction::kAF2);

    // --- Tim4 ---
    DECLARE_TIM_PIN_MAP(TimControllerId::kTim4,
                        TimGpioPinType::kCh1,
                        0,
                        GpioPortId::kPortB,
                        GpioPinId::kPin6,
                        GpioAlternativeFunction::kAF2);
    DECLARE_TIM_PIN_MAP(TimControllerId::kTim4,
                        TimGpioPinType::kCh1,
                        1,
                        GpioPortId::kPortD,
                        GpioPinId::kPin12,
                        GpioAlternativeFunction::kAF2);
    DECLARE_TIM_PIN_MAP(TimControllerId::kTim4,
                        TimGpioPinType::kCh1,
                        2,
                        GpioPortId::kPortF,
                        GpioPinId::kPin6,
                        GpioAlternativeFunction::kAF5);
    DECLARE_TIM_PIN_MAP(TimControllerId::kTim4,
                        TimGpioPinType::kCh2,
                        0,
                        GpioPortId::kPortB,
                        GpioPinId::kPin7,
                        GpioAlternativeFunction::kAF2);
    DECLARE_TIM_PIN_MAP(TimControllerId::kTim4,
                        TimGpioPinType::kCh2,
                        1,
                        GpioPortId::kPortD,
                        GpioPinId::kPin13,
                        GpioAlternativeFunction::kAF2);
    DECLARE_TIM_PIN_MAP(TimControllerId::kTim4,
                        TimGpioPinType::kCh2,
                        2,
                        GpioPortId::kPortF,
                        GpioPinId::kPin7,
                        GpioAlternativeFunction::kAF5);
    DECLARE_TIM_PIN_MAP(TimControllerId::kTim4,
                        TimGpioPinType::kCh3,
                        0,
                        GpioPortId::kPortB,
                        GpioPinId::kPin8,
                        GpioAlternativeFunction::kAF2);
    DECLARE_TIM_PIN_MAP(TimControllerId::kTim4,
                        TimGpioPinType::kCh3,
                        1,
                        GpioPortId::kPortD,
                        GpioPinId::kPin14,
                        GpioAlternativeFunction::kAF2);
    DECLARE_TIM_PIN_MAP(TimControllerId::kTim4,
                        TimGpioPinType::kCh3,
                        2,
                        GpioPortId::kPortF,
                        GpioPinId::kPin8,
                        GpioAlternativeFunction::kAF5);
    DECLARE_TIM_PIN_MAP(TimControllerId::kTim4,
                        TimGpioPinType::kCh4,
                        0,
                        GpioPortId::kPortB,
                        GpioPinId::kPin9,
                        GpioAlternativeFunction::kAF2);
    DECLARE_TIM_PIN_MAP(TimControllerId::kTim4,
                        TimGpioPinType::kCh4,
                        1,
                        GpioPortId::kPortD,
                        GpioPinId::kPin15,
                        GpioAlternativeFunction::kAF2);
    DECLARE_TIM_PIN_MAP(TimControllerId::kTim4,
                        TimGpioPinType::kCh4,
                        2,
                        GpioPortId::kPortF,
                        GpioPinId::kPin9,
                        GpioAlternativeFunction::kAF5);
    DECLARE_TIM_PIN_MAP(TimControllerId::kTim4,
                        TimGpioPinType::kETR,
                        0,
                        GpioPortId::kPortF,
                        GpioPinId::kPin6,
                        GpioAlternativeFunction::kAF10);

    // ===========================================================================
    // LITE TIMERS (Tim15, 16, 17)
    // ===========================================================================

    // --- Tim15 ---
    DECLARE_TIM_PIN_MAP(TimControllerId::kTim15,
                        TimGpioPinType::kCh1,
                        0,
                        GpioPortId::kPortA,
                        GpioPinId::kPin2,
                        GpioAlternativeFunction::kAF9);
    DECLARE_TIM_PIN_MAP(TimControllerId::kTim15,
                        TimGpioPinType::kCh1,
                        1,
                        GpioPortId::kPortB,
                        GpioPinId::kPin14,
                        GpioAlternativeFunction::kAF1);
    DECLARE_TIM_PIN_MAP(TimControllerId::kTim15,
                        TimGpioPinType::kCh1,
                        2,
                        GpioPortId::kPortF,
                        GpioPinId::kPin9,
                        GpioAlternativeFunction::kAF3);
    DECLARE_TIM_PIN_MAP(TimControllerId::kTim15,
                        TimGpioPinType::kCh1,
                        3,
                        GpioPortId::kPortG,
                        GpioPinId::kPin2,
                        GpioAlternativeFunction::kAF3);
    DECLARE_TIM_PIN_MAP(TimControllerId::kTim15,
                        TimGpioPinType::kCh1N,
                        0,
                        GpioPortId::kPortA,
                        GpioPinId::kPin1,
                        GpioAlternativeFunction::kAF9);
    DECLARE_TIM_PIN_MAP(TimControllerId::kTim15,
                        TimGpioPinType::kCh1N,
                        1,
                        GpioPortId::kPortB,
                        GpioPinId::kPin15,
                        GpioAlternativeFunction::kAF1);
    DECLARE_TIM_PIN_MAP(TimControllerId::kTim15,
                        TimGpioPinType::kCh2,
                        0,
                        GpioPortId::kPortA,
                        GpioPinId::kPin3,
                        GpioAlternativeFunction::kAF9);
    DECLARE_TIM_PIN_MAP(TimControllerId::kTim15,
                        TimGpioPinType::kCh2,
                        1,
                        GpioPortId::kPortB,
                        GpioPinId::kPin15,
                        GpioAlternativeFunction::kAF1);
    DECLARE_TIM_PIN_MAP(TimControllerId::kTim15,
                        TimGpioPinType::kCh2,
                        2,
                        GpioPortId::kPortF,
                        GpioPinId::kPin10,
                        GpioAlternativeFunction::kAF3);
    DECLARE_TIM_PIN_MAP(TimControllerId::kTim15,
                        TimGpioPinType::kCh2,
                        3,
                        GpioPortId::kPortG,
                        GpioPinId::kPin3,
                        GpioAlternativeFunction::kAF3);
    DECLARE_TIM_PIN_MAP(TimControllerId::kTim15,
                        TimGpioPinType::kBKIN,
                        0,
                        GpioPortId::kPortA,
                        GpioPinId::kPin9,
                        GpioAlternativeFunction::kAF9);
    DECLARE_TIM_PIN_MAP(TimControllerId::kTim15,
                        TimGpioPinType::kBKIN,
                        1,
                        GpioPortId::kPortC,
                        GpioPinId::kPin5,
                        GpioAlternativeFunction::kAF2);

    // --- Tim16 ---
    DECLARE_TIM_PIN_MAP(TimControllerId::kTim16,
                        TimGpioPinType::kCh1,
                        0,
                        GpioPortId::kPortA,
                        GpioPinId::kPin6,
                        GpioAlternativeFunction::kAF1);
    DECLARE_TIM_PIN_MAP(TimControllerId::kTim16,
                        TimGpioPinType::kCh1,
                        1,
                        GpioPortId::kPortB,
                        GpioPinId::kPin8,
                        GpioAlternativeFunction::kAF1);
    DECLARE_TIM_PIN_MAP(TimControllerId::kTim16,
                        TimGpioPinType::kCh1,
                        2,
                        GpioPortId::kPortD,
                        GpioPinId::kPin6,
                        GpioAlternativeFunction::kAF1);
    DECLARE_TIM_PIN_MAP(TimControllerId::kTim16,
                        TimGpioPinType::kCh1N,
                        0,
                        GpioPortId::kPortB,
                        GpioPinId::kPin6,
                        GpioAlternativeFunction::kAF1);
    DECLARE_TIM_PIN_MAP(TimControllerId::kTim16,
                        TimGpioPinType::kBKIN,
                        0,
                        GpioPortId::kPortB,
                        GpioPinId::kPin5,
                        GpioAlternativeFunction::kAF1);

    // --- Tim17 ---
    DECLARE_TIM_PIN_MAP(TimControllerId::kTim17,
                        TimGpioPinType::kCh1,
                        0,
                        GpioPortId::kPortA,
                        GpioPinId::kPin7,
                        GpioAlternativeFunction::kAF1);
    DECLARE_TIM_PIN_MAP(TimControllerId::kTim17,
                        TimGpioPinType::kCh1,
                        1,
                        GpioPortId::kPortB,
                        GpioPinId::kPin9,
                        GpioAlternativeFunction::kAF1);
    DECLARE_TIM_PIN_MAP(TimControllerId::kTim17,
                        TimGpioPinType::kCh1,
                        2,
                        GpioPortId::kPortD,
                        GpioPinId::kPin7,
                        GpioAlternativeFunction::kAF1);
    DECLARE_TIM_PIN_MAP(TimControllerId::kTim17,
                        TimGpioPinType::kCh1N,
                        0,
                        GpioPortId::kPortB,
                        GpioPinId::kPin7,
                        GpioAlternativeFunction::kAF1);
    DECLARE_TIM_PIN_MAP(TimControllerId::kTim17,
                        TimGpioPinType::kBKIN,
                        0,
                        GpioPortId::kPortA,
                        GpioPinId::kPin10,
                        GpioAlternativeFunction::kAF1);

#undef DECLARE_TIM_PIN_MAP

    // ---------------------------------------------------------------------------
    // VALIdATION AND LOOKUP MACHINERY (CONCEPTUAL)
    // ---------------------------------------------------------------------------

    namespace detail
    {
        template <TimControllerId tkControllerId,
                  TimGpioPinType  tkPinType,
                  uint8_t         tkIdx,
                  GpioPortId      tkPort,
                  GpioPinId       tkPin>
        concept CValidSpecificTimPin = requires {
            typename TimPinMap<tkControllerId, tkPinType, tkIdx>;
            TimPinMap<tkControllerId, tkPinType, tkIdx>::skPortId == tkPort;
            TimPinMap<tkControllerId, tkPinType, tkIdx>::skPinId == tkPin;
        };

        template <TimControllerId tkControllerId,
                  TimGpioPinType  tkPinType,
                  GpioPortId      tkPort,
                  GpioPinId       tkPin,
                  uint8_t... tkIdxs>
        constexpr std::optional<uint8_t> get_timer_pin_index(std::integer_sequence<uint8_t, tkIdxs...>)
        {
            std::optional<uint8_t> result = std::nullopt;
            ((CValidSpecificTimPin<tkControllerId, tkPinType, tkIdxs, tkPort, tkPin> ? (result = tkIdxs, true)
                                                                                     : false) ||
             ...);
            return result;
        }

        template <TimControllerId tkControllerId,
                  TimGpioPinType  tkPinType,
                  GpioPortId      tkPort,
                  GpioPinId       tkPin,
                  uint8_t... tkIdxs>
        constexpr bool check_any_valid_timer_pin(std::integer_sequence<uint8_t, tkIdxs...>)
        {
            return get_timer_pin_index<tkControllerId, tkPinType, tkPort, tkPin>(
                       std::integer_sequence<uint8_t, tkIdxs...>{})
                .has_value();
        }
    }  // namespace detail

    template <TimControllerId tkControllerId, TimGpioPinType tkPinType, GpioPortId tkPort, GpioPinId tkPin>
    concept CValidTimPin = detail::check_any_valid_timer_pin<tkControllerId, tkPinType, tkPort, tkPin>(
        std::make_integer_sequence<uint8_t, kTimMaxOutputIndex>{});

    template <TimControllerId tkControllerId, TimGpioPinType tkPinType, GpioPortId tkPort, GpioPinId tkPin>
        requires(CValidTimPin<tkControllerId, tkPinType, tkPort, tkPin>)
    constexpr uint8_t kTimPinOutputIndex = detail::get_timer_pin_index<tkControllerId, tkPinType, tkPort, tkPin>(
                                               std::make_integer_sequence<uint8_t, kTimMaxOutputIndex>{})
                                               .value();

    template <TimControllerId tkControllerId, TimGpioPinType tkPinType, GpioPortId tkPort, GpioPinId tkPin>
        requires(CValidTimPin<tkControllerId, tkPinType, tkPort, tkPin>)
    constexpr GpioAlternativeFunction kTimPinAF =
        TimPinMap<tkControllerId, tkPinType, kTimPinOutputIndex<tkControllerId, tkPinType, tkPort, tkPin>>::skAF;

}  // namespace valle::platform