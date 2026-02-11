#pragma once

#include <atomic>

#include "valle/System/hardware.hpp"

namespace valle::app
{
    extern std::atomic<uint32_t> g_rep_counter;
    void                         init();
}  // namespace valle::app
