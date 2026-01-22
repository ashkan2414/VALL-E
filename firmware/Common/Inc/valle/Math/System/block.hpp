#pragma once

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <memory>
#include <type_traits>
#include <variant>

#include "Valle/Utils/template_utils.hpp"
#include "Valle/platform.h"

//==============================================================================
// Metaprogramming Helpers (Flattening Logic)
//==============================================================================

namespace detail
{

    // A sequence of integers (using int instead of bool for clearer 1 vs -1)
    template <int... Is>
    using SignSequence = std::integer_sequence<int, Is...>;

    // Helper: Negate a sequence ( 1, -1 ) -> ( -1, 1 )
    template <typename Seq>
    struct negate_sequence;

    template <int... Is>
    struct negate_sequence<SignSequence<Is...>>
    {
        using type = SignSequence<(-Is)...>;
    };

    // Helper: Concatenate sequences
    template <typename S1, typename S2>
    struct cat_sequence;

    template <int... I1, int... I2>
    struct cat_sequence<SignSequence<I1...>, SignSequence<I2...>>
    {
        using type = SignSequence<I1..., I2...>;
    };

    // Helper to detect reference_wrapper
    template <typename T>
    struct is_reference_wrapper : std::false_type
    {
    };
    template <typename U>
    struct is_reference_wrapper<std::reference_wrapper<U>> : std::true_type
    {
    };
    template <typename T>
    inline constexpr bool is_reference_wrapper_v = is_reference_wrapper<T>::value;

    template <typename T>
    struct unwrap_ref_wrapper
    {
        using type = T;
    };

    template <typename T>
    struct unwrap_ref_wrapper<std::reference_wrapper<T>>
    {
        using type = T;
    };

    // Helper alias that also strips const/volatile/&
    // (Necessary because reference_wrapper might be passed as const&)
    template <typename T>
    using unwrap_ref_wrapper_t = typename unwrap_ref_wrapper<std::remove_cvref_t<T>>::type;

    template <typename T>
    constexpr unwrap_ref_wrapper_t<T> unwrap(T &&obj) noexcept
    {
        if constexpr (detail::is_reference_wrapper_v<T>)
        {
            return obj.get();
        }
        else
        {
            return std::forward<T>(obj);
        }
    }

}  // namespace detail

//==============================================================================
// The Pure Virtual Interface (Dynamic)
//==============================================================================
// Allows storage in std::vector<ISystemBlockBase<T>*>
template <typename T>
class ISystemBlockBase
{
public:
    using CSystemTag = void;  // Marker type

    virtual ~ISystemBlockBase() = default;

    // The virtual entry point
    virtual T    process_dyn(const T input) = 0;
    virtual void reset_dyn()                = 0;

    virtual std::shared_ptr<ISystemBlockBase<T>> clone() const = 0;
};

template <typename T>
using SystemPtr = std::shared_ptr<ISystemBlockBase<T>>;

template <typename T>
concept CSystem = requires { typename T::CSystemTag; };

template <typename T>
concept CSystemOrRef = CSystem<detail::unwrap_ref_wrapper_t<T>>;

template <typename T>
concept CCascadedSystem = requires { typename T::CascadedSystemTag; };

template <typename T>
concept CParallelSystem = requires { typename T::ParallelSystemTag; };

// Forward declarations
template <typename TDerived, typename T>
class ISystemBlock;
template <typename T, CSystemOrRef... TSystems>
class CascadedSystemBlock;
template <typename T, typename TSigns, CSystemOrRef... TSystems>
class ParallelSystemBlock;

template <typename T>
class DynamicCascadedSystemBlock;
template <typename T>
class DynamicParallelSystemBlock;
template <typename T>
class DynamicSystemPipeline;

template <typename T>
class Gain;
template <typename T>
class Bias;

template <typename TSystem>
struct CascadedSystemTraits
{
    // Case 1: It IS a CascadedSystemBlock
    // Return: The internal tuple
    static auto get_tuple(const TSystem &sys)
        requires CCascadedSystem<TSystem>
    {
        return sys.get_systems();
    }

    // Case 2: It is a Leaf System
    // Return: Tuple<Leaf>
    static auto get_tuple(const TSystem &sys)
        requires(!CCascadedSystem<TSystem>)
    {
        return std::make_tuple(sys);
    }
};

template <typename TSystem>
struct ParallelSystemTraits
{
    // Case 1: It IS a ParallelSystemBlock
    // Return: The internal tuple, and the internal signs
    static auto get_tuple(const TSystem &sys)
        requires CParallelSystem<TSystem>
    {
        return sys.get_systems();
    }

    static auto get_signs()
        requires CParallelSystem<TSystem>
    {
        // Extract the TSigns template argument logic here is tricky
        // Cleaner to ask the class itself or deduce it.
        return typename std::decay_t<TSystem>::SignT{};
    }

    // Case 2: It is a Leaf System
    // Return: Tuple<Leaf>, Signs<1>
    static auto get_tuple(const TSystem &sys)
        requires(!CParallelSystem<TSystem>)
    {
        return std::make_tuple(sys);
    }
    static auto get_signs()
        requires(!CParallelSystem<TSystem>)
    {
        return detail::SignSequence<1>{};
    }
};

//==============================================================================
// CRTP base interface
//==============================================================================

template <typename TDerived, typename T>
class ISystemBlock : public ISystemBlockBase<T>
{
public:
    using ValueT = T;

    inline T process(const T input)
    {
        return static_cast<TDerived *>(this)->process_impl(input);
    }

    inline void process(const T *input, T *output, uint32_t n)
    {
        for (uint32_t i = 0; i < n; ++i)
        {
            output[i] = process(input[i]);
        }
    }

    inline void reset()
    {
        static_cast<TDerived *>(this)->reset_impl();
    }

    // Satisfies ISystemBlockBase. Redirects to the static impl.
    T process_dyn(const T input) override final
    {
        return process(input);
    }

    void reset_dyn() override final
    {
        reset();
    }

    std::shared_ptr<ISystemBlockBase<T>> clone() const override
    {
        const auto &derived = *static_cast<const TDerived *>(this);
        return std::make_shared<TDerived>(derived);
    }

    // ============================================================
    // Operator Overloads
    // ============================================================

