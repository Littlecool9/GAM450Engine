/**
 * @file Entity.h
 * @author Jackey Cen
 * @brief: Header file for Entity class
 * @brief: This is a base class for all entities in the engine
 * @brief: This class holds the UUID property
 * @copyright Copyright (c) 2026 DigiPen (USA) Corporation
 *
 */

#pragma once
#include "Engine/UUID/UUID.h"

namespace mts
{
    class Entity
    {
       public:
        Entity();
        UUID GetId() const;
        void RegenerateId() { _id = UUID::GenerateRandom(); }

       protected:
        //@brief: Unique identifier for the entity
        //@brief: UUID lifecycle depends on the derived class instance
        UUID _id;
    };

}  // namespace mts