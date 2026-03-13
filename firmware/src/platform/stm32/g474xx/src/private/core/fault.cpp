#include "valle/platform/core/fault.hpp"

#include <variant>

#include "stm32g4xx_hal.h"
#include "valle/platform/core/logging.hpp"
#include "valle/utils/tmp.hpp"

namespace valle
{
    void panic_handler(PanicSourceInfo&& info)
    {
        platform::FaultHandler<PanicSourceInfo>::handle(std::move(info));
    }
}  // namespace valle

namespace valle::platform
{

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
        void assert_failed(uint8_t* file, uint32_t line)
        {
            FaultHandler<AssertSourceInfo>::handle(AssertSourceInfo{
                .file = reinterpret_cast<const char*>(file),
                .line = line,
            });
        }
#endif /* USE_FULL_ASSERT */

        /**
         * @brief This function handles Non maskable interrupt.
         */
        void NMI_Handler(void)  // NOLINT(readability-identifier-naming)
        {
            FaultHandler<FaultInterruptSourceInfo>::handle(FaultInterruptSourceInfo{
                .fault_type = FaultInterruptType::kNMI,
            });
        }

        /**
         * @brief This function handles Hard fault interrupt.
         */
        void HardFault_Handler(void)  // NOLINT(readability-identifier-naming)
        {
            FaultHandler<FaultInterruptSourceInfo>::handle(FaultInterruptSourceInfo{
                .fault_type = FaultInterruptType::kHardFault,
            });
        }

        /**
         * @brief This function handles Memory management fault.
         */
        void MemManage_Handler(void)  // NOLINT(readability-identifier-naming)
        {
            FaultHandler<FaultInterruptSourceInfo>::handle(FaultInterruptSourceInfo{
                .fault_type = FaultInterruptType::kMemManageFault,
            });
        }

        /**
         * @brief This function handles Prefetch fault, memory access fault.
         */
        void BusFault_Handler(void)  // NOLINT(readability-identifier-naming)
        {
            FaultHandler<FaultInterruptSourceInfo>::handle(FaultInterruptSourceInfo{
                .fault_type = FaultInterruptType::kBusFault,
            });
        }

        /**
         * @brief This function handles Undefined instruction or illegal state.
         */
        void UsageFault_Handler(void)  // NOLINT(readability-identifier-naming)
        {
            FaultHandler<FaultInterruptSourceInfo>::handle(FaultInterruptSourceInfo{
                .fault_type = FaultInterruptType::kUsageFault,
            });
        }
    }

}  // namespace valle::platform