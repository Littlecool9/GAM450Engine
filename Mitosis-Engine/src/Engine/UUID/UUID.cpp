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

    UUID::UUID(const std::string& uuidStr)
    {
        *this = FromString(uuidStr);
    }

    bool UUID::IsValid() const
    {
        return m_High != 0 || m_Low != 0;
    }

    bool UUID::operator==(const UUID& other) const noexcept
    {
        return m_High == other.m_High && m_Low == other.m_Low;
    }

    bool UUID::operator!=(const UUID& other) const noexcept
    {
        return !(*this == other);
    }

    UUID UUID::FromString(const std::string& str)
    {
        boost::uuids::string_generator gen;
        boost::uuids::uuid u = gen(str);

        UUID id;
        std::memcpy(&id.m_High, u.data, 8);
        std::memcpy(&id.m_Low, u.data + 8, 8);
        return id;
    }

    std::string UUID::ToString() const
    {
        boost::uuids::uuid u{};
        std::memcpy(u.data, &m_High, 8);
        std::memcpy(u.data + 8, &m_Low, 8);
        return boost::uuids::to_string(u);
    }

    UUID UUID::GenerateRandom()
    {
        UUID result;
        boost::uuids::uuid u = s_Generator();
        std::memcpy(&result.m_High, u.data, 8);
        std::memcpy(&result.m_Low, u.data + 8, 8);
        return result;
    }

    UUID UUID::GenerateNil()
    {
        return UUID{};
    }

}  // namespace mts