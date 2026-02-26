/**
 * @file InspectorRegistry.h
 * @author sumin.park
 * @brief  holds and calls inspector drawers
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#pragma once
#include "ComponentDrawer.h"

namespace mts
{
    class InspectorRegistry
    {
       public:
        template <typename T>
        void Add()
        {
            mDrawers.emplace_back(std::make_unique<T>());
        }

        void DrawAll(GameObject* go);

       private:
        std::vector<std::unique_ptr<ComponentDrawer>> mDrawers;
    };

}  // namespace mts