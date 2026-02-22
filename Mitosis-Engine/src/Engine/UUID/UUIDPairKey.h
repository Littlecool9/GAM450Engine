/**
 * @file UUIDPairKey.h
 * @author sumin.park
 * @brief  Helper for order-independent pairs of UUIDs
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#pragma once
#include <functional>
#include "UUID.h"

namespace mts
{
    struct UUIDPairKey
    {
        UUID lo;
        UUID hi;

        UUIDPairKey() = default;

        UUIDPairKey(const UUID& a, const UUID& b)
        {
            if (UUIDPairKey::CompareUUID(b, a))
            {
                lo = b;
                hi = a;
            }
            else
            {
                lo = a;
                hi = b;
            }
        }

        bool operator==(const UUIDPairKey& other) const noexcept
        {
            return lo == other.lo && hi == other.hi;
        }

        static bool CompareUUID(const UUID& lhs, const UUID& rhs) noexcept
        {
            // Lexicographic compare on (high, low)
            if (lhs.mHigh != rhs.mHigh)
                return lhs.mHigh < rhs.mHigh;
            return lhs.mLow < rhs.mLow;
        }
    };

}  // namespace mts

namespace std
{
    template <>
    struct hash<mts::UUIDPairKey>
    {
        size_t operator()(const mts::UUIDPairKey& k) const noexcept
        {
            size_t seed = 0;
            const size_t h1 = std::hash<mts::UUID>{}(k.lo);
            const size_t h2 = std::hash<mts::UUID>{}(k.hi);

            // hash_combine
            seed ^= h1 + 0x9e3779b97f4a7c15ULL + (seed << 6) + (seed >> 2);
            seed ^= h2 + 0x9e3779b97f4a7c15ULL + (seed << 6) + (seed >> 2);
            return seed;
        }
    };
}  // namespace std