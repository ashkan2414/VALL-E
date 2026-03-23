#pragma once

#include <atomic>

#include "valle/base/system_build/build.hpp"
#include "valle/system/sync.hpp"
#include "valle/system/timing.hpp"

namespace valle::app
{
    void init();
    void start();
    void stop();

}  // namespace valle::app
