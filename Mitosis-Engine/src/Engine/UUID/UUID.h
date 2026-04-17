/**
 * @file UUID.h
 * @author Jackey Cen
 * @brief Header file for UUID class
 *
 * @copyright Copyright (c) 2026 DigiPen (USA) Corporation
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

        UUID(UUID&&) noexcept = default;
        UUID& operator=(UUID&&) noexcept = default;

        bool operator==(const UUID& other) const noexcept;
        bool operator!=(const UUID& other) const noexcept;

        bool IsValid() const;
        std::string ToString() const;
        uint64_t High() const { return mHigh; }
        uint64_t Low() const { return mLow; }

        static UUID FromString(const std::string& uuidStr);

        static UUID GenerateRandom();
        // Generates a nil UUID (all zeros)
        // useful for default or placeholder initialization
        static UUID GenerateNil();

        static UUID InvalidID;

       private:
        friend struct std::hash<UUID>;
        friend struct UUIDPairKey;

        UUID() = default;

        // avoid including boost headers in header files
        uint64_t mHigh = 0;
        uint64_t mLow = 0;
    };
}  // namespace mts

namespace std
{
    template <>
    struct hash<mts::UUID>
    {
        size_t operator()(const mts::UUID& id) const noexcept
        {
            return hash<uint64_t>()(id.mHigh) ^ (hash<uint64_t>()(id.mLow) << 1);
        }
    };
}  // namespace std
