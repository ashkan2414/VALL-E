#pragma once

#include <stdint.h>

namespace valle
{
    constexpr uint32_t kSystemClockFreqHz = 170'000'000;  // 170 MHz

    /**
     * @brief System Clock Configuration
     */
    void config_system_clock();

};  // namespace valle