#pragma once

#include <cstdint>

#include "valle/platform/core/id.hpp"

namespace valle::platform
{
    struct UartControllerId : public SequentialEnumValue<UartControllerId, uint8_t, 1, 6>
    {
        static const UartControllerId kUSART1;
        static const UartControllerId kUSART2;
        static const UartControllerId kUSART3;
        static const UartControllerId kUART4;
        static const UartControllerId kUART5;
        static const UartControllerId kLPUART1;
    };

    inline constexpr UartControllerId UartControllerId::kUSART1  = UartControllerId::from_number<1>();
    inline constexpr UartControllerId UartControllerId::kUSART2  = UartControllerId::from_number<2>();
    inline constexpr UartControllerId UartControllerId::kUSART3  = UartControllerId::from_number<3>();
    inline constexpr UartControllerId UartControllerId::kUART4   = UartControllerId::from_number<4>();
    inline constexpr UartControllerId UartControllerId::kUART5   = UartControllerId::from_number<5>();
    inline constexpr UartControllerId UartControllerId::kLPUART1 = UartControllerId::from_number<6>();

}  // namespace valle::platform