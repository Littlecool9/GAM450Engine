/**
 * @file Log.h
 * @author sumin.park
 * @brief The static class that manages logging.
 *
 * @copyright Copyright (c) 2025 - RM Engine
 *
 */

#pragma once
#include <format>
#include <source_location>
#include <string>

namespace rm
{
	enum class LogLevel : unsigned char
	{
		Trace,
		Debug,
		Info,
		Warn,
		Error,
		Critical
	};
	enum class LoggerId : unsigned char
	{
		Engine,
		App
	};

	class Log
	{
	public:
		static void Init();
		static void Shutdown();
		static void Submit(LoggerId logger, LogLevel level, std::string_view message, std::source_location where = std::source_location::current());

	private:
	};

	template <typename... Args>
	std::string Format(std::format_string<Args...> fmt, Args &&...args)
	{
		return std::format(fmt, std::forward<Args>(args)...);
	}
} // rm namespace