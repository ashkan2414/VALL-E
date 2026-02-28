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
#include <tuple>        // Essential for your Registry
#include <type_traits>  // Essential for your Metaprogramming
#include <utility>      // std::move, std::forward
#include <variant>
#endif

// ============================================================================
// PROJECT CORE (Optional)
// ============================================================================
// Only include your headers here if they are STABLE (not changing often).

#ifdef __cplusplus
#include "valle/core/device/device.hpp"
#include "valle/core/device/registry.hpp"
#endif