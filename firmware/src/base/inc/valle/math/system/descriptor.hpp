#pragma once

#include <Eigen/Dense>
#include <type_traits>

#define VALLE_SYSMATH_DECLARE_DESCRIPTOR_PROPERTIES(TDescriptor) \
    using DescriptorT  = TDescriptor;                            \
    using ValueT       = DescriptorT::ValueT;                    \
    using ComplexT     = DescriptorT::ComplexT;                  \
    using PoleT        = DescriptorT::PoleT;                     \
    using ScalarT      = DescriptorT::ScalarT;                   \
    using MatrixXT     = DescriptorT::MatrixXT;                  \
    using MatrixXcT    = DescriptorT::MatrixXcT;                 \
    using MatrixXsT    = DescriptorT::MatrixXsT;                 \
    using ColVectorXT  = DescriptorT::ColVectorXT;               \
    using ColVectorXcT = DescriptorT::ColVectorXcT;              \
    using ColVectorXsT = DescriptorT::ColVectorXsT;              \
    using RowVectorXT  = DescriptorT::RowVectorXT;               \
    using RowVectorXcT = DescriptorT::RowVectorXcT;              \
    using RowVectorXsT = DescriptorT::RowVectorXsT;              \
                                                                 \
    static constexpr auto kIsReal      = DescriptorT::kIsReal;   \
    static constexpr auto skIsDiscrete = DescriptorT::skIsDiscrete;

namespace valle
{
    template <std::floating_point TValue, bool tkIsReal, bool tkIsDiscrete>
    struct SystemDescriptor
    {
        using SystemDescriptorIdentity = void;  // just a tag used for concepts

        using ValueT   = TValue;
        using ComplexT = std::complex<ValueT>;
        using PoleT    = Pole<ValueT, tkIsDiscrete>;
        using ScalarT  = std::conditional_t<tkIsReal, ValueT, ComplexT>;

        using MatrixXT  = Eigen::Matrix<ValueT, Eigen::Dynamic, Eigen::Dynamic>;
        using MatrixXcT = Eigen::Matrix<ComplexT, Eigen::Dynamic, Eigen::Dynamic>;
        using MatrixXsT = Eigen::Matrix<ScalarT, Eigen::Dynamic, Eigen::Dynamic>;

        using ColVectorXT  = Eigen::Vector<ValueT>;
        using ColVectorXcT = Eigen::Vector<ComplexT>;
        using ColVectorXsT = Eigen::Vector<ScalarT>;

        using RowVectorXT  = Eigen::RowVector<ValueT>;
        using RowVectorXcT = Eigen::RowVector<ComplexT>;
        using RowVectorXsT = Eigen::RowVector<ScalarT>;

        static constexpr bool skIsReal     = tkIsReal;
        static constexpr bool skIsDiscrete = tkIsDiscrete;
        static constexpr char skDomainChar = skIsDiscrete ? 'z' : 's';
    };

    template <std::floating_point TValue, bool tkIsReal>
    using DiscreteSystemDescriptor = SystemDescriptor<TValue, tkIsReal, true>;

    template <std::floating_point TValue, bool tkIsReal>
    using ContinuousSystemDescriptor = SystemDescriptor<TValue, tkIsReal, false>;

    template <std::floating_point TValue, bool tkIsDiscrete>
    using RealSystemDescriptor = SystemDescriptor<TValue, true, tkIsDiscrete>;

    template <std::floating_point TValue, bool tkIsDiscrete>
    using ComplexSystemDescriptor = SystemDescriptor<TValue, false, tkIsDiscrete>;

    template <std::floating_point TValue>
    using DiscreteRealSystemDescriptor = SystemDescriptor<TValue, true, true>;

    template <std::floating_point TValue>
    using ContinuousRealSystemDescriptor = SystemDescriptor<TValue, true, false>;

    template <std::floating_point TValue>
    using DiscreteComplexSystemDescriptor = SystemDescriptor<TValue, false, true>;

    template <std::floating_point TValue>
    using ContinuousComplexSystemDescriptor = SystemDescriptor<TValue, false, false>;

    template <typename T>
    concept CSystemDescriptor = requires { typename T::SystemDescriptorIdentity; };

    template <typename T>
    concept CDiscreteSystemDescriptor = CSystemDescriptor<T> && (T::skIsDiscrete);

    template <typename T>
    concept CContinuousSystemDescriptor = CSystemDescriptor<T> && (!T::skIsDiscrete);

    template <typename T>
    concept CRealSystemDescriptor = CSystemDescriptor<T> && (T::kIsReal);

    template <typename T>
    concept CComplexSystemDescriptor = CSystemDescriptor<T> && (!T::kIsReal);

    template <typename T>
    concept CDiscreteRealSystemDescriptor = CDiscreteSystemDescriptor<T> && CRealSystemDescriptor<T>;

    template <typename T>
    concept CContinuousRealSystemDescriptor = CContinuousSystemDescriptor<T> && CRealSystemDescriptor<T>;

    template <typename T>
    concept CDiscreteComplexSystemDescriptor = CDiscreteSystemDescriptor<T> && CComplexSystemDescriptor<T>;

    template <typename T>
    concept CContinuousComplexSystemDescriptor = CContinuousSystemDescriptor<T> && CComplexSystemDescriptor<T>;

    /**
     * @brief Converts a descriptor to its discrete form.
     * If T is already discrete, it returns T.
     * If T is continuous, it returns the discrete equivalent.
     */
    template <typename T>
    using DiscreteSystemDescriptorT =
        typename std::conditional<CDiscreteSystemDescriptor<T>,  // Condition: Is T already discrete?
                                  T,                             // Type if true: Use T as is.
                                  SystemDescriptor<typename T::ValueT, T::kIsReal, true>  // Type if false: Convert to
                                                                                          // discrete.
                                  >::type;

    /**
     * @brief Converts a descriptor to its continuous form.
     * If T is already continuous, it returns T.
     * If T is discrete, it returns the continuous equivalent.
     */
    template <typename T>
    using ContinuousSystemDescriptorT =
        typename std::conditional<CContinuousSystemDescriptor<T>,  // Condition: Is T already continuous?
                                  T,                               // Type if true: Use T as is.
                                  SystemDescriptor<typename T::ValueT, T::kIsReal, false>  // Type if false: Convert to
                                                                                           // continuous.
                                  >::type;

    template <typename T, bool tkIsDiscrete>
    struct SampleTime
    {
        // Continuous system: no Ts, no burden
        constexpr static T skTs = T{};
    };

    template <typename T>
    struct SampleTime<T, true>
    {
        T skTs;
        explicit SampleTime(T ts_in = T{}) : skTs(ts_in)
        {
        }
    };

}  // namespace valle