#pragma once

#include <cstdint>

#include "stm32g4xx_ll_gpio.h"
#include "valle/platform/core/id.hpp"

namespace valle::platform
{
    struct GpioPortId : public SequentialEnumValue<GpioPortId, uint8_t, 0, 7>
    {
        static const GpioPortId kPortA;
        static const GpioPortId kPortB;
        static const GpioPortId kPortC;
        static const GpioPortId kPortD;
        static const GpioPortId kPortE;
        static const GpioPortId kPortF;
        static const GpioPortId kPortG;
    };

    inline constexpr GpioPortId GpioPortId::kPortA = GpioPortId::from_number<0>();
    inline constexpr GpioPortId GpioPortId::kPortB = GpioPortId::from_number<1>();
    inline constexpr GpioPortId GpioPortId::kPortC = GpioPortId::from_number<2>();
    inline constexpr GpioPortId GpioPortId::kPortD = GpioPortId::from_number<3>();
    inline constexpr GpioPortId GpioPortId::kPortE = GpioPortId::from_number<4>();
    inline constexpr GpioPortId GpioPortId::kPortF = GpioPortId::from_number<5>();
    inline constexpr GpioPortId GpioPortId::kPortG = GpioPortId::from_number<6>();

    inline constexpr auto kGpioPinIdNumberMapping =
        LLDriverEnumValueSequentialNumberMapping<16>{.start_number = 0,
                                                     .ll_id_table  = {LL_GPIO_PIN_0,
                                                                      LL_GPIO_PIN_1,
                                                                      LL_GPIO_PIN_2,
                                                                      LL_GPIO_PIN_3,
                                                                      LL_GPIO_PIN_4,
                                                                      LL_GPIO_PIN_5,
                                                                      LL_GPIO_PIN_6,
                                                                      LL_GPIO_PIN_7,
                                                                      LL_GPIO_PIN_8,
                                                                      LL_GPIO_PIN_9,
                                                                      LL_GPIO_PIN_10,
                                                                      LL_GPIO_PIN_11,
                                                                      LL_GPIO_PIN_12,
                                                                      LL_GPIO_PIN_13,
                                                                      LL_GPIO_PIN_14,
                                                                      LL_GPIO_PIN_15}};

    struct GpioPinId : public NumberedLLDriverEnumValue<GpioPinId, kGpioPinIdNumberMapping>
    {
        static const GpioPinId kPin0;
        static const GpioPinId kPin1;
        static const GpioPinId kPin2;
        static const GpioPinId kPin3;
        static const GpioPinId kPin4;
        static const GpioPinId kPin5;
        static const GpioPinId kPin6;
        static const GpioPinId kPin7;
        static const GpioPinId kPin8;
        static const GpioPinId kPin9;
        static const GpioPinId kPin10;
        static const GpioPinId kPin11;
        static const GpioPinId kPin12;
        static const GpioPinId kPin13;
        static const GpioPinId kPin14;
        static const GpioPinId kPin15;
    };

    inline constexpr GpioPinId GpioPinId::kPin0  = GpioPinId::from_number<0>();
    inline constexpr GpioPinId GpioPinId::kPin1  = GpioPinId::from_number<1>();
    inline constexpr GpioPinId GpioPinId::kPin2  = GpioPinId::from_number<2>();
    inline constexpr GpioPinId GpioPinId::kPin3  = GpioPinId::from_number<3>();
    inline constexpr GpioPinId GpioPinId::kPin4  = GpioPinId::from_number<4>();
    inline constexpr GpioPinId GpioPinId::kPin5  = GpioPinId::from_number<5>();
    inline constexpr GpioPinId GpioPinId::kPin6  = GpioPinId::from_number<6>();
    inline constexpr GpioPinId GpioPinId::kPin7  = GpioPinId::from_number<7>();
    inline constexpr GpioPinId GpioPinId::kPin8  = GpioPinId::from_number<8>();
    inline constexpr GpioPinId GpioPinId::kPin9  = GpioPinId::from_number<9>();
    inline constexpr GpioPinId GpioPinId::kPin10 = GpioPinId::from_number<10>();
    inline constexpr GpioPinId GpioPinId::kPin11 = GpioPinId::from_number<11>();
    inline constexpr GpioPinId GpioPinId::kPin12 = GpioPinId::from_number<12>();
    inline constexpr GpioPinId GpioPinId::kPin13 = GpioPinId::from_number<13>();
    inline constexpr GpioPinId GpioPinId::kPin14 = GpioPinId::from_number<14>();
    inline constexpr GpioPinId GpioPinId::kPin15 = GpioPinId::from_number<15>();

}  // namespace valle::platform