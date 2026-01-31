
namespace valle
{
    extern "C"
    {
        /**
         * @brief This function handles System service call via SWI instruction.
         */
        __attribute__((weak)) void SVC_Handler(void)  // NOLINT(readability-identifier-naming)
        {
        }

        /**
         * @brief This function handles Debug monitor.
         */
        __attribute__((weak)) void DebugMon_Handler(void)  // NOLINT(readability-identifier-naming)
        {
        }

        /**
         * @brief This function handles Pendable request for system service.
         */
        __attribute__((weak)) void PendSV_Handler(void)  // NOLINT(readability-identifier-naming)
        {
        }

        /**
         * @brief This function handles System tick timer.
         */
        __attribute__((weak)) void SysTick_Handler(void)  // NOLINT(readability-identifier-naming)
        {
        }
    }
}  // namespace valle