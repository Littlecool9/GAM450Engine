/**
 * @file SerializationUtility.cpp
 * @author Dong Ta
 * @brief  definitions for functions to help deserialization process.
 * @version 0.1
 * @date 1/8/2026 7:06:27 AM
 *
 * @copyright Copyright (c) 2026 DigiPen (USA) Corporation
 *
 */
#include "pch.h"
#include "SerializationUtility.h"

// other includes
#include "JsonAbstraction.h"
#include "FileManager.h"

namespace mts
{
    namespace SerializationUtility
    {
        void DeserializeFromFile(Deserializable* item, std::string const& filepath)
        {
            mts::JsonDocument doc;
            if (!doc.LoadFromFile(FileManager::Resolve(filepath)))
            {
                LOG_WARN("Failed to deserialize file: {}", filepath);
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

            return doc.SaveToFile(FileManager::Resolve(path));
        }

        std::string SerializeToString(Deserializable* item)
        {
            JsonDocument doc;

            // Build JSON document from object
            item->Serialize(doc);

            // Convert document to string
            std::string result;
            if (!doc.SaveToString(result))
            {
                LOG_INFO("String serialization error occurred\n");
                return {};
            }

            return result;
        }
    }  // namespace SerializationUtility
}  // namespace mts