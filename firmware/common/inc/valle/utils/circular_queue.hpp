#pragma once

#include <algorithm>
#include <array>
#include <cstddef>
#include <iterator>
#include <optional>

namespace valle
{

    template <typename T, std::size_t tkCapacity, bool tkOverwrite = false>
    class CircularQueue
    {
    public:
        static_assert(tkCapacity > 0, "CircularQueue capacity must be greater than 0");

        /// The capacity of the circular queue
        static constexpr std::size_t skCapacity = tkCapacity;

        template <bool tkIsConst>
        class Iterator
        {
        private:
            using QueueType = std::conditional_t<tkIsConst, const CircularQueue, CircularQueue>;
            using ValueType = std::conditional_t<tkIsConst, const T, T>;

            QueueType*  m_p_queue;
            std::size_t m_index;
            std::size_t m_count;

        public:
            using iterator_category = std::forward_iterator_tag;
            using value_type        = ValueType;
            using difference_type   = std::ptrdiff_t;
            using pointer           = ValueType*;
            using reference         = ValueType&;

            // NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
            Iterator(QueueType* queue, std::size_t index, std::size_t count)
                : m_p_queue(queue), m_index(index), m_count(count)
            {
            }

            bool operator!=(const Iterator& other) const
            {
                return m_count != other.m_count;
            }

            Iterator& operator++()
            {
                m_index = (m_index + 1) % skCapacity;
                ++m_count;
                return *this;
            }

            reference operator*() const
            {
                return m_p_queue->m_buffer[m_index];
            }

            Iterator operator+(std::size_t n) const
            {
                Iterator temp = *this;
                temp.m_index  = (temp.m_index + n) % skCapacity;
                temp.m_count += n;
                return temp;
            }
        };

        using IteratorType      = Iterator<false>;
        using ConstIteratorType = Iterator<true>;

    private:
        std::array<T, tkCapacity> m_buffer{};  ///< The underlying array buffer for the queue.
        std::size_t               m_head{};    ///< The index of the front element.
        std::size_t               m_tail{};    ///< The index of the next element to be inserted.
        std::size_t               m_size{};    ///< The number of elements in the queue.

    public:
        CircularQueue() = default;

        /**
         * @brief Converting constructor.
         * Copies elements from a queue with a different capacity or overwrite policy.
         * Matches the destination's overwrite policy during the copy.
         */
        template <std::size_t tkOtherCapacity, bool tkOtherOverwrite>
        explicit constexpr CircularQueue(const CircularQueue<T, tkOtherCapacity, tkOtherOverwrite>& other)
        {
            for (std::size_t i = 0; i < other.size(); ++i)
            {
                // other[i] uses the relative index logic you already implemented
                this->push(other[i]);
            }
        }

        /**
         * @brief Converting move constructor.
         * Moves elements from a queue with a different capacity or overwrite policy.
         */
        template <std::size_t tkOtherCapacity, bool tkOtherOverwrite>
        explicit constexpr CircularQueue(CircularQueue<T, tkOtherCapacity, tkOtherOverwrite>&& other)
        {
            for (std::size_t i = 0; i < other.size(); ++i)
            {
                this->push(std::move(other[i]));
            }
            other.clear();
        }

        /**
         * @brief Converting assignment operator.
         */
        template <std::size_t tkOtherCapacity, bool tkOtherOverwrite>
        constexpr CircularQueue& operator=(const CircularQueue<T, tkOtherCapacity, tkOtherOverwrite>& other)
        {
            if (reinterpret_cast<const void*>(this) != reinterpret_cast<const void*>(&other))
            {
                clear();
                for (std::size_t i = 0; i < other.size(); ++i)
                {
                    this->push(other[i]);
                }
            }
            return *this;
        }

        /**
         * @brief Converting move assignment operator.
         */
        template <std::size_t tkOtherCapacity, bool tkOtherOverwrite>
        constexpr CircularQueue& operator=(CircularQueue<T, tkOtherCapacity, tkOtherOverwrite>&& other)
        {
            if (reinterpret_cast<const void*>(this) != reinterpret_cast<const void*>(&other))
            {
                clear();
                for (std::size_t i = 0; i < other.size(); ++i)
                {
                    this->push(std::move(other[i]));
                }
                other.clear();
            }
            return *this;
        }

        /**
         * @brief Accesses the front element of the queue.
         * @return A reference to the front element.
         * @note Does not throw exceptions; call only when queue is not empty.
         */
        constexpr T& front() noexcept
        {
            return m_buffer[m_head];
        }

        /**
         * @brief Accesses the front element of the queue (const version).
         * @return A const reference to the front element.
         * @note Does not throw exceptions; call only when queue is not empty.
         */
        constexpr const T& front() const noexcept
        {
            return m_buffer[m_head];
        }

        /**
         * @brief Accesses the back element of the queue.
         * @return A reference to the back element.
         * @note Does not throw exceptions; call only when queue is not empty.
         */
        constexpr T& back() noexcept
        {
            return m_buffer[(m_tail == 0) ? skCapacity - 1 : m_tail - 1];
        }

        /**
         * @brief Accesses the back element of the queue (const version).
         * @return A const reference to the back element.
         * @note Does not throw exceptions; call only when queue is not empty.
         */
        constexpr const T& back() const noexcept
        {
            return m_buffer[(m_tail == 0) ? skCapacity - 1 : m_tail - 1];
        }

        /**
         * @brief Checks if the queue is empty.
         * @return True if the queue is empty, false otherwise.
         */
        constexpr bool empty() const noexcept
        {
            return m_size == 0;
        }

        /**
         * @brief Checks if the queue is full.
         * @return True if the queue is full, false otherwise.
         */
        constexpr bool full() const noexcept
        {
            return size() == capacity();
        }

