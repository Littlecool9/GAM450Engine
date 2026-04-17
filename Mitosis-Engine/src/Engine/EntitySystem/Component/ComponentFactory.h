/**
 * @file ComponentFactory.h
 * @author Dong Ta
 * @brief  Utility class to create components dynamically
 * @version 0.1
 * @date 1/15/2026 6:55:28 AM
 *
 * @copyright Copyright (c) 2026 DigiPen (USA) Corporation
 *
 */

#pragma once
#include <functional>

#pragma once
#include "unordered_map"
#include "Component.h"
namespace mts
{
    class Component;
    class ComponentFactory
    {
       private:
        typedef std::function<Component*()> ComponentCreator;

       public:
        // @brief: Create a component based on its string name
        // @brief: string-based creation requires manual registration of new component types in the
        // cpp file
        // @brief: string-based creation is mainly used for deserialization from files
        template <typename... Args>
        static Component* CreateComponent(const std::string& componentName, Args&&... args)
        {
            if (componentCreators.find(componentName) != componentCreators.end())
            {
                ComponentCreator constructor = componentCreators.at(componentName);
                return constructor(std::forward<Args>(args)...);
            }
            return nullptr;
        }

        // @brief: Create a component based on its type
        // @brief: type-based creation does not require manual registration of new component types
        // @brief: type-based creation is mainly used for gameplay logic
        template <typename T, typename... Args>
        static T* CreateComponent(Args&&... args)
        {
            return new T(std::forward<Args>(args)...);
        }

        static bool RegisterComponent(const std::string& componentName, ComponentCreator creator);
        static bool IsRegistered(const std::string& objectName)
        {
            return componentCreators.find(objectName) != componentCreators.end();
        }

       private:
        static std::unordered_map<std::string, ComponentCreator> componentCreators;
    };

}  // namespace mts