#pragma once

#include <concepts>
#include <cstdint>
#include <optional>
#include <utility>

#include "valle/platform/hardware/gpio.hpp"

namespace valle::platform
{
    enum class I2cControllerGpioPinType : uint8_t
    {
        kSCL,
        kSDA,
        kSMBA,
    };

    constexpr static uint8_t kI2cMaxOutputIndex = 5;  // pick whatever upper bound you want

    template <I2cControllerId tkControllerId, I2cControllerGpioPinType tkPinType, uint8_t tkOutputIdx = 0>

    struct I2cControllerPinMap;

#define DECLARE_I2C_PIN_MAP(tkControllerId, tkPinType, tkOutputIdx, port, pin, af) \
                                                                                   \
    static_assert((tkOutputIdx) < kI2cMaxOutputIndex);                             \
    template <>                                                                    \
    struct I2cControllerPinMap<(tkControllerId), (tkPinType), (tkOutputIdx)>       \
    {                                                                              \
        constexpr static GpioPortId              skPortId = (port);                \
        constexpr static GpioPinId               skPinId  = (pin);                 \
        constexpr static GpioAlternativeFunction skAF     = (af);                  \
    };

    // I2c1
    DECLARE_I2C_PIN_MAP(I2cControllerId::kI2c1,
                        I2cControllerGpioPinType::kSCL,
                        0,
                        GpioPortId::kPortA,
                        GpioPinId::kPin13,
                        GpioAlternativeFunction::kAF4);
    DECLARE_I2C_PIN_MAP(I2cControllerId::kI2c1,
                        I2cControllerGpioPinType::kSCL,
                        1,
                        GpioPortId::kPortA,
                        GpioPinId::kPin15,
                        GpioAlternativeFunction::kAF4);
    DECLARE_I2C_PIN_MAP(I2cControllerId::kI2c1,
                        I2cControllerGpioPinType::kSCL,
                        2,
                        GpioPortId::kPortB,
                        GpioPinId::kPin8,
                        GpioAlternativeFunction::kAF4);

    DECLARE_I2C_PIN_MAP(I2cControllerId::kI2c1,
                        I2cControllerGpioPinType::kSDA,
                        0,
                        GpioPortId::kPortA,
                        GpioPinId::kPin14,
                        GpioAlternativeFunction::kAF4);
    DECLARE_I2C_PIN_MAP(I2cControllerId::kI2c1,
                        I2cControllerGpioPinType::kSDA,
                        1,
                        GpioPortId::kPortB,
                        GpioPinId::kPin7,
                        GpioAlternativeFunction::kAF4);
    DECLARE_I2C_PIN_MAP(I2cControllerId::kI2c1,
                        I2cControllerGpioPinType::kSDA,
                        2,
                        GpioPortId::kPortB,
                        GpioPinId::kPin9,
                        GpioAlternativeFunction::kAF4);

    DECLARE_I2C_PIN_MAP(I2cControllerId::kI2c1,
                        I2cControllerGpioPinType::kSMBA,
                        0,
                        GpioPortId::kPortB,
                        GpioPinId::kPin5,
                        GpioAlternativeFunction::kAF4);

    // I2c2
    DECLARE_I2C_PIN_MAP(I2cControllerId::kI2c2,
                        I2cControllerGpioPinType::kSCL,
                        0,
                        GpioPortId::kPortA,
                        GpioPinId::kPin9,
                        GpioAlternativeFunction::kAF4);
    DECLARE_I2C_PIN_MAP(I2cControllerId::kI2c2,
                        I2cControllerGpioPinType::kSCL,
                        1,
                        GpioPortId::kPortC,
                        GpioPinId::kPin4,
                        GpioAlternativeFunction::kAF4);
    DECLARE_I2C_PIN_MAP(I2cControllerId::kI2c2,
                        I2cControllerGpioPinType::kSCL,
                        2,
                        GpioPortId::kPortF,
                        GpioPinId::kPin6,
                        GpioAlternativeFunction::kAF4);

    DECLARE_I2C_PIN_MAP(I2cControllerId::kI2c2,
                        I2cControllerGpioPinType::kSDA,
                        0,
                        GpioPortId::kPortA,
                        GpioPinId::kPin8,
                        GpioAlternativeFunction::kAF4);
    DECLARE_I2C_PIN_MAP(I2cControllerId::kI2c2,
                        I2cControllerGpioPinType::kSDA,
                        1,
                        GpioPortId::kPortF,
                        GpioPinId::kPin0,
                        GpioAlternativeFunction::kAF4);

    DECLARE_I2C_PIN_MAP(I2cControllerId::kI2c2,
                        I2cControllerGpioPinType::kSMBA,
                        0,
                        GpioPortId::kPortA,
                        GpioPinId::kPin10,
                        GpioAlternativeFunction::kAF4);
    DECLARE_I2C_PIN_MAP(I2cControllerId::kI2c2,
                        I2cControllerGpioPinType::kSMBA,
                        1,
                        GpioPortId::kPortB,
                        GpioPinId::kPin12,
                        GpioAlternativeFunction::kAF4);
    DECLARE_I2C_PIN_MAP(I2cControllerId::kI2c2,
                        I2cControllerGpioPinType::kSMBA,
                        2,
                        GpioPortId::kPortF,
                        GpioPinId::kPin2,
                        GpioAlternativeFunction::kAF4);

