/**
 * @file ComponentFactory.cpp
 * @author Dong Ta
 * @brief  Contains definitions for ComponentFactory class
 * @version 0.1
 * @date 1/15/2026 6:55:28 AM
 *
 * @copyright Copyright (c) 2025 - Mitosis Engine
 *
 */

#include "pch.h"
#include "ComponentFactory.h"
// Include all component headers here
#include "Transform.h"
#include "MeshRenderer.h"
namespace mts
{
    ComponentFactory::ComponentFactory()
    {
        // Whenever a new component is created, remember to register it here,
        // This assumes component constructors take in a gameobject as parameter like in the
        // provided transform and meshrenderer examples
        // THIS IS CASE SENSITIVE
        Register("Transform", +[](GameObject* o) -> Component* { return new Transform(o); });

        Register("MeshRenderer", +[](GameObject* o) -> Component* { return new MeshRenderer(o); });
        //Register("ActionList", +[](GameObject* o) -> Component* { return new ActionList(o); });
    }
    ComponentFactory::~ComponentFactory()
    {
    }
}  // namespace mts