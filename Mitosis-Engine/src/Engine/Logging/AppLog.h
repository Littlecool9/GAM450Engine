/**
 * @file AppLog.h
 * @author sumin.park
 * @brief Logging for apps using the engine.
 *
 * @copyright Copyright (c) 2026 DigiPen (USA) Corporation
 *
 */

#pragma once
#include "Log.h"

#undef LOG_BASE
#undef LOG_INFO
#undef LOG_WARN
#undef LOG_ERROR
#undef LOG_CRITICAL

#define LOG_BASE(loggerId, level, fmt, ...)                             \
    ::mts::Log::Submit((loggerId),                                      \
                       (level),                                         \
                       ::mts::Format((fmt) __VA_OPT__(, ) __VA_ARGS__), \
                       std::source_location::current())

#ifdef MTS_RELEASE
    #define LOG_INFO(fmt, ...)     ((void)0)
    #define LOG_WARN(fmt, ...)     ((void)0)
#else
    #define LOG_INFO(fmt, ...) LOG_BASE(::mts::LoggerId::App, ::mts::LogLevel::Info, (fmt), __VA_ARGS__)
    #define LOG_WARN(fmt, ...) LOG_BASE(::mts::LoggerId::App, ::mts::LogLevel::Warn, (fmt), __VA_ARGS__)
#endif
#define LOG_ERROR(fmt, ...) \
    LOG_BASE(::mts::LoggerId::App, ::mts::LogLevel::Error, (fmt), __VA_ARGS__)
#define LOG_CRITICAL(fmt, ...) \
    LOG_BASE(::mts::LoggerId::App, ::mts::LogLevel::Critical, (fmt), __VA_ARGS__)