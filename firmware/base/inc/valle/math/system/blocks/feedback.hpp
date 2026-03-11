#pragma once

#include "valle/math/system/block.hpp"

namespace valle::system
{

    //==============================================================================
    // FeedbackSystem: Feedback loop control system
    //==============================================================================
    template <CSystemOrRef TOpenLoopSys>
    class FeedbackSystem : public ISystemBlock<FeedbackSystem<TOpenLoopSys>, typename TOpenLoopSys::ValueT>
    {
    public:
        using ValueT                                  = typename TOpenLoopSys::ValueT;
        static constexpr ValueT skDefaultMinTolerance = static_cast<ValueT>(1e-6);

    private:
        TOpenLoopSys m_ol_system{};
        ValueT       m_last_output   = static_cast<ValueT>(0.0);
        ValueT       m_min_tolerance = skDefaultMinTolerance;

    public:
        explicit FeedbackSystem(TOpenLoopSys &open_loop, ValueT min_tolerance = skDefaultMinTolerance)
            : m_ol_system(open_loop), m_last_output(static_cast<ValueT>(0.0)), m_min_tolerance(min_tolerance)
        {
        }

        ValueT process_impl(const ValueT reference)
        {
            const ValueT error = reference - m_last_output;
            if (at_target_error(error, m_min_tolerance))
            {
                return open_loop().process(static_cast<ValueT>(0));
            }

            return open_loop().process(error);
        }

        // --- CRTP reset call ---
        void reset_impl()
        {
            open_loop().reset();
        }

        // --- Optional accessors ---
        TOpenLoopSys &open_loop()
        {
            return m_ol_system;
        }
        const TOpenLoopSys &open_loop() const
        {
            return m_ol_system;
        }

        bool at_target(const ValueT reference) const
        {
            return at_target(reference, m_min_tolerance);
        }

        bool at_target(const ValueT reference, const ValueT tolerance) const
        {
            return at_target_error(reference - m_last_output, tolerance);
        }

    private:
        static bool at_target_error(const ValueT error, const ValueT tolerance)
        {
            return std::fabs(error) < tolerance;
        }
    };

    //==============================================================================
    // ExFeedbackSystem1: Feedback loop control system with external feedback (function call)
    //==============================================================================

    template <typename TFeedbackFn, typename TValue>
    concept CFeedbackFunction = requires(TFeedbackFn f) {
        { f() } -> std::same_as<TValue>;
    };

    template <CSystemOrRef TOpenLoopSys, CFeedbackFunction<typename TOpenLoopSys::ValueT> TFeedbackFn>
    class ExFeedbackSystem1
        : public ISystemBlock<ExFeedbackSystem1<TOpenLoopSys, TFeedbackFn>, typename TOpenLoopSys::ValueT>
    {
    public:
        using ValueT                                  = typename TOpenLoopSys::ValueT;
        static constexpr ValueT skDefaultMinTolerance = static_cast<ValueT>(1e-6);

    private:
        TOpenLoopSys m_ol_system{};
        TFeedbackFn  m_feedback_fn{};
        ValueT       m_min_tolerance = skDefaultMinTolerance;

    public:
        explicit ExFeedbackSystem1() : m_ol_system(), m_feedback_fn(), m_min_tolerance(skDefaultMinTolerance)
        {
        }

        explicit ExFeedbackSystem1(const TOpenLoopSys &open_loop,
                                   TFeedbackFn       &&feedback_fn,
                                   ValueT              min_tolerance = skDefaultMinTolerance)
            : m_ol_system(open_loop), m_feedback_fn(std::move(feedback_fn)), m_min_tolerance(min_tolerance)
        {
        }

        ValueT process_impl(const ValueT reference)
        {
            const ValueT error = reference - get_feedback();
            if (at_target_error(error, m_min_tolerance))
            {
                return open_loop().process(static_cast<ValueT>(0));
            }

            return open_loop().process(error);
        }

        void set_open_loop_system(const TOpenLoopSys &open_loop)
        {
            m_ol_system = open_loop;
        }

        void set_feedback_function(TFeedbackFn feedback_fn)
        {
            m_feedback_fn = feedback_fn;
        }

        void set_min_tolerance(const ValueT tolerance)
        {
            m_min_tolerance = tolerance;
        }

        // --- CRTP reset call ---
        void reset_impl()
        {
            open_loop().reset();
        }

        // --- Optional accessors ---
        TOpenLoopSys &open_loop()
        {
            return m_ol_system;
        }
        const TOpenLoopSys &open_loop() const
        {
            return m_ol_system;
        }

        bool at_target(const ValueT reference) const
        {
            return at_target(reference, m_min_tolerance);
        }

        // NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
        bool at_target(const ValueT reference, const ValueT tolerance) const
        {
            return at_target_error(reference - get_feedback(), tolerance);
        }

        ValueT get_feedback() const
        {
            return m_feedback_fn();
        }

    private:
        // NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
        static bool at_target_error(const ValueT error, const ValueT tolerance)
        {
            return std::fabs(error) < tolerance;
        }
    };

    template <CSystemOrRef TOpenLoopSys>
    class ExFeedbackSystem2 : public ISystemBlock<ExFeedbackSystem2<TOpenLoopSys>, typename TOpenLoopSys::ValueT>
    {
    public:
        using ValueT                                  = typename TOpenLoopSys::ValueT;
        static constexpr ValueT skDefaultMinTolerance = static_cast<ValueT>(1e-6);

    private:
        TOpenLoopSys m_ol_system{};
        ValueT       m_min_tolerance{};
        ValueT       m_feedback_value{};

    public:
        explicit ExFeedbackSystem2()
            : m_ol_system(), m_min_tolerance(skDefaultMinTolerance), m_feedback_value(static_cast<ValueT>(0))
        {
        }

        // NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
        explicit ExFeedbackSystem2(const TOpenLoopSys &open_loop, const ValueT min_tolerance = skDefaultMinTolerance)
            : m_ol_system(open_loop), m_min_tolerance(min_tolerance), m_feedback_value(static_cast<ValueT>(0))
        {
        }

        ValueT process_impl(const ValueT reference)
        {
            const ValueT error = reference - get_feedback();
            if (at_target_error(error, m_min_tolerance))
            {
                return open_loop().process(static_cast<ValueT>(0));
            }

            return open_loop().process(error);
        }

        void set_feedback(const ValueT feedback)
        {
            m_feedback_value = feedback;
        }

        void set_open_loop_system(const TOpenLoopSys &open_loop)
        {
            m_ol_system = open_loop;
        }

        void set_min_tolerance(const ValueT tolerance)
        {
            m_min_tolerance = tolerance;
        }

        // --- CRTP reset call ---
        void reset_impl()
        {
            open_loop().reset();
        }

        // --- Optional accessors ---
        TOpenLoopSys &open_loop()
        {
            return m_ol_system;
        }
        const TOpenLoopSys &open_loop() const
        {
            return m_ol_system;
        }

        bool at_target(const ValueT reference) const
        {
            return at_target(reference, m_min_tolerance);
        }

        // NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
        bool at_target(const ValueT reference, const ValueT tolerance) const
        {
            return at_target_error(reference - get_feedback(), tolerance);
        }

        ValueT get_feedback() const
        {
            return m_feedback_value;
        }

    private:
        // NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
        static bool at_target_error(const ValueT error, const ValueT tolerance)
        {
            return std::fabs(error) < tolerance;
        }
    };

}  // namespace valle::system