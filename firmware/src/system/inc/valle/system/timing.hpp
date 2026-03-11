#pragma once

#include "valle/platform/system_interface/timing.hpp"

namespace valle::system
{
    using TimingContext = valle::platform::system_interface::TimingContext;

    // =========================================================================
    // Global namespace aliases
    // =========================================================================
    using SecondsClock       = typename TimingContext::SecondClockT;
    using DurationSecondsRep = typename TimingContext::DurationSecondsRepT;
    using DurationSeconds    = typename TimingContext::DurationSecondsT;
    using DelaySecondsRep    = typename TimingContext::DelaySecondsRepT;
    using DelaySeconds       = typename TimingContext::DelaySecondsT;
    using TimeoutSecondsRep  = typename TimingContext::TimeoutSecondsRepT;
    using TimeoutSeconds     = typename TimingContext::TimeoutSecondsT;
    using TimerSeconds       = typename TimingContext::TimerSecondsT;

    using MIllisClock       = typename TimingContext::MilliClockT;
    using DurationMillisRep = typename TimingContext::DurationMillisRepT;
    using DurationMillis    = typename TimingContext::DurationMillisT;
    using DelayMillisRep    = typename TimingContext::DelayMillisRepT;
    using DelayMillis       = typename TimingContext::DelayMillisT;
    using TimeoutMillisRep  = typename TimingContext::TimeoutMillisRepT;
    using TimeoutMillis     = typename TimingContext::TimeoutMillisT;
    using TimerMillis       = typename TimingContext::TimerMillisT;

    using MicrosClock       = typename TimingContext::MicroClockT;
    using DurationMicrosRep = typename TimingContext::DurationMicrosRepT;
    using DurationMicros    = typename TimingContext::DurationMicrosT;
    using DelayMicrosRep    = typename TimingContext::DelayMicrosRepT;
    using DelayMicros       = typename TimingContext::DelayMicrosT;
    using TimeoutMicrosRep  = typename TimingContext::TimeoutMicrosRepT;
    using TimeoutMicros     = typename TimingContext::TimeoutMicrosT;
    using TimerMicros       = typename TimingContext::TimerMicrosT;

}  // namespace valle::system