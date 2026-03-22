#pragma once

#include <concepts>
#include <cstdint>
#include <optional>
#include <utility>

#include "valle/platform/hardware/gpio.hpp"

namespace valle::platform
{
    enum class I2CControllerGPIOPinType : uint8_t
    {
        kSCL,
        kSDA,
        kSMBA,
    };

    constexpr static uint8_t kI2CMaxOutputIndex = 5;  // pick whatever upper bound you want

    template <I2CControllerID tkControllerID, I2CControllerGPIOPinType tkPinType, uint8_t tkOutputIdx = 0>
        requires(kValidI2CControllerID<tkControllerID> && (tkOutputIdx < kI2CMaxOutputIndex))
    struct I2CControllerPinMap;

#define DECLARE_I2C_PIN_MAP(tkControllerID, tkPinType, tkOutputIdx, port, pin, af) \
    static_assert(kValidI2CControllerID<(tkControllerID)>);                        \
    static_assert((tkOutputIdx) < kI2CMaxOutputIndex);                             \
    template <>                                                                    \
    struct I2CControllerPinMap<(tkControllerID), (tkPinType), (tkOutputIdx)>       \
    {                                                                              \
        constexpr static GPIOPortID              skPortID = (port);                \
        constexpr static GPIOPinID               skPinID  = (pin);                 \
        constexpr static GPIOAlternativeFunction skAF     = (af);                  \
    };

    // I2C1
    DECLARE_I2C_PIN_MAP(1, I2CControllerGPIOPinType::kSCL, 0, GPIOPortID::kA, 13, GPIOAlternativeFunction::kAF4);
    DECLARE_I2C_PIN_MAP(1, I2CControllerGPIOPinType::kSCL, 1, GPIOPortID::kA, 15, GPIOAlternativeFunction::kAF4);
    DECLARE_I2C_PIN_MAP(1, I2CControllerGPIOPinType::kSCL, 2, GPIOPortID::kB, 8, GPIOAlternativeFunction::kAF4);

    DECLARE_I2C_PIN_MAP(1, I2CControllerGPIOPinType::kSDA, 0, GPIOPortID::kA, 14, GPIOAlternativeFunction::kAF4);
    DECLARE_I2C_PIN_MAP(1, I2CControllerGPIOPinType::kSDA, 1, GPIOPortID::kB, 7, GPIOAlternativeFunction::kAF4);
    DECLARE_I2C_PIN_MAP(1, I2CControllerGPIOPinType::kSDA, 2, GPIOPortID::kB, 9, GPIOAlternativeFunction::kAF4);

    DECLARE_I2C_PIN_MAP(1, I2CControllerGPIOPinType::kSMBA, 0, GPIOPortID::kB, 5, GPIOAlternativeFunction::kAF4);

    // I2C2
    DECLARE_I2C_PIN_MAP(2, I2CControllerGPIOPinType::kSCL, 0, GPIOPortID::kA, 9, GPIOAlternativeFunction::kAF4);
    DECLARE_I2C_PIN_MAP(2, I2CControllerGPIOPinType::kSCL, 1, GPIOPortID::kC, 4, GPIOAlternativeFunction::kAF4);
    DECLARE_I2C_PIN_MAP(2, I2CControllerGPIOPinType::kSCL, 2, GPIOPortID::kF, 6, GPIOAlternativeFunction::kAF4);

    DECLARE_I2C_PIN_MAP(2, I2CControllerGPIOPinType::kSDA, 0, GPIOPortID::kA, 8, GPIOAlternativeFunction::kAF4);
    DECLARE_I2C_PIN_MAP(2, I2CControllerGPIOPinType::kSDA, 1, GPIOPortID::kF, 0, GPIOAlternativeFunction::kAF4);

    DECLARE_I2C_PIN_MAP(2, I2CControllerGPIOPinType::kSMBA, 0, GPIOPortID::kA, 10, GPIOAlternativeFunction::kAF4);
    DECLARE_I2C_PIN_MAP(2, I2CControllerGPIOPinType::kSMBA, 1, GPIOPortID::kB, 12, GPIOAlternativeFunction::kAF4);
    DECLARE_I2C_PIN_MAP(2, I2CControllerGPIOPinType::kSMBA, 2, GPIOPortID::kF, 2, GPIOAlternativeFunction::kAF4);