    // --- Multiplication (Cascade) ---

    // Case 1: Standard System (Value or const&)
    template <CSystem TRHS>
    auto operator*(const ISystemBlock<TRHS, T> &rhs) const
    {
        return combine_cascade(*static_cast<const TDerived *>(this), *static_cast<const TRHS *>(&rhs));
    }

    // Case 2: Reference Wrapper
    template <typename TRHS>
    auto operator*(std::reference_wrapper<TRHS> rhs) const
    {
        return combine_cascade(*static_cast<const TDerived *>(this), rhs);
    }

    template <typename U, CSystem TLHS, typename TRHS>
    friend auto operator*(std::reference_wrapper<TLHS> lhs, const ISystemBlock<TRHS, U> &rhs);

    template <CSystem TLHS, CSystem TRHS>
    friend auto operator*(std::reference_wrapper<TLHS> lhs, std::reference_wrapper<TRHS> rhs);

    // --- Addition (Parallel) ---

    template <CSystem TRHS>
    auto operator+(const ISystemBlock<TRHS, T> &rhs) const
    {
        return combine_parallel<1>(*static_cast<const TDerived *>(this), *static_cast<const TRHS *>(&rhs));
    }

    template <typename TRHS>
    auto operator+(std::reference_wrapper<TRHS> rhs) const
    {
        return combine_parallel<1>(*static_cast<const TDerived *>(this), rhs);
    }

    template <typename U, CSystem TLHS, typename TRHS>
    friend auto operator+(std::reference_wrapper<TLHS> lhs, const ISystemBlock<TRHS, U> &rhs);

    template <CSystem TLHS, CSystem TRHS>
    friend auto operator+(std::reference_wrapper<TLHS> lhs, std::reference_wrapper<TRHS> rhs);

    // --- Subtraction (Parallel) ---

    template <CSystem TRHS>
    auto operator-(const ISystemBlock<TRHS, T> &rhs) const
    {
        return combine_parallel<-1>(*static_cast<const TDerived *>(this), *static_cast<const TRHS *>(&rhs));
    }

    template <CSystem TRHS>
    auto operator-(std::reference_wrapper<TRHS> rhs) const
    {
        return combine_parallel<-1>(*static_cast<const TDerived *>(this), rhs);
    }

    template <typename U, CSystem TLHS, typename TRHS>
    friend auto operator-(std::reference_wrapper<TLHS> lhs, const ISystemBlock<TRHS, U> &rhs);

    template <CSystem TLHS, CSystem TRHS>
    friend auto operator-(std::reference_wrapper<TLHS> lhs, std::reference_wrapper<TRHS> rhs);

    // --- Scalar Operations ---

    template <typename U>
    auto operator*(const U scalar) const
        requires std::is_convertible_v<U, T>
    {
        return *this * Gain<T>(static_cast<T>(scalar));
    }

    template <typename U>
    auto operator/(const U scalar) const
        requires std::is_convertible_v<U, T>
    {
        return *this * Gain<T>(static_cast<T>(1) / static_cast<T>(scalar));
    }

    template <typename U>
    auto operator+(const U scalar) const
        requires std::is_convertible_v<U, T>
    {
        return *this * Bias<T>(static_cast<T>(scalar));
    }

    template <typename U>
    auto operator-(const U scalar) const
        requires std::is_convertible_v<U, T>
    {
        return *this * Bias<T>(-static_cast<T>(scalar));
    }

    // unary minus
    auto operator-() const
    {
        return Gain<T>(static_cast<T>(-1)) * (*this);
    }

private:
    // Logic for Multiplication (Cascade)
    // Accepts either a Concrete System (const TRHS&) or a Wrapper (reference_wrapper<TRHS>)
    template <CSystemOrRef TLHS, CSystemOrRef TRHS>
    static auto combine_cascade(TLHS &lhs, TRHS &rhs)
    {
        // LHS Tuple (Unwrap if System, Keep as Leaf if Wrapper)
        auto lhs_tuple = [&]()
        {
            if constexpr (detail::is_reference_wrapper_v<TLHS>)
            {
                return std::tuple<TLHS>(TLHS(lhs));  // Do NOT flatten wrappers, treat as leaf
            }
            else
            {
                return CascadedSystemTraits<TLHS>::get_tuple(lhs);
            }
        }();

        // RHS Tuple (Unwrap if System, Keep as Leaf if Wrapper)
        auto rhs_tuple = [&]()
        {
            if constexpr (detail::is_reference_wrapper_v<TRHS>)
            {
                return std::tuple<TRHS>(TRHS(rhs));  // Do NOT flatten wrappers, treat as leaf
            }
            else
            {
                return CascadedSystemTraits<TRHS>::get_tuple(rhs);
            }
        }();

        auto combined_tuple = std::tuple_cat(lhs_tuple, rhs_tuple);

        return std::apply(
            [&](auto &&...systems)
            {
                // std::decay_t preserves reference_wrapper type but removes const& from standard objects
                using NewSystemT = CascadedSystemBlock<T, std::decay_t<decltype(systems)>...>;

                return NewSystemT(T{}, std::forward<decltype(systems)>(systems)...);
            },
            std::move(combined_tuple));
    }