    // I2c3
    DECLARE_I2C_PIN_MAP(I2cControllerId::kI2c3,
                        I2cControllerGpioPinType::kSCL,
                        0,
                        GpioPortId::kPortA,
                        GpioPinId::kPin8,
                        GpioAlternativeFunction::kAF2);
    DECLARE_I2C_PIN_MAP(I2cControllerId::kI2c3,
                        I2cControllerGpioPinType::kSCL,
                        1,
                        GpioPortId::kPortC,
                        GpioPinId::kPin8,
                        GpioAlternativeFunction::kAF8);
    DECLARE_I2C_PIN_MAP(I2cControllerId::kI2c3,
                        I2cControllerGpioPinType::kSCL,
                        2,
                        GpioPortId::kPortF,
                        GpioPinId::kPin3,
                        GpioAlternativeFunction::kAF4);
    DECLARE_I2C_PIN_MAP(I2cControllerId::kI2c3,
                        I2cControllerGpioPinType::kSCL,
                        3,
                        GpioPortId::kPortG,
                        GpioPinId::kPin7,
                        GpioAlternativeFunction::kAF4);

    DECLARE_I2C_PIN_MAP(I2cControllerId::kI2c3,
                        I2cControllerGpioPinType::kSDA,
                        0,
                        GpioPortId::kPortB,
                        GpioPinId::kPin5,
                        GpioAlternativeFunction::kAF8);
    DECLARE_I2C_PIN_MAP(I2cControllerId::kI2c3,
                        I2cControllerGpioPinType::kSDA,
                        1,
                        GpioPortId::kPortC,
                        GpioPinId::kPin9,
                        GpioAlternativeFunction::kAF8);
    DECLARE_I2C_PIN_MAP(I2cControllerId::kI2c3,
                        I2cControllerGpioPinType::kSDA,
                        2,
                        GpioPortId::kPortC,
                        GpioPinId::kPin11,
                        GpioAlternativeFunction::kAF8);
    DECLARE_I2C_PIN_MAP(I2cControllerId::kI2c3,
                        I2cControllerGpioPinType::kSDA,
                        3,
                        GpioPortId::kPortF,
                        GpioPinId::kPin4,
                        GpioAlternativeFunction::kAF4);
    DECLARE_I2C_PIN_MAP(I2cControllerId::kI2c3,
                        I2cControllerGpioPinType::kSDA,
                        4,
                        GpioPortId::kPortG,
                        GpioPinId::kPin8,
                        GpioAlternativeFunction::kAF4);

    DECLARE_I2C_PIN_MAP(I2cControllerId::kI2c3,
                        I2cControllerGpioPinType::kSMBA,
                        0,
                        GpioPortId::kPortA,
                        GpioPinId::kPin9,
                        GpioAlternativeFunction::kAF2);
    DECLARE_I2C_PIN_MAP(I2cControllerId::kI2c3,
                        I2cControllerGpioPinType::kSMBA,
                        1,
                        GpioPortId::kPortB,
                        GpioPinId::kPin2,
                        GpioAlternativeFunction::kAF4);
    DECLARE_I2C_PIN_MAP(I2cControllerId::kI2c3,
                        I2cControllerGpioPinType::kSMBA,
                        2,
                        GpioPortId::kPortG,
                        GpioPinId::kPin6,
                        GpioAlternativeFunction::kAF4);

    // I2c4
    DECLARE_I2C_PIN_MAP(I2cControllerId::kI2c4,
                        I2cControllerGpioPinType::kSCL,
                        0,
                        GpioPortId::kPortA,
                        GpioPinId::kPin13,
                        GpioAlternativeFunction::kAF3);
    DECLARE_I2C_PIN_MAP(I2cControllerId::kI2c4,
                        I2cControllerGpioPinType::kSCL,
                        1,
                        GpioPortId::kPortC,
                        GpioPinId::kPin6,
                        GpioAlternativeFunction::kAF8);
    DECLARE_I2C_PIN_MAP(I2cControllerId::kI2c4,
                        I2cControllerGpioPinType::kSCL,
                        2,
                        GpioPortId::kPortF,
                        GpioPinId::kPin14,
                        GpioAlternativeFunction::kAF4);
    DECLARE_I2C_PIN_MAP(I2cControllerId::kI2c4,
                        I2cControllerGpioPinType::kSCL,
                        3,
                        GpioPortId::kPortG,
                        GpioPinId::kPin3,
                        GpioAlternativeFunction::kAF4);

