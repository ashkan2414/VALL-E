#pragma once

#include "stm32g4xx_ll_rcc.h"

namespace valle::platform
{

    // ============================================================================
    // HARDWARE TRAITS
    // ============================================================================

    struct HSIOscillatorInterface
    {
        static constexpr uint32_t skFrequencyHz                = HSI_VALUE;
        static constexpr uint32_t skDefaultCalibrationTrim     = 0x40U;
        static constexpr uint32_t skDefaultEnableTimeoutCount  = 100'000U;
        static constexpr uint32_t skDefaultDisableTimeoutCount = 100'000U;

        static void enable_in_stop_mode()
        {
            LL_RCC_HSI_EnableInStopMode();
        }

        static void disable_in_stop_mode()
        {
            LL_RCC_HSI_DisableInStopMode();
        }

        static void enable()
        {
            LL_RCC_HSI_Enable();
        }

        static void disable()
        {
            LL_RCC_HSI_Disable();
        }

        [[nodiscard]] static bool is_ready()
        {
            return LL_RCC_HSI_IsReady() == 1;
        }

        [[nodiscard]] static uint32_t get_calibration()
        {
            return LL_RCC_HSI_GetCalibration();
        }

        static void set_calibration_trimming(const uint32_t trim)
        {
            LL_RCC_HSI_SetCalibTrimming(trim);
        }

        [[nodiscard]] static uint32_t get_calibration_trimming()
        {
            return LL_RCC_HSI_GetCalibTrimming();
        }
    };

}  // namespace valle::platform