    // Logic for Addition/Subtraction (Parallel)
    // Accepts either a Concrete System or a Wrapper
    template <int tkMult, CSystemOrRef TLHS, CSystemOrRef TRHS>
    static auto combine_parallel(TLHS &lhs, TRHS &rhs)
    {
        // 1. Get LHS Tuples & Signs
        auto lhs_tuple = [&]()
        {
            if constexpr (detail::is_reference_wrapper_v<TLHS>)
            {
                return std::tuple<TLHS>(TLHS(lhs));  // Do NOT flatten wrappers, treat as leaf
            }
            else
            {
                return CascadedSystemTraits<TLHS>::get_tuple(lhs);
            }
        }();
        using LHSSignsT = std::conditional_t<detail::is_reference_wrapper_v<TLHS>,
                                             detail::SignSequence<1>,  // Default sign for opaque wrapper
                                             decltype(ParallelSystemTraits<TLHS>::get_signs())>;

        // 2. Get RHS Tuples & Signs
        // If wrapper: Tuple is (wrapper), Sign is 1.
        // If system:  Tuple is flattened, Sign is extracted from traits.
        auto rhs_tuple = [&]()
        {
            if constexpr (detail::is_reference_wrapper_v<TRHS>)
            {
                return std::tuple<TRHS>(TRHS(rhs));
            }
            else
            {
                return ParallelSystemTraits<TRHS>::get_tuple(rhs);
            }
        }();

        using RHSSignsRawT = std::conditional_t<detail::is_reference_wrapper_v<TRHS>,
                                                detail::SignSequence<1>,  // Default sign for opaque wrapper
                                                decltype(ParallelSystemTraits<TRHS>::get_signs())>;

        // 3. Negate RHS signs if this is subtraction
        using RHSSignsT =
            std::conditional_t<tkMult == 1, RHSSignsRawT, typename detail::negate_sequence<RHSSignsRawT>::type>;

        // 4. Concatenate
        using FinalSignsT   = typename detail::cat_sequence<LHSSignsT, RHSSignsT>::type;
        auto combined_tuple = std::tuple_cat(lhs_tuple, rhs_tuple);

        return std::apply(
            [&](auto &&...systems)
            {
                using NewSystemT = ParallelSystemBlock<T, FinalSignsT, std::decay_t<decltype(systems)>...>;

                return NewSystemT(T{}, FinalSignsT{}, std::forward<decltype(systems)>(systems)...);
            },
            std::move(combined_tuple));
    }
};

template <typename T, CSystem TLHS, typename TRHS>
auto operator*(std::reference_wrapper<TLHS> lhs, const ISystemBlock<TRHS, T> &rhs)
{
    return TLHS::combine_cascade(lhs, rhs);
}

template <CSystem TLHS, CSystem TRHS>
auto operator*(std::reference_wrapper<TLHS> lhs, std::reference_wrapper<TRHS> rhs)
{
    return TLHS::combine_cascade(lhs, rhs);
}

template <typename T, CSystem TLHS, CSystem TRHS>
auto operator+(std::reference_wrapper<TLHS> lhs, const ISystemBlock<TRHS, T> &rhs)
{
    return TLHS::template combine_parallel<1>(lhs, rhs);
}

template <CSystem TLHS, CSystem TRHS>
auto operator+(std::reference_wrapper<TLHS> lhs, std::reference_wrapper<TRHS> rhs)
{
    return TLHS::template combine_parallel<1>(lhs, rhs);
}

template <typename T, typename TLHS, typename TRHS>
auto operator-(std::reference_wrapper<TLHS> lhs, const ISystemBlock<TRHS, T> &rhs)
{
    return TLHS::template combine_parallel<-1>(lhs, rhs);
}

template <CSystem TLHS, CSystem TRHS>
auto operator-(std::reference_wrapper<TLHS> lhs, std::reference_wrapper<TRHS> rhs)
{
    return TLHS::template combine_parallel<-1>(lhs, rhs);
}

//==============================================================================
// Cascaded System (Series: Input -> A -> B -> Output)
//==============================================================================
template <typename T, CSystemOrRef... TSystems>
class CascadedSystemBlock : public ISystemBlock<CascadedSystemBlock<T, TSystems...>, T>
{
public:
    using CascadedSystemTag = void;  // Marker type

private:
    std::tuple<TSystems...> m_systems;

public:
    // Standard Constructor
    explicit CascadedSystemBlock(TSystems... systems) : m_systems(std::move(systems)...)
    {
    }

    // Constructor Helper inside operator*
    // We pass a dummy T to differentiate this constructor or strict tuple handling
    explicit CascadedSystemBlock(T /* dummy_val */, TSystems... systems) : m_systems(std::move(systems)...)
    {
    }

    // Accessor for flattening logic
    const std::tuple<TSystems...> &get_systems() const
    {
        return m_systems;
    }

    inline T process_impl(const T input)
    {
        T output = input;
        std::apply(
            [&](auto &...system)
            {
                // Unary left fold
                ((output = detail::unwrap(system).process(output)), ...);
            },
            m_systems);
        return output;
    }

    inline void reset_impl()
    {
        std::apply([](auto &...system) { (detail::unwrap(system).reset(), ...); }, m_systems);
    }
};

//==============================================================================
// Parallel System (Sum: Input -> A+B -> Output)
//==============================================================================
template <typename T, int... tkSigns, CSystemOrRef... TSystems>
    requires(sizeof...(tkSigns) == sizeof...(TSystems))
class ParallelSystemBlock<T, detail::SignSequence<tkSigns...>, TSystems...>
    : public ISystemBlock<ParallelSystemBlock<T, detail::SignSequence<tkSigns...>, TSystems...>, T>

{
public:
    using ParallelSystemTag = void;  // Marker type

private:
    std::tuple<TSystems...> m_systems;

public:
    explicit ParallelSystemBlock(TSystems... systems) : m_systems(std::move(systems)...)
    {
    }

    // Constructor Helper inside operator*
    // We pass a dummy T to differentiate this constructor or strict tuple handling
    explicit ParallelSystemBlock(T /* dummy_val */,
                                 detail::SignSequence<tkSigns...> /* dummy_signs */,
                                 TSystems... systems)
        : m_systems(std::move(systems)...)
    {
    }

    const std::tuple<TSystems...> &get_systems() const
    {
        return m_systems;
    }

    inline T process_impl(const T input)
    {
        T sum = 0;
        process_fold(sum, input, std::index_sequence_for<TSystems...>{});
        return sum;
    }

    inline void reset_impl()
    {
        std::apply([](auto &...system) { (detail::unwrap(system).reset(), ...); }, m_systems);
    }

private:
    template <std::size_t... Is>
    inline void process_fold(T &sum, const T input, std::index_sequence<Is...>)
    {
        // Fold expression using the helper
        ((sum += (get_sign<Is>() == 1 ? detail::unwrap(std::get<Is>(m_systems)).process(input)
                                      : -detail::unwrap(std::get<Is>(m_systems)).process(input))),
         ...);
    }

    // Retrieve sign for index I
    template <std::size_t I>
    static constexpr int get_sign()
    {
        constexpr int signs_array[] = {tkSigns...};
        return signs_array[I];
    }
};

#ifdef HEAP_MEMORY_ENABLED

