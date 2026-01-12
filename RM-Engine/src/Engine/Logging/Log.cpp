/**
 * @file Log.cpp
 * @author sumin.park
 * @brief The static class that manages logging.
 *
 * @copyright Copyright (c) 2025 - RM Engine
 *
 */
#include "pch.h"
#include "Log.h"

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <array>

constexpr auto LOG_MAX_FILE_SIZE_BYTES = (5 * 1024 * 1024);
constexpr auto LOG_MAX_ROTATION_FILES = 3;
constexpr auto LOG_PATTERN = "%^[%H:%M:%S] [%n] [%l] %v%$";
constexpr auto LOG_PATH = "../log/app.log";

namespace rm
{
	static std::array<std::shared_ptr<spdlog::logger>, 2> loggers;

	static spdlog::level::level_enum ToSpdLvl(LogLevel level)
	{
		switch (level)
		{
		case LogLevel::Trace:
			return spdlog::level::trace;
		case LogLevel::Debug:
			return spdlog::level::debug;
		case LogLevel::Info:
			return spdlog::level::info;
		case LogLevel::Warn:
			return spdlog::level::warn;
		case LogLevel::Error:
			return spdlog::level::err;
		case LogLevel::Critical:
			return spdlog::level::critical;
		}
	}

	static spdlog::logger *Get(LoggerId id)
	{
		return loggers[static_cast<size_t>(id)].get();
	}

	void Log::Init()
	{
		auto console = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
		auto file = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(LOG_PATH, LOG_MAX_FILE_SIZE_BYTES, LOG_MAX_ROTATION_FILES);

		console->set_pattern(LOG_PATTERN);
		spdlog::flush_on(spdlog::level::err);

		std::vector<spdlog::sink_ptr> sinks{console, file};

		loggers[static_cast<size_t>(LoggerId::Engine)] =
			std::make_shared<spdlog::logger>("ENGINE", sinks.begin(), sinks.end());

		loggers[static_cast<size_t>(LoggerId::App)] =
			std::make_shared<spdlog::logger>("APP", sinks.begin(), sinks.end());
	}

	void Log::Shutdown()
	{
		spdlog::shutdown();
	}

	void Log::Submit(LoggerId logger, LogLevel level, std::string_view message, std::source_location where)
	{
		auto *targetLogger = Get(logger);
		assert(targetLogger);
		targetLogger->log(ToSpdLvl(level), "{}", message);
	}
} // rm namespace