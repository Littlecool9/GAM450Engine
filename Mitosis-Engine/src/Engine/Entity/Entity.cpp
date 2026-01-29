/**
 * @file Entity.cpp
 * @author Jackey Cen
 * @brief: Implementation file for Entity class
 *
 * @copyright Copyright (c) 2025 - MTS Engine
 *
 */

#include "pch.h"
#include "Entity.h"

namespace mts
{
    Entity::Entity() : _id(UUID::GenerateRandom())
    {
    }

    UUID Entity::GetId() const
    {
        return _id;
    }

}  // namespace mts