//==============================================================================
// Dynamic Cascaded System System
//==============================================================================

template <typename T>
class DynamicCascadedSystemBlock : public ISystemBlock<DynamicCascadedSystemBlock<T>, T>
{
public:
    using SystemPtrT = SystemPtr<T>;
    using VectorT    = std::vector<SystemPtrT>;

private:
    // We store pointers to the interface.
    // Ownership is external (systems usually live in main() or a factory).
    std::shared_ptr<VectorT> m_systems;

public:
    inline DynamicCascadedSystemBlock() : m_systems(std::make_shared<VectorT>())
    {
    }
    inline DynamicCascadedSystemBlock(SystemPtrT sys) : m_systems(std::make_shared<VectorT>())
    {
        append(sys);
    }

    template <CSystem TSystem>
    inline DynamicCascadedSystemBlock(const TSystem &sys) : m_systems(std::make_shared<VectorT>())
    {
        append(sys);
    }

    template <CSystem TSystem>
    inline DynamicCascadedSystemBlock(TSystem &&sys) : m_systems(std::make_shared<VectorT>())
    {
        append(std::move(sys));
    }

    inline void append(SystemPtrT sys)
    {
        m_systems->push_back(sys);
    }

    template <CSystem TSystem>
    inline void append(const TSystem &sys)
    {
        append(std::make_shared<TSystem>(sys));
    }

    template <std::ranges::range R>
    inline void append(R &&systems)
    {
        m_systems->insert(m_systems->end(), std::begin(systems), std::end(systems));
    }

    inline void append(const DynamicCascadedSystemBlock &sys)
    {
        append(*sys.m_systems);
    }
    inline void append(DynamicCascadedSystemBlock &&sys)
    {
        append(std::move(*sys.m_systems));
    }

    inline void append_copy(SystemPtrT sys)
    {
        append(sys->clone());
    }

    template <std::ranges::range R>
    inline void append_copy(R &&s)
    {
        for (auto &sys : s)
        {
            append_copy(sys);
        }
    }

    inline void append_copy(const DynamicCascadedSystemBlock &sys)
    {
        append_copy(*sys.m_systems);
    }
    inline void append_copy(DynamicCascadedSystemBlock &&sys)
    {
        append_copy(*sys.m_systems);
    }

    inline void append_front(SystemPtrT sys)
    {
        m_systems->insert(m_systems->begin(), sys);
    }

    template <CSystem TSystem>
    inline void append_front(const TSystem &sys)
    {
        append_front(std::make_shared<TSystem>(sys));
    }

    template <std::ranges::range R>
    inline void append_front(R &&s)
        requires std::is_convertible_v<typename R::value_type, SystemPtrT>
    {
        m_systems->insert(m_systems->front(), std::begin(s), std::end(s));
    }

    inline void append_front(const DynamicCascadedSystemBlock &sys)
    {
        append_front(*sys.m_systems);
    }
    inline void append_front(DynamicCascadedSystemBlock &&sys)
    {
        append_front(std::move(*sys.m_systems));
    }

    inline void append_front_copy(SystemPtrT sys)
    {
        append_front_copy(*sys);
    }
    template <std::ranges::range R>
    inline void append_front_copy(R &&s)
        requires std::is_convertible_v<typename R::value_type, SystemPtrT>
    {
        for (auto &sys : s)
        {
            append_front_copy(sys);
        }
    }

    inline void append_front_copy(const DynamicCascadedSystemBlock &sys)
    {
        append_front_copy(*sys.m_systems);
    }
    inline void append_front_copy(DynamicCascadedSystemBlock &&sys)
    {
        append_front_copy(*sys.m_systems);
    }

    inline void clear()
    {
        m_systems->clear();
    }

    [[nodiscard]] inline DynamicCascadedSystemBlock copy() const
    {
        DynamicCascadedSystemBlock result;
        result.append_copy(*this);
        return result;
    }

    // --- CRTP Implementation ---

    inline T process_impl(T input)
    {
        // Iterate over the shared vector
        for (const auto &sys : *m_systems)
        {
            input = sys->process_dyn(input);
        }
        return input;
    }

    inline void reset_impl()
    {
        for (const auto &sys : *m_systems)
        {
            sys->reset_dyn();
        }
    }

    // Helper to allow this to act like a collection for basic iteration if needed
    inline auto begin()
    {
        return m_systems->begin();
    }
    inline auto end()
    {
        return m_systems->end();
    }

    // Cascaded system operators
    inline DynamicCascadedSystemBlock<T> &operator*=(const DynamicCascadedSystemBlock<T> &sys)
    {
        append_copy(*sys.m_systems);
        return *this;
    }

    // Concrete system operators
    template <CSystem TSystem>
    inline DynamicCascadedSystemBlock<T> &operator*=(const TSystem &sys)
    {
        append(sys);
        return *this;
    }

    template <CSystem TSystem>
    [[nodiscard]] inline DynamicCascadedSystemBlock<T> operator*(const TSystem &sys) const
        requires(!std::same_as<TSystem, DynamicCascadedSystemBlock>)
    {
        DynamicCascadedSystemBlock<T> result = copy();
        result *= sys;
        return result;
    }

    template <CSystem TSystem>
    [[nodiscard]] inline DynamicParallelSystemBlock<T> operator+(const TSystem &sys) const
    {
        DynamicParallelSystemBlock<T> result;
        result.append(*this);
        result += sys;
        return result;
    }

    template <CSystem TSystem>
    [[nodiscard]] inline DynamicParallelSystemBlock<T> operator-(const TSystem &sys) const
    {
        DynamicParallelSystemBlock<T> result;
        result.append(*this);
        result -= sys;
        return result;
    }

    // System ptr Operators
    inline DynamicCascadedSystemBlock<T> &operator*=(const SystemPtrT sys)
    {
        append(sys);
        return *this;
    }

    [[nodiscard]] inline DynamicCascadedSystemBlock<T> operator*(const SystemPtrT sys) const
    {
        DynamicCascadedSystemBlock<T> result = copy();
        result.append(sys);
        return result;
    }

    [[nodiscard]] inline DynamicParallelSystemBlock<T> operator+(const SystemPtrT sys) const
    {
        DynamicParallelSystemBlock<T> result;
        result.append(*this);
        result.append(sys);
        return result;
    }