        /**
         * @brief Returns the number of elements in the queue.
         * @return The number of elements in the queue.
         */
        constexpr std::size_t size() const noexcept
        {
            return m_size;
        }

        /**
         * @brief Returns the capacity of the queue.
         * @return The capacity of the queue.
         */
        constexpr std::size_t capacity() const noexcept
        {
            return skCapacity;
        }

        /**
         * @brief Returns an iterator to the beginning of the queue.
         * @return A pointer to the first element of the queue.
         */
        constexpr IteratorType begin() noexcept
        {
            return IteratorType(this, m_head, 0);
        }

        /**
         * @brief Returns a const iterator to the beginning of the queue.
         * @return A const pointer to the first element of the queue.
         */
        constexpr ConstIteratorType begin() const noexcept
        {
            return ConstIteratorType(this, m_head, 0);
        }

        /**
         * @brief Returns an iterator to the end of the queue.
         * @return A pointer to the position after the last element of the queue.
         */
        constexpr IteratorType end() noexcept
        {
            return IteratorType(this, m_tail, m_size);
        }

        /**
         * @brief Returns a const iterator to the end of the queue.
         * @return A const pointer to the position after the last element of the queue.
         */
        constexpr ConstIteratorType end() const noexcept
        {
            return ConstIteratorType(this, m_tail, m_size);
        }

        /**
         * @brief Accesses an element in the queue by index.
         * @param index The index of the element to access.
         * @return A reference to the element at the specified index.
         * @note Index is relative to the front of the queue.
         */
        constexpr T& operator[](std::size_t index) noexcept
        {
            return m_buffer[(m_head + index) % skCapacity];
        }

        /**
         * @brief Accesses an element in the queue by index (const version).
         * @param index The index of the element to access.
         * @return A const reference to the element at the specified index.
         * @note Index is relative to the front of the queue.
         */
        constexpr const T& operator[](std::size_t index) const noexcept
        {
            return m_buffer[(m_head + index) % skCapacity];
        }

        /**
         * @brief Clears the queue, removing all elements.
         */
        constexpr void clear() noexcept
        {
            m_head = m_tail = m_size = 0;
        }

        /**
         * @brief Constructs an element in-place at the back of the queue.
         *
         * @tparam TArgs The types of the arguments for the constructor.
         * @param args The arguments to forward to the constructor of T.
         * @return True if the element was successfully added, false if the queue was full and overwriting is disabled.
         */
        template <typename... TArgs>
        [[nodiscard]] constexpr bool emplace(TArgs&&... args) noexcept
        {
            if (m_size == skCapacity)
            {
                if constexpr (tkOverwrite)
                {
                    discard();  // Remove the oldest element when overwriting is enabled
                }
                else
                {
                    return false;  // Queue is full
                }
            }

            m_buffer[m_tail] = T(std::forward<TArgs>(args)...);
            m_tail           = (m_tail + 1) % skCapacity;
            ++m_size;
            return true;
        }

        /**
         * @brief Adds an element to the back of the queue.
         * @param value The value to add to the queue.
         * @return True if the element was added successfully, false if the queue was full and overwriting is disabled.
         */
        [[nodiscard]] constexpr bool push(const T& value) noexcept
        {
            if (m_size == skCapacity)
            {
                if constexpr (tkOverwrite)
                {
                    discard();  // Remove the oldest element when overwriting is enabled
                }
                else
                {
                    return false;  // Queue is full
                }
            }

            m_buffer[m_tail] = value;
            m_tail           = (m_tail + 1) % skCapacity;
            ++m_size;
            return true;
        }

        /**
         * @brief Adds an element to the back of the queue (move version).
         * @param value The value to move into the queue.
         * @return True if the element was added successfully, false if the queue was full and overwriting is disabled.
         */
        [[nodiscard]] constexpr bool push(T&& value) noexcept
        {
            if (m_size == skCapacity)
            {
                if constexpr (tkOverwrite)
                {
                    discard();  // Remove the oldest element when overwriting is enabled
                }
                else
                {
                    return false;  // Queue is full
                }
            }

            m_buffer[m_tail] = std::move(value);
            m_tail           = (m_tail + 1) % skCapacity;
            ++m_size;
            return true;
        }

        /**
         * @brief Removes the front element from the queue.
         * @return True if an element was removed, false if the queue was empty.
         */
        [[nodiscard]] constexpr std::optional<T> pop() noexcept
        {
            if (empty())
            {
                return std::nullopt;
            }

            std::optional<T> value = std::move(m_buffer[m_head]);

            m_head = (m_head + 1) % skCapacity;
            --m_size;

            return value;
        }

        [[nodiscard]] constexpr bool discard() noexcept
        {
            if (empty())
            {
                return false;
            }

            m_head = (m_head + 1) % skCapacity;
            --m_size;

            return true;
        }

        /**
         * @brief Compares two circular queues for equality.
         * @param lhs The first queue.
         * @param rhs The second queue.
         * @return True if the two queues have the same elements in the same order, false otherwise.
         */
        friend constexpr bool operator==(const CircularQueue& lhs, const CircularQueue& rhs) noexcept
        {
            if (lhs.size() != rhs.size()) return false;
            return std::equal(lhs.begin(), lhs.end(), rhs.begin());
        }

        /**
         * @brief Compares two circular queues for inequality.
         * @param lhs The first queue.
         * @param rhs The second queue.
         * @return True if the two queues differ, false if they are equal.
         */
        friend constexpr bool operator!=(const CircularQueue& lhs, const CircularQueue& rhs) noexcept
        {
            return !(lhs == rhs);
        }
    };

}  // namespace valle