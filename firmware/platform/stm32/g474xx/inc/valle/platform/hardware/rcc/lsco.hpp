#pragma once

#include "stm32g4xx_ll_rcc.h"

namespace valle
{
    // =============================================================================
    // ENUMERATIONS
    // =============================================================================
    enum class LSCOClockSource : uint32_t
    {
        kLSI = LL_RCC_LSCO_CLKSOURCE_LSI,
        kLSE = LL_RCC_LSCO_CLKSOURCE_LSE
    };

    // =============================================================================
    // INTERFACE
    // =============================================================================
    struct LSCOInterface
    {
        static void set_source(const LSCOClockSource source)
        {
            LL_RCC_LSCO_SetSource(static_cast<uint32_t>(source));
        }

        static void enable()
        {
            LL_RCC_LSCO_Enable();
        }

        static void disable()
        {
            LL_RCC_LSCO_Disable();
        }
    };

}  // namespace valle