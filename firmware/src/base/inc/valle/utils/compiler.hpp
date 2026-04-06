#pragma once

// Compiler detection.
#ifdef _MSC_VER
#define VALLE_COMPILER_MSVC
#elif defined(__GNUC__)
#define VALLE_COMPILER_GCC
#elif defined(__clang__)
#define VALLE_COMPILER_CLANG
#else
#error "Unsupported compiler"
#endif

// VALLE_NO_INLINE: Prevents the compiler from inlining a function, which can be useful for debugging or to reduce code
// size in certain cases.
#if defined(VALLE_COMPILER_GCC) || defined(VALLE_COMPILER_CLANG)
#define VALLE_NO_INLINE __attribute__((noinline))
#elif defined(VALLE_COMPILER_MSVC)
#define VALLE_NO_INLINE __declspec(noinline)
#else
#define VALLE_NO_INLINE
#endif

// VALLE_FORCE_INLINE: Suggests to the compiler that a function should always be inlined, which can improve performance.
#if defined(VALLE_COMPILER_MSVC)
#define VALLE_FORCE_INLINE __forceinline
#elif defined(VALLE_COMPILER_GCC) || defined(VALLE_COMPILER_CLANG)
#define VALLE_FORCE_INLINE inline __attribute__((always_inline))
#else
#define VALLE_FORCE_INLINE inline
#endif

// VALLE_OPTIMIZE_PUSH and VALLE_OPTIMIZE_POP: These macros allow you to push and pop optimization settings for specific
// sections of code.
#if defined(VALLE_COMPILER_GCC) || defined(VALLE_COMPILER_CLANG)
#define VALLE_DO_PRAGMA_IMPL(x) _Pragma(#x)
#define VALLE_DO_PRAGMA(x)      VALLE_DO_PRAGMA_IMPL(x)
#endif

#if defined(VALLE_COMPILER_CLANG)
#define VALLE_OPTIMIZE_PUSH VALLE_DO_PRAGMA(clang optimize on)
#define VALLE_OPTIMIZE_POP  VALLE_DO_PRAGMA(clang optimize off)
#elif defined(VALLE_COMPILER_GCC)
#define VALLE_OPTIMIZE_PUSH           \
    VALLE_DO_PRAGMA(GCC push_options) \
    VALLE_DO_PRAGMA(GCC optimize("O3,inline-functions,inline-small-functions"))
#define VALLE_OPTIMIZE_POP VALLE_DO_PRAGMA(GCC pop_options)
#elif defined(VALLE_COMPILER_MSVC)
#define VALLE_OPTIMIZE_PUSH __pragma(optimize("gt", on))
#define VALLE_OPTIMIZE_POP  __pragma(optimize("", on))
#else
#error "Unsupported compiler"
#endif