    // I2C3
    DECLARE_I2C_PIN_MAP(3, I2CControllerGPIOPinType::kSCL, 0, GPIOPortID::kA, 8, GPIOAlternativeFunction::kAF2);
    DECLARE_I2C_PIN_MAP(3, I2CControllerGPIOPinType::kSCL, 1, GPIOPortID::kC, 8, GPIOAlternativeFunction::kAF8);
    DECLARE_I2C_PIN_MAP(3, I2CControllerGPIOPinType::kSCL, 2, GPIOPortID::kF, 3, GPIOAlternativeFunction::kAF4);
    DECLARE_I2C_PIN_MAP(3, I2CControllerGPIOPinType::kSCL, 3, GPIOPortID::kG, 7, GPIOAlternativeFunction::kAF4);

    DECLARE_I2C_PIN_MAP(3, I2CControllerGPIOPinType::kSDA, 0, GPIOPortID::kB, 5, GPIOAlternativeFunction::kAF8);
    DECLARE_I2C_PIN_MAP(3, I2CControllerGPIOPinType::kSDA, 1, GPIOPortID::kC, 9, GPIOAlternativeFunction::kAF8);
    DECLARE_I2C_PIN_MAP(3, I2CControllerGPIOPinType::kSDA, 2, GPIOPortID::kC, 11, GPIOAlternativeFunction::kAF8);
    DECLARE_I2C_PIN_MAP(3, I2CControllerGPIOPinType::kSDA, 3, GPIOPortID::kF, 4, GPIOAlternativeFunction::kAF4);
    DECLARE_I2C_PIN_MAP(3, I2CControllerGPIOPinType::kSDA, 4, GPIOPortID::kG, 8, GPIOAlternativeFunction::kAF4);

    DECLARE_I2C_PIN_MAP(3, I2CControllerGPIOPinType::kSMBA, 0, GPIOPortID::kA, 9, GPIOAlternativeFunction::kAF2);
    DECLARE_I2C_PIN_MAP(3, I2CControllerGPIOPinType::kSMBA, 1, GPIOPortID::kB, 2, GPIOAlternativeFunction::kAF4);
    DECLARE_I2C_PIN_MAP(3, I2CControllerGPIOPinType::kSMBA, 2, GPIOPortID::kG, 6, GPIOAlternativeFunction::kAF4);

    // I2C4
    DECLARE_I2C_PIN_MAP(4, I2CControllerGPIOPinType::kSCL, 0, GPIOPortID::kA, 13, GPIOAlternativeFunction::kAF3);
    DECLARE_I2C_PIN_MAP(4, I2CControllerGPIOPinType::kSCL, 1, GPIOPortID::kC, 6, GPIOAlternativeFunction::kAF8);
    DECLARE_I2C_PIN_MAP(4, I2CControllerGPIOPinType::kSCL, 2, GPIOPortID::kF, 14, GPIOAlternativeFunction::kAF4);
    DECLARE_I2C_PIN_MAP(4, I2CControllerGPIOPinType::kSCL, 3, GPIOPortID::kG, 3, GPIOAlternativeFunction::kAF4);

    DECLARE_I2C_PIN_MAP(4, I2CControllerGPIOPinType::kSDA, 0, GPIOPortID::kB, 7, GPIOAlternativeFunction::kAF3);
    DECLARE_I2C_PIN_MAP(4, I2CControllerGPIOPinType::kSDA, 1, GPIOPortID::kC, 7, GPIOAlternativeFunction::kAF8);
    DECLARE_I2C_PIN_MAP(4, I2CControllerGPIOPinType::kSDA, 2, GPIOPortID::kF, 15, GPIOAlternativeFunction::kAF4);
    DECLARE_I2C_PIN_MAP(4, I2CControllerGPIOPinType::kSDA, 3, GPIOPortID::kG, 4, GPIOAlternativeFunction::kAF4);

