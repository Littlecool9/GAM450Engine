/**
 * @file SerializationUtility.cpp
 * @author Dong Ta
 * @brief  definitions for functions to help deserialization process.
 * @version 0.1
 * @date 1/8/2026 7:06:27 AM
 *
 * @copyright Copyright (c) 2025 - RM Engine
 *
 */
#include "pch.h"
#include "SerializationUtility.h"

// other includes
#include <fstream>
#include "JsonAbstraction.h"

namespace mts
{
    namespace SerializationUtility
    {
        void DeserializeFromFile(Deserializable* item, std::string const& filepath)
        {
            mts::JsonDocument doc;
            if (!doc.LoadFromFile(filepath))
            {
                LOG_INFO("File parsing error occurred\n");
                return;
            }

            item->Deserialize(doc.Root());
        }
        void DeserializeFromString(Deserializable* item, std::string const& str)
        {
            mts::JsonDocument doc;
            if (!doc.LoadFromString(str))
            {
                LOG_INFO("String parsing error occurred\n");
                return;
            }

            item->Deserialize(doc.Root());
        }
        bool SerializeToFile(Deserializable* item, const std::string& path)
        {
            JsonDocument doc;

            item->Serialize(doc);  // builds the document

            return doc.SaveToFile(path);
        }
    } 
}  