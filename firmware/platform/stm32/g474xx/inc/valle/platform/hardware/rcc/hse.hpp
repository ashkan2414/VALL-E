#pragma once

#include "stm32g4xx_ll_rcc.h"

namespace valle
{
    // =============================================================================
    // ENUMERATIONS
    // =============================================================================
    enum class HSEOscillatorMode
    {
        kCrystal,
        kBypass
    };

    // =============================================================================
    // INTERFACE
    // =============================================================================
    struct HSEOscillatorInterface
    {
        static constexpr uint32_t skFrequencyHz                = HSE_VALUE;
        static constexpr uint32_t skDefaultEnableTimeoutCount  = 5'000'000U;
        static constexpr uint32_t skDefaultDisableTimeoutCount = 5'000'000U;

        static void enable(const HSEOscillatorMode mode = HSEOscillatorMode::kCrystal)
        {
            switch (mode)
            {
                case HSEOscillatorMode::kCrystal:
                    LL_RCC_HSE_DisableBypass();
                    LL_RCC_HSE_Enable();
                    break;

                case HSEOscillatorMode::kBypass:
                    LL_RCC_HSE_EnableBypass();
                    LL_RCC_HSE_Enable();
                    break;
            }
        }

        static void enable_css()
        {
            LL_RCC_HSE_EnableCSS();
        }

        static void disable()
        {
            LL_RCC_HSE_Disable();
            LL_RCC_HSE_DisableBypass();
        }

        [[nodiscard]] static bool is_ready()
        {
            return LL_RCC_HSE_IsReady() == 1;
        }
    };

}  // namespace valle
