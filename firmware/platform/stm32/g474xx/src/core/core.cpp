#include "valle/platform/core.hpp"

namespace valle
{
    void platform_init()
    {
        /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
        HAL_Init();
    }

    extern "C"
    {
        void HAL_MspInit(void)  // NOLINT(readability-identifier-naming)
        {
            __HAL_RCC_SYSCFG_CLK_ENABLE();
            __HAL_RCC_PWR_CLK_ENABLE();
            HAL_PWREx_DisableUCPDDeadBattery();
        }
    }

}  // namespace valle
