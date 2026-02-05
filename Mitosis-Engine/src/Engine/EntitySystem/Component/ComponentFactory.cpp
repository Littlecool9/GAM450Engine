/**
 * @file ComponentFactory.cpp
 * @author Dong Ta
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
#include "Button.h"
#include "Camera/Camera.h"
#include "Camera/CameraFollow.h"
#include "PhysicsComponents/RigidBody2D.h"
#include "PhysicsComponents/BoxCollider2D.h"
#include "PhysicsComponents/CircleCollider2D.h"
#include "Audio/AudioEmitter.h"
#include "Audio/AudioListener.h"

namespace mts
{
    std::unordered_map<std::string, ComponentFactory::ComponentCreator>
        ComponentFactory::componentCreators = {
            {"Transform", []() -> Component* { return new Transform(); }},
            {"MeshRenderer", []() -> Component* { return new MeshRenderer(); }},
            {"ActionList", []() -> Component* { return new ActionList(); }},
            {"Button", []() -> Component* { return new Button(); }},
            {"RigidBody2D", []() -> Component* { return new RigidBody2D(); }},
            {"BoxCollider2D", []() -> Component* { return new BoxCollider2D(); }},
            {"CameraComponent", []() -> Component* { return new CameraComponent(); }},
            {"CameraFollow", []() -> Component* { return new CameraFollow(); }},
            {"AudioListener", []() -> Component* { return new AudioListener(); }},
            {"AudioEmitter", []() -> Component* { return new AudioEmitter(); }},
            {"CircleCollider2D", []() -> Component* { return new CircleCollider2D(); }}};

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