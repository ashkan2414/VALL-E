#pragma once

#include <atomic>
#include <cstdint>

#include "valle/base/platform_support/system_interface/sync.hpp"
#include "valle/platform/core/sync.hpp"

namespace valle::platform::system_interface
{
    using CriticalSection = valle::platform::CriticalSection;

}  // namespace valle::platform::system_interface