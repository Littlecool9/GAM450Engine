/**
 * @file UUID.cpp
 * @author Jackey Cen
 * @brief Implementation file for UUID class
 *
 * @copyright Copyright (c) 2025 - Mitosis Engine
 *
 */

#include "pch.h"
#include "UUID.h"
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

namespace mts
{
    static boost::uuids::random_generator s_Generator;
    UUID UUID::InvalidID = UUID::GenerateNil();

    bool UUID::IsValid() const
    {
        return mHigh != 0 || mLow != 0;
    }

    bool UUID::operator==(const UUID& other) const noexcept
    {
        return mHigh == other.mHigh && mLow == other.mLow;
    }

    bool UUID::operator!=(const UUID& other) const noexcept
    {
        return !(*this == other);
    }

    UUID UUID::FromString(const std::string& str)
    {
        if (str.empty())
            return UUID::InvalidID;
        boost::uuids::string_generator gen;
        boost::uuids::uuid u = gen(str);

        UUID id;
        std::memcpy(&id.mHigh, u.data, 8);
        std::memcpy(&id.mLow, u.data + 8, 8);
        return id;
    }

    std::string UUID::ToString() const
    {
        boost::uuids::uuid u{};
        std::memcpy(u.data, &mHigh, 8);
        std::memcpy(u.data + 8, &mLow, 8);
        return boost::uuids::to_string(u);
    }

    UUID UUID::GenerateRandom()
    {
        UUID result;
        boost::uuids::uuid u = s_Generator();
        std::memcpy(&result.mHigh, u.data, 8);
        std::memcpy(&result.mLow, u.data + 8, 8);
        return result;
    }

    UUID UUID::GenerateNil()
    {
        return UUID{};
    }

}  // namespace mts