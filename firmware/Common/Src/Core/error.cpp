#include "Valle/Core/error.hpp"

#include "stm32g4xx_hal.h"

namespace valle
{
    void fault_handler()
    {
        __disable_irq();
        while (1)
        {
        }
    }

    extern "C"
    {
#ifdef USE_FULL_ASSERT
        /**
         * @brief  Reports the name of the source file and the source line number
         *         where the assert_param error has occurred (used by HAL).
         * @param  file: pointer to the source file name
         * @param  line: assert_param error line source number
         * @retval None
         */
        void assert_failed(uint8_t *file, uint32_t line)
        {
            fault_handler();
        }
#endif /* USE_FULL_ASSERT */

        /**
         * @brief This function handles Non maskable interrupt.
         */
        void NMI_Handler(void)  // NOLINT(readability-identifier-naming)
        {
            fault_handler();
        }

        /**
         * @brief This function handles Hard fault interrupt.
         */
        void HardFault_Handler(void)  // NOLINT(readability-identifier-naming)
        {
            fault_handler();
        }

        /**
         * @brief This function handles Memory management fault.
         */
        void MemManage_Handler(void)  // NOLINT(readability-identifier-naming)
        {
            fault_handler();
        }

        /**
         * @brief This function handles Prefetch fault, memory access fault.
         */
        void BusFault_Handler(void)  // NOLINT(readability-identifier-naming)
        {
            fault_handler();
        }

        /**
         * @brief This function handles Undefined instruction or illegal state.
         */
        void UsageFault_Handler(void)  // NOLINT(readability-identifier-naming)
        {
            fault_handler();
        }
    }

}  // namespace valle