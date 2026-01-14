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
        void DeserializeFromFile(Deserializable* item, std::string const& filepath);
        void DeserializeFromString(Deserializable* item, std::string const& str);
    }  // namespace SerializationUtility
}   

