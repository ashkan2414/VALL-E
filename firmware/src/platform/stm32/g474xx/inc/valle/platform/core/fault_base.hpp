#pragma once

#include "stm32g474xx.h"
#include "valle/base/panic.hpp"
#include "valle/platform/core/logging.hpp"
#include "valle/utils/enum.hpp"
#include "valle/utils/tmp.hpp"

namespace valle::platform
{
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

    struct FaultInterruptSourceInfo
    {
        FaultInterruptType fault_type;
    };

    using FaultSources = TypeList<PanicSourceInfo, AssertSourceInfo, FaultInterruptSourceInfo>;

    template <typename TSource>
    concept CFaultSource = TypeListContains<TSource, FaultSources>::value;

    template <CFaultSource TSource>
    void default_fault_handler(TSource&& source)
    {
        __disable_irq();

        if constexpr (std::same_as<TSource, PanicSourceInfo>)
        {
            VALLE_LOG_ERROR("Panic: {} at {}:{} in function {}",
                            source.message,
                            source.location.file_name,
                            source.location.line,
                            source.location.function_name);
        }
        else if constexpr (std::same_as<TSource, AssertSourceInfo>)
        {
            VALLE_LOG_ERROR("Assert failed at {}:{}", source.file, source.line);
        }
        else if constexpr (std::same_as<TSource, FaultInterruptSourceInfo>)
        {
            VALLE_LOG_ERROR("Fault interrupt: {}", enum_name(source.fault_type));
        }

        while (true)
        {
        }
    }

    template <CFaultSource TSource>
    struct FaultHandler
    {
        static void handle(TSource&& source)
        {
            default_fault_handler<TSource>(std::move(source));
        }
    };

}  // namespace valle::platform