    DECLARE_I2C_PIN_MAP(4, I2CControllerGPIOPinType::kSMBA, 0, GPIOPortID::kA, 14, GPIOAlternativeFunction::kAF3);
    DECLARE_I2C_PIN_MAP(4, I2CControllerGPIOPinType::kSMBA, 1, GPIOPortID::kD, 11, GPIOAlternativeFunction::kAF4);
    DECLARE_I2C_PIN_MAP(4, I2CControllerGPIOPinType::kSMBA, 2, GPIOPortID::kF, 13, GPIOAlternativeFunction::kAF4);

#undef DECLARE_I2C_PIN_MAP

    namespace detail
    {
        template <I2CControllerID          tkControllerID,
                  I2CControllerGPIOPinType tkPinType,
                  uint8_t                  tkOutputIdx,
                  GPIOPortID               tkPortID,
                  GPIOPinID                tkPinID>
        concept CValidSpecificI2CControllerPin = requires {
            I2CControllerPinMap<tkControllerID, tkPinType, tkOutputIdx>::skPortID == tkPortID;
            I2CControllerPinMap<tkControllerID, tkPinType, tkOutputIdx>::skPinID == tkPinID;
        };

        template <I2CControllerID          tkControllerID,
                  I2CControllerGPIOPinType tkPinType,
                  GPIOPortID               tkPortID,
                  GPIOPinID                tkPinID,
                  uint8_t... tkIdxs>
        constexpr std::optional<uint8_t> get_i2c_controller_pin_output_index(std::integer_sequence<uint8_t, tkIdxs...>)
        {
            std::optional<uint8_t> result = std::nullopt;
            ((CValidSpecificI2CControllerPin<tkControllerID, tkPinType, tkIdxs, tkPortID, tkPinID>
                  ? (result = tkIdxs, true)
                  : false) ||
             ...);
            return result;
        }

        template <I2CControllerID          tkControllerID,
                  I2CControllerGPIOPinType tkPinType,
                  GPIOPortID               tkPortID,
                  GPIOPinID                tkPinID,
                  uint8_t... tkIdxs>
        constexpr bool check_any_valid_i2c_controller_pin_map(std::integer_sequence<uint8_t, tkIdxs...>)
        {
            return get_i2c_controller_pin_output_index<tkControllerID, tkPinType, tkPortID, tkPinID>(
                       std::integer_sequence<uint8_t, tkIdxs...>{})
                .has_value();
        }
    }  // namespace detail

    template <I2CControllerID          tkControllerID,
              I2CControllerGPIOPinType tkPinType,
              GPIOPortID               tkPortID,
              GPIOPinID                tkPinID>
    concept CValidI2CControllerPin =
        detail::check_any_valid_i2c_controller_pin_map<tkControllerID, tkPinType, tkPortID, tkPinID>(
            std::make_integer_sequence<uint8_t, kI2CMaxOutputIndex>{});

    template <I2CControllerID          tkControllerID,
              I2CControllerGPIOPinType tkPinType,
              GPIOPortID               tkPortID,
              GPIOPinID                tkPinID>
        requires(CValidI2CControllerPin<tkControllerID, tkPinType, tkPortID, tkPinID>)
    constexpr uint8_t kI2CControllerPinOutputIndex =
        detail::get_i2c_controller_pin_output_index<tkControllerID, tkPinType, tkPortID, tkPinID>(
            std::make_integer_sequence<uint8_t, kI2CMaxOutputIndex>{})
            .value();

    template <I2CControllerID          tkControllerID,
              I2CControllerGPIOPinType tkPinType,
              GPIOPortID               tkPortID,
              GPIOPinID                tkPinID>
        requires(CValidI2CControllerPin<tkControllerID, tkPinType, tkPortID, tkPinID>)
    constexpr GPIOAlternativeFunction kI2CControllerPinAF =
        I2CControllerPinMap<tkControllerID,
                            tkPinType,
                            kI2CControllerPinOutputIndex<tkControllerID, tkPinType, tkPortID, tkPinID>>::skAF;

}  // namespace valle::platform