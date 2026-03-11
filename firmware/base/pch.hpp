#pragma once

// ============================================================================
// STANDARD LIBRARY (Heavy Templates)
// ============================================================================
// These are the most "expensive" headers to parse repeatedly.

#ifdef __cplusplus

#include <algorithm>
#include <array>
#include <atomic>
#include <bit>  // C++20 bit manipulation
#include <cmath>
#include <concepts>  // C++20 concepts
#include <cstdint>
#include <functional>  // std::reference_wrapper
#include <limits>
#include <memory>
#include <span>
#include <tuple>        // Essential for Registry
#include <type_traits>  // Essential for Metaprogramming
#include <utility>      // std::move, std::forward
#include <variant>

#endif

// ============================================================================
// PROJECT CORE
// ============================================================================
// Only include headers here if they are STABLE (not changing often).

#ifdef __cplusplus
#include "valle/base/hardware_build/device.hpp"
#include "valle/utils/atomic.hpp"
#include "valle/utils/circular_queue.hpp"
#include "valle/utils/enum.hpp"
#include "valle/utils/source_location.hpp"
#include "valle/utils/timing.hpp"
#include "valle/utils/tmp.hpp"

#endif