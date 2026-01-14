/**
 * @file Format.h
 * @author sumin.park
 * @brief  basic string formats for the engine
 *
 * @copyright Copyright (c) 2025 - RM Engine
 *
 */

#pragma once
#include <string_view>

namespace mts::string_format
{
    constexpr std::string_view ShortFile(std::string_view path)
    {
        size_t pos = path.find_last_of("/\\");
        return (pos == std::string_view::npos) ? path : path.substr(pos + 1);
    }

    constexpr std::string_view ShortFunc(std::string_view name)
    {
        size_t pos = name.find_last_of("::");
        return (pos == std::string_view::npos) ? name : name.substr(pos + 1);
    }
} // rm namespace