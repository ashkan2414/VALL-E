#pragma once

#ifdef _MSC_VER
#define VALLE_COMPILER_MSVC
#elif defined(__GNUC__)
#define VALLE_COMPILER_GCC
#elif defined(__clang__)
#define VALLE_COMPILER_CLANG
#else
#error "Unsupported compiler"
#endif

#if defined(VALLE_COMPILER_MSVC)
#define VALLE_FORCE_INLINE __forceinline
#elif defined(VALLE_COMPILER_GCC) || defined(VALLE_COMPILER_CLANG)
#define VALLE_FORCE_INLINE inline __attribute__((always_inline))
#else
#define VALLE_FORCE_INLINE inline
#endif