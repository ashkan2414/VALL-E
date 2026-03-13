#pragma once

#include <atomic>
#include <cstdint>

#include "stm32g474xx.h"

namespace valle::platform
{
    class CriticalSection
    {
    private:
        uint32_t m_primask;

    public:
        CriticalSection() : m_primask(__get_PRIMASK())
        {
            __disable_irq();
            __DSB();
            __ISB();
        }

        ~CriticalSection()
        {
            __set_PRIMASK(m_primask);
        };

        // Delete copy/move to prevent accidental double-unlocks
        CriticalSection(const CriticalSection&)            = delete;
        CriticalSection& operator=(const CriticalSection&) = delete;
        CriticalSection(CriticalSection&&)                 = delete;
        CriticalSection& operator=(CriticalSection&&)      = delete;
    };

}  // namespace valle::platform