    [[nodiscard]] inline DynamicParallelSystemBlock<T> operator-(const SystemPtrT sys) const
    {
        DynamicParallelSystemBlock<T> result;
        result.append(*this);
        result.append(sys, -1.0);
        return result;
    }

    // Scalar Operations
    template <typename U>
    inline DynamicCascadedSystemBlock<T> &operator+=(const U scalar)
        requires std::is_convertible_v<U, T>
    {
        *this *= std::make_shared<Bias<T>>(static_cast<T>(scalar));
        return *this;
    }

    template <typename U>
    inline DynamicCascadedSystemBlock<T> &operator-=(const U scalar)
        requires std::is_convertible_v<U, T>
    {
        *this *= std::make_shared<Bias<T>>(static_cast<T>(scalar));
        return *this;
    }

    template <typename U>
    inline DynamicCascadedSystemBlock<T> &operator*=(const U scalar)
        requires std::is_convertible_v<U, T>
    {
        *this *= std::make_shared<Gain<T>>(static_cast<T>(scalar));
        return *this;
    }

    template <typename U>
    inline DynamicCascadedSystemBlock<T> &operator/=(const U scalar)
        requires std::is_convertible_v<U, T>
    {
        *this *= static_cast<T>(1) / static_cast<T>(scalar);
        return *this;
    }

    template <typename U>
    [[nodiscard]] inline DynamicCascadedSystemBlock<T> operator+(const U scalar) const
        requires std::is_convertible_v<U, T>
    {
        DynamicCascadedSystemBlock<T> result = copy();
        result += scalar;
        return result;
    }

    template <typename U>
    [[nodiscard]] inline DynamicCascadedSystemBlock<T> operator-(const U scalar) const
        requires std::is_convertible_v<U, T>
    {
        return *this + (-static_cast<T>(scalar));
    }

    template <typename U>
    [[nodiscard]] inline DynamicCascadedSystemBlock<T> operator*(const U scalar) const
        requires std::is_convertible_v<U, T>
    {
        DynamicCascadedSystemBlock<T> result = copy();
        result *= scalar;
        return result;
    }

    template <typename U>
    [[nodiscard]] inline DynamicCascadedSystemBlock<T> operator/(const U scalar) const
        requires std::is_convertible_v<U, T>
    {
        return *this * (static_cast<T>(1) / static_cast<T>(scalar));
    }

    // unary minus
    [[nodiscard]] inline DynamicCascadedSystemBlock<T> operator-() const
    {
        DynamicCascadedSystemBlock<T> result = copy();
        result.append_front(std::make_shared<Gain<T>>(static_cast<T>(-1)));
        return result;
    }

    // friend scalar operators
    template <typename U, typename V>
    friend DynamicCascadedSystemBlock<V> operator+(const U scalar, const DynamicCascadedSystemBlock<V> &sys)
        requires std::is_convertible_v<U, V>;
    template <typename U, typename V>
    friend DynamicCascadedSystemBlock<V> operator-(const U scalar, const DynamicCascadedSystemBlock<V> &sys)
        requires std::is_convertible_v<U, V>;
    template <typename U, typename V>
    friend DynamicCascadedSystemBlock<V> operator*(const U scalar, const DynamicCascadedSystemBlock<V> &sys)
        requires std::is_convertible_v<U, V>;
};

template <typename U, typename V>
DynamicCascadedSystemBlock<V> operator+(const U scalar, const DynamicCascadedSystemBlock<V> &sys)
    requires std::is_convertible_v<U, V>
{
    DynamicCascadedSystemBlock<V> result = sys.copy();
    result.append_front(std::make_shared<Bias<V>>(static_cast<V>(scalar)));
    return result;
}

template <typename U, typename V>
DynamicCascadedSystemBlock<V> operator-(const U scalar, const DynamicCascadedSystemBlock<V> &sys)
    requires std::is_convertible_v<U, V>
{
    return scalar + (-sys);
}

template <typename U, typename V>
DynamicCascadedSystemBlock<V> operator*(const U scalar, const DynamicCascadedSystemBlock<V> &sys)
    requires std::is_convertible_v<U, V>
{
    DynamicCascadedSystemBlock<V> result = sys.copy();
    result.append_front(std::make_shared<Gain<V>>(static_cast<V>(scalar)));
    return result;
}

//==============================================================================
// Dynamic Parallel System
//==============================================================================
template <typename T>
class DynamicParallelSystemBlock : public ISystemBlock<DynamicParallelSystemBlock<T>, T>
{
public:
    using SystemPtrT = SystemPtr<T>;
    using PairT      = std::pair<SystemPtrT, T>;
    using VectorT    = std::vector<PairT>;

private:
    // We store pointers to the interface.
    // Ownership is external (systems usually live in main() or a factory).
    std::shared_ptr<VectorT> m_systems;

public:
    inline DynamicParallelSystemBlock() : m_systems(std::make_shared<VectorT>())
    {
    }

    inline DynamicParallelSystemBlock(SystemPtrT sys, const T gain = static_cast<T>(1.0))
        : m_systems(std::make_shared<VectorT>())
    {
        append(sys, gain);
    }

    inline void append(PairT sys)
    {
        m_systems->push_back(sys);
    }
    inline void append(SystemPtrT sys, const T gain = 1.0)
    {
        append(std::make_pair(sys, gain));
    }

    template <CSystem TSystem>
    inline void append(const TSystem &sys, const T gain = 1.0)
    {
        append(std::make_shared<TSystem>(sys), gain);
    }

    template <std::ranges::range R>
    inline void append(R &&systems)
        requires std::is_convertible_v<typename R::value_type, PairT>
    {
        m_systems->insert(m_systems->end(), std::begin(systems), std::end(systems));
    }

    template <std::ranges::range R>
    inline void append(R &&systems, const T gain = 1.0)
        requires std::is_convertible_v<typename R::value_type, SystemPtrT>
    {
        for (auto &sys : systems)
        {
            append(sys, gain);
        }
    }

    inline void append(const DynamicParallelSystemBlock &sys)
    {
        append(*sys.m_systems);
    }
    inline void append(DynamicParallelSystemBlock &&sys)
    {
        append(std::move(*sys.m_systems));
    }

