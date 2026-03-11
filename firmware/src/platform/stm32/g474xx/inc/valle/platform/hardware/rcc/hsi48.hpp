#pragma once

#include "stm32g4xx_ll_rcc.h"

namespace valle::platform
{
    // =============================================================================
    // INTERFACE
    // =============================================================================
    struct HSI48OscillatorInterface
    {
        static constexpr uint32_t skFrequencyHz                = HSI48_VALUE;
        static constexpr uint32_t skDefaultEnableTimeoutCount  = 100'000U;
        static constexpr uint32_t skDefaultDisableTimeoutCount = 100'000U;

        static void enable()
        {
            LL_RCC_HSI48_Enable();
        }

        static void disable()
        {
            LL_RCC_HSI48_Disable();
        }

        [[nodiscard]] static bool is_ready()
        {
            return LL_RCC_HSI48_IsReady() == 1;
        }

        [[nodiscard]] static uint32_t get_calibration()
        {
            return LL_RCC_HSI48_GetCalibration();
        }
    };

}  // namespace valle::platform