    DECLARE_I2C_PIN_MAP(I2cControllerId::kI2c4,
                        I2cControllerGpioPinType::kSDA,
                        0,
                        GpioPortId::kPortB,
                        GpioPinId::kPin7,
                        GpioAlternativeFunction::kAF3);
    DECLARE_I2C_PIN_MAP(I2cControllerId::kI2c4,
                        I2cControllerGpioPinType::kSDA,
                        1,
                        GpioPortId::kPortC,
                        GpioPinId::kPin7,
                        GpioAlternativeFunction::kAF8);
    DECLARE_I2C_PIN_MAP(I2cControllerId::kI2c4,
                        I2cControllerGpioPinType::kSDA,
                        2,
                        GpioPortId::kPortF,
                        GpioPinId::kPin15,
                        GpioAlternativeFunction::kAF4);
    DECLARE_I2C_PIN_MAP(I2cControllerId::kI2c4,
                        I2cControllerGpioPinType::kSDA,
                        3,
                        GpioPortId::kPortG,
                        GpioPinId::kPin4,
                        GpioAlternativeFunction::kAF4);

    DECLARE_I2C_PIN_MAP(I2cControllerId::kI2c4,
                        I2cControllerGpioPinType::kSMBA,
                        0,
                        GpioPortId::kPortA,
                        GpioPinId::kPin14,
                        GpioAlternativeFunction::kAF3);
    DECLARE_I2C_PIN_MAP(I2cControllerId::kI2c4,
                        I2cControllerGpioPinType::kSMBA,
                        1,
                        GpioPortId::kPortD,
                        GpioPinId::kPin11,
                        GpioAlternativeFunction::kAF4);
    DECLARE_I2C_PIN_MAP(I2cControllerId::kI2c4,
                        I2cControllerGpioPinType::kSMBA,
                        2,
                        GpioPortId::kPortF,
                        GpioPinId::kPin13,
                        GpioAlternativeFunction::kAF4);

#undef DECLARE_I2C_PIN_MAP

    namespace detail
    {
        template <I2cControllerId          tkControllerId,
                  I2cControllerGpioPinType tkPinType,
                  uint8_t                  tkOutputIdx,
                  GpioPortId               tkPortId,
                  GpioPinId                tkPinId>
        concept CValidSpecificI2cControllerPin = requires {
            I2cControllerPinMap<tkControllerId, tkPinType, tkOutputIdx>::skPortId == tkPortId;
            I2cControllerPinMap<tkControllerId, tkPinType, tkOutputIdx>::skPinId == tkPinId;
        };

        template <I2cControllerId          tkControllerId,
                  I2cControllerGpioPinType tkPinType,
                  GpioPortId               tkPortId,
                  GpioPinId                tkPinId,
                  uint8_t... tkIdxs>
        constexpr std::optional<uint8_t> get_i2c_controller_pin_output_index(std::integer_sequence<uint8_t, tkIdxs...>)
        {
            std::optional<uint8_t> result = std::nullopt;
            ((CValidSpecificI2cControllerPin<tkControllerId, tkPinType, tkIdxs, tkPortId, tkPinId>
                  ? (result = tkIdxs, true)
                  : false) ||
             ...);
            return result;
        }

        template <I2cControllerId          tkControllerId,
                  I2cControllerGpioPinType tkPinType,
                  GpioPortId               tkPortId,
                  GpioPinId                tkPinId,
                  uint8_t... tkIdxs>
        constexpr bool check_any_valid_i2c_controller_pin_map(std::integer_sequence<uint8_t, tkIdxs...>)
        {
            return get_i2c_controller_pin_output_index<tkControllerId, tkPinType, tkPortId, tkPinId>(
                       std::integer_sequence<uint8_t, tkIdxs...>{})
                .has_value();
        }
    }  // namespace detail

    template <I2cControllerId          tkControllerId,
              I2cControllerGpioPinType tkPinType,
              GpioPortId               tkPortId,
              GpioPinId                tkPinId>
    concept CValidI2cControllerPin =
        detail::check_any_valid_i2c_controller_pin_map<tkControllerId, tkPinType, tkPortId, tkPinId>(
            std::make_integer_sequence<uint8_t, kI2cMaxOutputIndex>{});

    template <I2cControllerId          tkControllerId,
              I2cControllerGpioPinType tkPinType,
              GpioPortId               tkPortId,
              GpioPinId                tkPinId>
        requires(CValidI2cControllerPin<tkControllerId, tkPinType, tkPortId, tkPinId>)
    constexpr uint8_t kI2cControllerPinOutputIndex =
        detail::get_i2c_controller_pin_output_index<tkControllerId, tkPinType, tkPortId, tkPinId>(
            std::make_integer_sequence<uint8_t, kI2cMaxOutputIndex>{})
            .value();

    template <I2cControllerId          tkControllerId,
              I2cControllerGpioPinType tkPinType,
              GpioPortId               tkPortId,
              GpioPinId                tkPinId>
        requires(CValidI2cControllerPin<tkControllerId, tkPinType, tkPortId, tkPinId>)
    constexpr GpioAlternativeFunction kI2cControllerPinAF =
        I2cControllerPinMap<tkControllerId,
                            tkPinType,
                            kI2cControllerPinOutputIndex<tkControllerId, tkPinType, tkPortId, tkPinId>>::skAF;

}  // namespace valle::platform
