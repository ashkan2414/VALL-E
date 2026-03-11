

#include "valle/platform/system_interface/init.hpp"

#include "stm32g4xx_hal.h"

namespace valle::platform::system_interface
{
    void init()
    {
        /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
        HAL_Init();
    }

}  // namespace valle::platform::system_interface

extern "C"
{
    void HAL_MspInit(void)  // NOLINT(readability-identifier-naming)
    {
        __HAL_RCC_SYSCFG_CLK_ENABLE();
        __HAL_RCC_PWR_CLK_ENABLE();
        HAL_PWREx_DisableUCPDDeadBattery();
    }
}