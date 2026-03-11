#include "valle/platform/system_backend/error.hpp"

#include <variant>

#include "stm32g4xx_hal.h"
#include "valle/platform/core/logging.hpp"

namespace valle::platform::system_backend
{
    struct PanicSourceInfo
    {
        std::string_view message{};
        SourceLocation   location{};
    };

    struct AssertSourceInfo
    {
        std::string_view file{};
        uint32_t         line{};
    };

    enum class FaultInterruptType
    {
        kHardFault,
        kMemManageFault,
        kBusFault,
        kUsageFault,
        kNMI
    };

    struct FaultInterruptContextInfo
    {
        FaultInterruptType fault_type;
    };

    struct FaultContext
    {
        std::variant<PanicSourceInfo, AssertSourceInfo, FaultInterruptContextInfo> source_info;
    };

    FaultContext g_fault_context;

    void fault_handler(FaultContext&& context)
    {
        __disable_irq();
        g_fault_context = std::move(context);

        std::visit(Overloaded{[](const PanicSourceInfo& info)
                              {
                                  VALLE_LOG_ERROR("Panic: {} at {}:{} in function {}",
                                                  info.message,
                                                  info.location.file_name,
                                                  info.location.line,
                                                  info.location.function_name);
                              },
                              [](const AssertSourceInfo& info)
                              { VALLE_LOG_ERROR("Assert failed at {}:{}", info.file, info.line); },
                              [](const FaultInterruptContextInfo& info)
                              {
                                  VALLE_LOG_ERROR("Fault interrupt: {}",
                                                  [&info]() -> std::string_view
                                                  {
                                                      switch (info.fault_type)
                                                      {
                                                          case FaultInterruptType::kHardFault:
                                                              return "HardFault";
                                                          case FaultInterruptType::kMemManageFault:
                                                              return "MemManageFault";
                                                          case FaultInterruptType::kBusFault:
                                                              return "BusFault";
                                                          case FaultInterruptType::kUsageFault:
                                                              return "UsageFault";
                                                          case FaultInterruptType::kNMI:
                                                              return "NMI";
                                                          default:
                                                              return "Unknown";
                                                      }
                                                  }());
                              }},
                   g_fault_context.source_info);

        while (true)
        {
        }
    }

    /**
     * @brief This function logs a panic message and halts the system.
     * @param message The panic message to log.
     * @param location The source location where the panic occurred.
     */
    // NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
    void panic_handler(std::string_view message, SourceLocation location)
    {
        fault_handler(FaultContext{.source_info = PanicSourceInfo{
                                       .message  = message,
                                       .location = location,
                                   }});
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
        void assert_failed(uint8_t* file, uint32_t line)
        {
            fault_handler(FaultContext{.source_info = AssertSourceInfo{
                                           .file = reinterpret_cast<const char*>(file),
                                           .line = line,
                                       }});
        }
#endif /* USE_FULL_ASSERT */

        /**
         * @brief This function handles Non maskable interrupt.
         */
        void NMI_Handler(void)  // NOLINT(readability-identifier-naming)
        {
            fault_handler(FaultContext{.source_info = FaultInterruptContextInfo{
                                           .fault_type = FaultInterruptType::kNMI,
                                       }});
        }

        /**
         * @brief This function handles Hard fault interrupt.
         */
        void HardFault_Handler(void)  // NOLINT(readability-identifier-naming)
        {
            fault_handler(FaultContext{.source_info = FaultInterruptContextInfo{
                                           .fault_type = FaultInterruptType::kHardFault,
                                       }});
        }

        /**
         * @brief This function handles Memory management fault.
         */
        void MemManage_Handler(void)  // NOLINT(readability-identifier-naming)
        {
            fault_handler(FaultContext{.source_info = FaultInterruptContextInfo{
                                           .fault_type = FaultInterruptType::kMemManageFault,
                                       }});
        }

        /**
         * @brief This function handles Prefetch fault, memory access fault.
         */
        void BusFault_Handler(void)  // NOLINT(readability-identifier-naming)
        {
            fault_handler(FaultContext{.source_info = FaultInterruptContextInfo{
                                           .fault_type = FaultInterruptType::kBusFault,
                                       }});
        }

        /**
         * @brief This function handles Undefined instruction or illegal state.
         */
        void UsageFault_Handler(void)  // NOLINT(readability-identifier-naming)
        {
            fault_handler(FaultContext{.source_info = FaultInterruptContextInfo{
                                           .fault_type = FaultInterruptType::kUsageFault,
                                       }});
        }
    }

}  // namespace valle::platform::system_backend