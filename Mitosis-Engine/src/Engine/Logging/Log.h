/**
 * @file Log.h
 * @author sumin.park
 * @brief The static class that manages logging.
 *
 * @copyright Copyright (c) 2025 - Mitosis Engine
 *
 */

#pragma once
#include <format>
#include <source_location>
#include <string>
#include <vector>

namespace mts
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

    struct LogEntry
    {
        LogLevel    level;
        std::string text;
    };

    class Log
    {
       public:
        static void Init();
        static void Shutdown();
        static void Submit(LoggerId logger,
                           LogLevel level,
                           std::string_view message,
                           std::source_location where = std::source_location::current());

        static const std::vector<LogEntry>& GetEntries();
        static void ClearEntries();

       private:
    };

    template <typename... Args>
    std::string Format(std::format_string<Args...> fmt, Args&&... args)
    {
        return std::format(fmt, std::forward<Args>(args)...);
    }
}  // namespace mts