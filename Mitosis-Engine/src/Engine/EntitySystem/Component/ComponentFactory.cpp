/**
 * @file ComponentFactory.cpp
 * @author Jackey Cen
 * @brief: Component Factory Implementation
 *
 * @copyright Copyright (c) 2025 - Mitosis Engine
 *
 */

#include "pch.h"
#include "ComponentFactory.h"
// Include all component headers here
#include "ActionList.h"
#include "Transform.h"
#include "MeshRenderer.h"

namespace mts
{
    std::unordered_map<std::string, ComponentFactory::ComponentCreator>
        ComponentFactory::componentCreators = {
            {"Transform", []() -> Component* { return MTS_NEW (Transform); }},
            {"MeshRenderer", []() -> Component* { return MTS_NEW (MeshRenderer); }},
            {"ActionList", []() -> Component* { return MTS_NEW (ActionList); }},
    };

    bool ComponentFactory::RegisterComponent(const std::string& componentName,
                                             ComponentCreator creator)
    {
        if (IsRegistered(componentName))
        {
            return false;
        }
        componentCreators[componentName] = creator;
        return true;
    }
}  // namespace mts