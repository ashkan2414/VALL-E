#pragma once

#include <atomic>

namespace valle
{

    template <typename T>
    struct Atomic : public std::atomic<T>
    {
        using std::atomic<T>::atomic;  // Inherit constructors

        // Move Constructor
        constexpr Atomic(Atomic&& other) noexcept : std::atomic<T>(other.load(std::memory_order_relaxed))
        {
        }

        // Move Assignment Operator
        constexpr Atomic& operator=(Atomic&& other) noexcept
        {
            if (this != &other)
            {
                this->store(other.load(std::memory_order_relaxed), std::memory_order_relaxed);
            }
            return *this;
        }
    };

}  // namespace valle