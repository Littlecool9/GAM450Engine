/**
 * @file UUID.h
 * @author Jackey Cen
 * @brief Header file for UUID class
 *
 * @copyright Copyright (c) 2025 - MTS Engine
 *
 */

#pragma once
#include <string>
#include <functional>
#include <cstdint>

namespace mts
{
    // TODO: Determined whether uuid needs to extends from serializable / deserializable
    class UUID
    {
       public:
        UUID(const UUID&) = default;
        UUID& operator=(const UUID&) = default;
        // used for deserialization
        explicit UUID(const std::string& uuidStr);

        UUID(UUID&&) noexcept = default;
        UUID& operator=(UUID&&) noexcept = default;

        bool IsValid() const;
        std::string ToString() const;

        static UUID FromString(const std::string& uuidStr);

        static UUID GenerateRandom();
        // Generates a nil UUID (all zeros)
        // useful for default or placeholder initialization
        static UUID GenerateNil();

        bool operator==(const UUID& other) const noexcept;
        bool operator!=(const UUID& other) const noexcept;

       private:
        friend struct std::hash<UUID>;
        friend struct UUIDPairKey;

        UUID() = default;

        // avoid including boost headers in header files
        uint64_t m_High = 0;
        uint64_t m_Low = 0;
    };
}  // namespace mts

namespace std
{
    template <>
    struct hash<mts::UUID>
    {
        size_t operator()(const mts::UUID& id) const noexcept
        {
            return hash<uint64_t>()(id.m_High) ^ (hash<uint64_t>()(id.m_Low) << 1);
        }
    };
}  // namespace std
