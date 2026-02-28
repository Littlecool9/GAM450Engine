/**
 * @file ComponentDrawer.h
 * @author sumin.park
 * @brief  interface for component drawers
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#pragma once
#include <string>

namespace mts
{
    class GameObject;
    class ComponentDrawer
    {
       public:
        virtual ~ComponentDrawer() = default;
        virtual const char* GetName() const = 0;
        virtual bool Draw(GameObject* go) = 0;
    };

}  // namespace mts