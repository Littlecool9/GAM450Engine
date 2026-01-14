/**
 * @file Assert.h
 * @author sumin.park
 * @brief The MTS_ASSERT macro definition file.
 *
 * @copyright Copyright (c) 2025 - Mitosis Engine
 *
 */

#pragma once
#include <cstdlib>

#if defined(_MSC_VER)
#define MTS_DEBUGBREAK() __debugbreak()
#elif defined(__clang__) || defined(__GNUC__)
#define MTS_DEBUGBREAK() __builtin_trap()
#else
#define MTS_DEBUGBREAK() std::abort()
#endif

// Debug-only assert: breaks when condition is false.
// In Release, it compiles out completely.
#ifndef NDEBUG
#define MTS_ASSERT(expr)      \
    do                        \
    {                         \
        if (!(expr))          \
        {                     \
            MTS_DEBUGBREAK(); \
        }                     \
    } while (0)
#else
#define MTS_ASSERT(expr) ((void) 0)
#endif