    inline void append_copy(SystemPtrT sys, const T gain = 1.0)
    {
        append(sys->clone(), gain);
    }
    inline void append_copy(PairT sys)
    {
        append_copy(sys.first, sys.second);
    }

    template <std::ranges::range R>
    inline void append_copy(R &&s)
    {
        for (auto &sys : s)
        {
            append_copy(sys);
        }
    }

    template <std::ranges::range R>
    inline void append_copy(R &&systems, const T gain = 1.0)
        requires std::is_convertible_v<typename R::value_type, SystemPtrT>
    {
        for (auto &sys : systems)
        {
            append_copy(sys, gain);
        }
    }

    inline void append_copy(const DynamicParallelSystemBlock &sys)
    {
        append_copy(*sys.m_systems);
    }
    inline void append_copy(DynamicParallelSystemBlock &&sys)
    {
        append_copy(*sys.m_systems);
    }

    inline void append_front(PairT sys)
    {
        m_systems->insert(m_systems->begin(), sys);
    }
    inline void append_front(SystemPtrT sys, const T gain = 1.0)
    {
        append_front(std::make_pair(sys, gain));
    }

    template <CSystem TSystem>
    inline void append_front(const TSystem &sys, const T gain = 1.0)
    {
        append_front(std::make_shared<TSystem>(sys), gain);
    }

    template <std::ranges::range R>
    inline void append_front(R &&s)
        requires std::is_convertible_v<typename R::value_type, PairT>
    {
        m_systems->insert(m_systems->front(), std::begin(s), std::end(s));
    }

    template <std::ranges::range R>
    inline void append_front(R &&systems, const T gain = 1.0)
        requires std::is_convertible_v<typename R::value_type, SystemPtrT>
    {
        for (auto &sys : systems)
        {
            append_front(sys, gain);
        }
    }

    inline void append_front(const DynamicParallelSystemBlock &sys)
    {
        append_front(*sys.m_systems);
    }
    inline void append_front(DynamicParallelSystemBlock &&sys)
    {
        append_front(std::move(*sys.m_systems));
    }

    inline void append_front_copy(SystemPtrT sys, const T gain = 1.0)
    {
        append_front_copy(*sys, gain);
    }
    template <std::ranges::range R>
    inline void append_front_copy(R &&s)
        requires std::is_convertible_v<typename R::value_type, PairT>
    {
        for (auto &sys : s)
        {
            append_front_copy(sys);
        }
    }

    template <std::ranges::range R>
    inline void append_front_copy(R &&systems, const T gain = 1.0)
        requires std::is_convertible_v<typename R::value_type, SystemPtrT>
    {
        for (auto &sys : systems)
        {
            append_front_copy(sys, gain);
        }
    }

    inline void append_front_copy(const DynamicParallelSystemBlock &sys)
    {
        append_front_copy(*sys.m_systems);
    }
    inline void append_front_copy(DynamicParallelSystemBlock &&sys)
    {
        append_front_copy(*sys.m_systems);
    }

    inline void clear()
    {
        m_systems->clear();
    }

    [[nodiscard]] inline DynamicParallelSystemBlock copy() const
    {
        DynamicParallelSystemBlock result;
        result.append_copy(*this);
        return result;
    }

    // --- CRTP Implementation ---

    inline T process_impl(const T input)
    {
        T sum = 0;
        // Iterate over the shared vector
        for (const auto &sys : *m_systems)
        {
            sum += sys.first->process_dyn(input) * sys.second;
        }
        return sum;
    }

    inline void reset_impl()
    {
        for (const auto &sys : *m_systems)
        {
            sys.first->reset_dyn();
        }
    }

    // Helper to allow this to act like a collection for basic iteration if needed
    inline auto begin()
    {
        return m_systems->begin();
    }
    inline auto end()
    {
        return m_systems->end();
    }

    // Parallel System operators
    inline DynamicParallelSystemBlock<T> &operator+=(const DynamicParallelSystemBlock<T> &sys)
    {
        append(*sys.m_systems);
        return *this;
    }

    inline DynamicParallelSystemBlock<T> &operator-=(const DynamicParallelSystemBlock<T> &sys)
    {
        *this += (-sys);
        return *this;
    }

    // Concrete system operators
    template <CSystem TSystem>
    inline DynamicParallelSystemBlock<T> &operator+=(const TSystem &sys)
    {
        append(sys, 1.0);
        return *this;
    }

    template <CSystem TSystem>
    inline DynamicParallelSystemBlock<T> &operator-=(const TSystem &sys)
    {
        append(sys, -1.0);
        return *this;
    }

    template <CSystem TSystem>
    [[nodiscard]] inline DynamicCascadedSystemBlock<T> operator*(const TSystem &sys) const
    {
        DynamicCascadedSystemBlock<T> result;
        result.append(*this);
        result *= sys;
        return result;
    }

    template <CSystem TSystem>
    [[nodiscard]] inline DynamicParallelSystemBlock<T> operator+(const TSystem &sys) const
    {
        DynamicParallelSystemBlock<T> result = copy();
        result += sys;
        return result;
    }

    template <CSystem TSystem>
    [[nodiscard]] inline DynamicParallelSystemBlock<T> operator-(const TSystem &sys) const
    {
        DynamicParallelSystemBlock<T> result = copy();
        result -= sys;
        return result;
    }

    // System Ptr Operators
    inline DynamicParallelSystemBlock<T> &operator+=(const SystemPtrT sys)
    {
        append(sys, true);
        return *this;
    }

    inline DynamicParallelSystemBlock<T> &operator-=(const SystemPtrT sys)
    {
        append(sys, false);
        return *this;
    }

    [[nodiscard]] inline DynamicParallelSystemBlock<T> operator+(const SystemPtrT sys) const
    {
        DynamicParallelSystemBlock<T> result = copy();
        result += sys;
        return result;
    }

    [[nodiscard]] inline DynamicParallelSystemBlock<T> operator-(const SystemPtrT sys) const
    {
        DynamicParallelSystemBlock<T> result = copy();
        result -= sys;
        return result;
    }

