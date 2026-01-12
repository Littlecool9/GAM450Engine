/**
 * @file AppLog.h
 * @author sumin.park
 * @brief Logging for apps using the engine.
 *
 * @copyright Copyright (c) 2025 - RM Engine
 *
 */

#pragma once
#include "Log.h"

#define LOG_BASE(loggerId, level, fmt, ...)                          \
	::rm::Log::Submit((loggerId), (level),                           \
					  ::rm::Format((fmt)__VA_OPT__(, ) __VA_ARGS__), \
					  std::source_location::current())

#define LOG_INFO(fmt, ...) LOG_BASE(::rm::LoggerId::App, ::rm::LogLevel::Info, (fmt), __VA_ARGS__)
#define LOG_WARN(fmt, ...) LOG_BASE(::rm::LoggerId::App, ::rm::LogLevel::Warn, (fmt), __VA_ARGS__)
#define LOG_ERROR(fmt, ...) LOG_BASE(::rm::LoggerId::App, ::rm::LogLevel::Error, (fmt), __VA_ARGS__)
#define LOG_CRITICAL(fmt, ...) LOG_BASE(::rm::LoggerId::App, ::rm::LogLevel::Critical, (fmt), __VA_ARGS__)