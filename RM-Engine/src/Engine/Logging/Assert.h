/**
 * @file Assert.h
 * @author sumin.park
 * @brief The RM_ASSERT macro definition file.
 *
 * @copyright Copyright (c) 2025 - RM Engine
 *
 */

#pragma once
#include <cstdlib>

#if defined(_MSC_VER)
#define RM_DEBUGBREAK() __debugbreak()
#elif defined(__clang__) || defined(__GNUC__)
#define RM_DEBUGBREAK() __builtin_trap()
#else
#define RM_DEBUGBREAK() std::abort()
#endif

// Debug-only assert: breaks when condition is false.
// In Release, it compiles out completely.
#ifndef NDEBUG
#define RM_ASSERT(expr) \
  do                    \
  {                     \
    if (!(expr))        \
    {                   \
      RM_DEBUGBREAK();  \
    }                   \
  } while (0)
#else
#define RM_ASSERT(expr) ((void)0)
#endif