    [[nodiscard]] inline DynamicCascadedSystemBlock<T> operator*(const SystemPtrT sys) const
    {
        DynamicCascadedSystemBlock<T> result;
        result.append(*this);
        result.append(sys);
        return result;
    }

    // Scalar Operations
    template <typename U>
    inline DynamicParallelSystemBlock<T> &operator*=(const U scalar)
        requires std::is_convertible_v<U, T>
    {
        for (auto &sys_pair : *m_systems)
        {
            sys_pair.second *= static_cast<T>(scalar);
        }
        return *this;
    }

    template <typename U>
    inline DynamicParallelSystemBlock<T> &operator/=(const U scalar)
        requires std::is_convertible_v<U, T>
    {
        *this *= static_cast<T>(1) / static_cast<T>(scalar);
        return *this;
    }

    template <typename U>
    inline DynamicCascadedSystemBlock<T> operator+(const U scalar) const
        requires std::is_convertible_v<U, T>
    {
        DynamicCascadedSystemBlock<T> result;
        result.append(*this);
        result.append(std::make_shared<Bias<T>>(static_cast<T>(scalar)));
        return result;
    }

    template <typename U>
    inline DynamicCascadedSystemBlock<T> operator-(const U scalar) const
        requires std::is_convertible_v<U, T>
    {
        return *this + (-static_cast<T>(scalar));
    }

    template <typename U>
    inline DynamicParallelSystemBlock<T> operator*(const U scalar) const
        requires std::is_convertible_v<U, T>
    {
        DynamicParallelSystemBlock<T> result = copy();
        result *= scalar;
        return result;
    }

    template <typename U>
    inline DynamicParallelSystemBlock<T> operator/(const U scalar) const
        requires std::is_convertible_v<U, T>
    {
        return *this * (static_cast<T>(1) / static_cast<T>(scalar));
    }

    // unary minus
    [[nodiscard]] inline DynamicParallelSystemBlock<T> operator-() const
    {
        DynamicParallelSystemBlock<T> result = copy();
        for (auto &sys_pair : *result.m_systems)
        {
            sys_pair.second = -sys_pair.second;
        }
        return result;
    }

    // friend scalar operators
    template <typename U, typename V>
    friend DynamicCascadedSystemBlock<V> operator+(const U scalar, const DynamicParallelSystemBlock<V> &sys)
        requires std::is_convertible_v<U, V>;
    template <typename U, typename V>
    friend DynamicCascadedSystemBlock<V> operator-(const U scalar, const DynamicParallelSystemBlock<V> &sys)
        requires std::is_convertible_v<U, V>;
    template <typename U, typename V>
    friend DynamicParallelSystemBlock<V> operator*(const U scalar, const DynamicParallelSystemBlock<V> &sys)
        requires std::is_convertible_v<U, V>;
};

template <typename U, typename V>
inline DynamicCascadedSystemBlock<V> operator+(const U scalar, const DynamicParallelSystemBlock<V> &sys)
    requires std::is_convertible_v<U, V>
{
    return scalar + DynamicCascadedSystemBlock<V>(sys);
}

template <typename U, typename V>
inline DynamicCascadedSystemBlock<V> operator-(const U scalar, const DynamicParallelSystemBlock<V> &sys)
    requires std::is_convertible_v<U, V>
{
    return scalar + (-sys);
}

template <typename U, typename V>
inline DynamicParallelSystemBlock<V> operator*(const U scalar, const DynamicParallelSystemBlock<V> &sys)
    requires std::is_convertible_v<U, V>
{
    return sys * scalar;
}

//==============================================================================
// Dynamic System Pipeline
//==============================================================================
template <typename T>
class DynamicSystemPipeline : public ISystemBlock<DynamicSystemPipeline<T>, T>
{
public:
    using SystemPtrT = SystemPtr<T>;
    using VariantT   = std::variant<DynamicCascadedSystemBlock<T>, DynamicParallelSystemBlock<T>>;

private:
    VariantT m_system;

public:
    inline DynamicSystemPipeline() : m_system(DynamicCascadedSystemBlock<T>())
    {
    }

    explicit inline DynamicSystemPipeline(const DynamicCascadedSystemBlock<T> &cascade) : m_system(cascade)
    {
    }

    explicit inline DynamicSystemPipeline(const DynamicParallelSystemBlock<T> &parallel) : m_system(parallel)
    {
    }

    explicit inline DynamicSystemPipeline(VariantT &&sys_variant) : m_system(std::move(sys_variant))
    {
    }

    [[nodiscard]] DynamicSystemPipeline copy()
    {
        return DynamicSystemPipeline(std::visit([](auto &sys) { return VariantT(sys.copy()); }, m_system));
    }

    // --- CRTP Implementation ---

    inline T process_impl(const T input)
    {
        return std::visit([&](auto &system) { return system.process(input); }, m_system);
    }

    inline void reset_impl()
    {
        std::visit([&](auto &system) { system.reset(); }, m_system);
    }

    // Concrete system or system ptr operators
    template <typename TSystem>
    inline DynamicSystemPipeline<T> &operator*=(const TSystem sys)
    {
        std::visit(Overloaded{[&](DynamicCascadedSystemBlock<T> &cascade) { cascade *= sys; },
                              [&](DynamicParallelSystemBlock<T> &parallel) { m_system = std::move(parallel * sys); }},
                   m_system);
        return *this;
    }

    template <typename TSystem>
    inline DynamicSystemPipeline<T> &operator+=(const TSystem sys)
    {
        std::visit(Overloaded{[&](DynamicCascadedSystemBlock<T> &cascade) { m_system = std::move(cascade + sys); },
                              [&](DynamicParallelSystemBlock<T> &parallel) { parallel += sys; }},
                   m_system);
        return *this;
    }

    template <typename TSystem>
    inline DynamicSystemPipeline<T> &operator-=(const TSystem sys)
    {
        std::visit(Overloaded{[&](DynamicCascadedSystemBlock<T> &cascade) { m_system = std::move(cascade - sys); },
                              [&](DynamicParallelSystemBlock<T> &parallel) { parallel -= sys; }},
                   m_system);
        return *this;
    }

    template <typename TSystem>
    [[nodiscard]] inline DynamicSystemPipeline<T> operator*(const TSystem sys) const
    {
        DynamicSystemPipeline<T> result = copy();
        result *= sys;
        return result;
    }

