/**
 * @file Log.cpp
 * @author sumin.park
 * @brief The static class that manages logging.
 *
 * @copyright Copyright (c) 2025 - Mitosis Engine
 *
 */
#include "pch.h"
#include "Log.h"
#include "Engine/Helpers/Format.h"
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <array>

constexpr auto LOG_PATTERN = "%^[%H:%M:%S] [%n] [%l] %v%$";

namespace mts
{
    static std::array<std::shared_ptr<spdlog::logger>, 2> loggers;
    static std::vector<LogEntry> s_logEntries;

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
        return spdlog::level::info;
    }

    static spdlog::logger* Get(LoggerId id)
    {
        return loggers[static_cast<size_t>(id)].get();
    }

    void Log::Init()
    {
        auto console = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        console->set_pattern(LOG_PATTERN);

        std::vector<spdlog::sink_ptr> sinks{console};

        loggers[static_cast<size_t>(LoggerId::Engine)] =
            std::make_shared<spdlog::logger>("ENGINE", sinks.begin(), sinks.end());

        loggers[static_cast<size_t>(LoggerId::App)] =
            std::make_shared<spdlog::logger>("APP", sinks.begin(), sinks.end());
    }

    void Log::Shutdown()
    {
        spdlog::shutdown();
    }

    void Log::Submit(LoggerId logger,
                     LogLevel level,
                     std::string_view message,
                     std::source_location where)
    {
        auto* targetLogger = Get(logger);
        assert(targetLogger);
        auto fileName = string_format::ShortFile(where.file_name());
        // auto functionName = string_format::ShortFunc(where.function_name());
        std::string entryText = std::format("[{}: line {}] {}", fileName, where.line(), message);
        s_logEntries.push_back({ level, entryText });
        targetLogger->log(ToSpdLvl(level), "{}", entryText);
    }

    const std::vector<LogEntry>& Log::GetEntries() { return s_logEntries; }
    void Log::ClearEntries()                        { s_logEntries.clear(); }
}  // namespace mts