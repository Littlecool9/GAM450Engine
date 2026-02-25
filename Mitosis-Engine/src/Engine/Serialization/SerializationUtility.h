/**
 * @file SerializationUtility.h
 * @author Dong Ta
 * @brief  Functions to help deserialization process.
 * @version 0.1
 * @date 1/8/2026 7:06:27 AM
 *
 * @copyright Copyright (c) 2025 - RM Engine
 *
 */

#pragma once
#include <string>
#include "Deserializable.h"

namespace mts
{
    namespace SerializationUtility
    {
        // Reading from a file
        void DeserializeFromFile(Deserializable* item, std::string const& filepath);
        void DeserializeFromString(Deserializable* item, std::string const& str);
        // Writing to a file
        bool SerializeToFile(Deserializable* item, const std::string& path);
        std::string SerializeToString(Deserializable* item);
    }  // namespace SerializationUtility
}  // namespace mts