    template <typename TSystem>
    [[nodiscard]] inline DynamicSystemPipeline<T> operator+(const TSystem sys) const
    {
        DynamicSystemPipeline<T> result = copy();
        result += sys;
        return result;
    }

    template <typename TSystem>
    [[nodiscard]] inline DynamicSystemPipeline<T> operator-(const TSystem sys) const
    {
        DynamicSystemPipeline<T> result = copy();
        result -= sys;
        return result;
    }

    // Scalar Operations
    template <typename U>
    inline DynamicSystemPipeline<T> &operator+=(const U scalar)
        requires std::is_convertible_v<U, T>
    {
        std::visit(
            Overloaded{[&](DynamicCascadedSystemBlock<T> &cascade) { cascade += scalar; },
                       [&](DynamicParallelSystemBlock<T> &parallel) { m_system = std::move(parallel + scalar); }},
            m_system);
        return *this;
    }

    template <typename U>
    inline DynamicSystemPipeline<T> &operator-=(const U scalar)
        requires std::is_convertible_v<U, T>
    {
        *this += -static_cast<T>(scalar);
        return *this;
    }

    template <typename U>
    inline DynamicSystemPipeline<T> &operator*=(const U scalar)
        requires std::is_convertible_v<U, T>
    {
        std::visit([&](auto &sys) { sys *= scalar; }, m_system);
        return *this;
    }

    template <typename U>
    inline DynamicSystemPipeline<T> &operator/=(const U scalar)
        requires std::is_convertible_v<U, T>
    {
        *this *= static_cast<T>(1) / static_cast<T>(scalar);
        return *this;
    }

    template <typename U>
    [[nodiscard]] inline DynamicSystemPipeline<T> operator+(const U scalar) const
        requires std::is_convertible_v<U, T>
    {
        DynamicSystemPipeline<T> result = copy();
        result += scalar;
        return result;
    }

    template <typename U>
    [[nodiscard]] inline DynamicSystemPipeline<T> operator-(const U scalar) const
        requires std::is_convertible_v<U, T>
    {
        DynamicSystemPipeline<T> result = copy();
        result -= scalar;
        return result;
    }

    template <typename U>
    [[nodiscard]] inline DynamicSystemPipeline<T> operator*(const U scalar) const
        requires std::is_convertible_v<U, T>
    {
        DynamicSystemPipeline<T> result = copy();
        result *= scalar;
        return result;
    }

    template <typename U>
    [[nodiscard]] inline DynamicSystemPipeline<T> operator/(const U scalar) const
        requires std::is_convertible_v<U, T>
    {
        DynamicSystemPipeline<T> result = copy();
        result /= scalar;
        return result;
    }

    // unary minus
    [[nodiscard]] inline DynamicSystemPipeline<T> operator-() const
    {
        DynamicSystemPipeline<T>(std::visit([](auto &sys) { return -sys; }, m_system));
    }

    // friend scalar operators
    template <typename U, typename V>
    friend DynamicSystemPipeline<V> operator+(const U scalar, const DynamicSystemPipeline<V> &sys)
        requires std::is_convertible_v<U, V>;
    template <typename U, typename V>
    friend DynamicSystemPipeline<V> operator-(const U scalar, const DynamicSystemPipeline<V> &sys)
        requires std::is_convertible_v<U, V>;
    template <typename U, typename V>
    friend DynamicSystemPipeline<V> operator*(const U scalar, const DynamicSystemPipeline<V> &sys)
        requires std::is_convertible_v<U, V>;
};

template <typename U, typename V>
DynamicSystemPipeline<V> operator+(const U scalar, const DynamicSystemPipeline<V> &sys)
    requires std::is_convertible_v<U, V>
{
    return DynamicSystemPipeline<V>(std::visit([&](auto &s) { return scalar + s; }, sys.m_system));
}

template <typename U, typename V>
DynamicSystemPipeline<V> operator-(const U scalar, const DynamicSystemPipeline<V> &sys)
    requires std::is_convertible_v<U, V>
{
    return DynamicSystemPipeline<V>(std::visit([&](auto &s) { return scalar - s; }, sys.m_system));
}

template <typename U, typename V>
DynamicSystemPipeline<V> operator*(const U scalar, const DynamicSystemPipeline<V> &sys)
    requires std::is_convertible_v<U, V>
{
    return DynamicSystemPipeline<V>(
        std::visit([&](auto &s) { return DynamicSystemPipeline<V>::VariantT(scalar * s); }, sys.m_system));
}

#endif  // HEAP_MEMORY_ENABLED

//==============================================================================
// Gain: x * k
//==============================================================================
template <typename T>
class Gain : public ISystemBlock<Gain<T>, T>
{
private:
    T k;

public:
    inline Gain(const T v) : k(v)
    {
    }
    inline T process_impl(const T x)
    {
        return x * k;
    }
    inline void reset_impl()
    {
    }

    inline T gain() const
    {
        return k;
    }
    inline void set_gain(const T gain)
    {
        k = gain;
    }
};

//==============================================================================
// Bias: x + b
//==============================================================================
template <typename T>
class Bias : public ISystemBlock<Bias<T>, T>
{
private:
    T b;

public:
    inline Bias(const T v) : b(v)
    {
    }
    inline T process_impl(const T x)
    {
        return x + b;
    }
    inline void reset_impl()
    {
    }

    inline T bias() const
    {
        return b;
    }
    inline void set_bias(const T bias)
    {
        b = bias;
    }
};

//==============================================================================
// Saturator: standalone clamp
//==============================================================================
template <typename T>
class Saturator : public ISystemBlock<Saturator<T>, T>
{
private:
    T m_min;
    T m_max;

public:
    inline Saturator(T min_val, T max_val) : m_min(min_val), m_max(max_val)
    {
    }

    inline T process_impl(const T input)
    {
        return std::max(m_min, std::min(input, m_max));
    }

    inline void reset_impl()
    {
        // no state
    }

    inline void set_limits(const T min_val, const T max_val)
    {
        m_min = min_val;
        m_max = max_val;
    }

    inline T min_limit() const
    {
        return m_min;
    }
    inline T max_limit() const
    {
        return m_max;
    }
};
