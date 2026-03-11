#include "valle/base/panic.hpp"

#include "valle/platform/core/fault.hpp"

namespace valle
{
    void panic_handler(const PanicSourceInfo& info)
    {
        platform::FaultHandler<PanicSourceInfo>::handle(info);
    }
}  // namespace valle