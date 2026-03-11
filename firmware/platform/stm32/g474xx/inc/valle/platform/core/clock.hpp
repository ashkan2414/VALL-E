#pragma once

#include <cstdint>

#include "stm32g4xx.h"
#include "stm32g4xx_hal_rcc.h"
#include "stm32g4xx_ll_rcc.h"

namespace valle
{
    constexpr uint32_t kSystemClockFreqHz = 170'000'000;  // 170 MHz

    // =========================================================================
    // UTILITY FUNCTIONS
    // =========================================================================

    /**
     * @brief Get the APB2 Timer Clock Frequency (TIM1, TIM8, HRTIM1)
     * @return uint32_t Frequency in Hz
     */
    inline uint32_t get_apb2_timer_clock_freq()
    {
        const uint32_t pclk2_freq     = HAL_RCC_GetPCLK2Freq();
        const uint32_t apb2_prescaler = LL_RCC_GetAPB2Prescaler();
        if (apb2_prescaler == LL_RCC_APB2_DIV_1)
        {
            return pclk2_freq;
        }

        return pclk2_freq * 2;
    }

};  // namespace valle