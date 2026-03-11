#pragma once

#include "stm32g4xx_ll_rcc.h"

namespace valle
{
    // =============================================================================
    // INTERFACE
    // =============================================================================
    struct LSIOscillatorInterface
    {
        static constexpr uint32_t skFrequencyHz                = LSI_VALUE;
        static constexpr uint32_t skDefaultEnableTimeoutCount  = 500'000U;
        static constexpr uint32_t skDefaultDisableTimeoutCount = 500'000U;

        static void enable()
        {
            LL_RCC_LSI_Enable();
        }

        static void disable()
        {
            LL_RCC_LSI_Disable();
        }

        [[nodiscard]] static bool is_ready()
        {
            return LL_RCC_LSI_IsReady() == 1;
        }
    };

}  // namespace valle