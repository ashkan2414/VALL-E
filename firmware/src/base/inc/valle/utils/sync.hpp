#pragma once

#include <utility>

namespace valle
{
    template <typename T, typename TSyncGuard>
    class Synchronized
    {
    private:
        T m_data;

    public:
        template <typename... Args>
        explicit constexpr Synchronized(Args&&... args) : m_data(std::forward<Args>(args)...)
        {
        }

        /**
         * @brief Execute a function with exclusive access to the data.
         *
         * @tparam F Lambda/Callable type
         * @param func Logic to execute while interrupts are disabled
         */
        template <typename F>
        auto with_lock(F&& func)
        {
            TSyncGuard sync_guard;  // --- LOCK ---
            return func(m_data);
        }  // --- UNLOCK (via destructor) ---

        // Prevent direct access
        T* operator->() = delete;
        T& operator*()  = delete;
    };
}  // namespace valle