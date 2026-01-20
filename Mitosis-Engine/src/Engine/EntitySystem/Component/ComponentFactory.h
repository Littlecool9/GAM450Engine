/**
 * @file ComponentFactory.h
 * @author Dong Ta
 * @brief  Utility class to create components dynamically
 * @version 0.1
 * @date 1/15/2026 6:55:28 AM
 *
 * @copyright Copyright (c) 2025 - Mitosis Engine
 *
 */


#pragma once
#include "unordered_map"
#include "Component.h"
namespace mts
{
    class ComponentFactory
    {
        using CreatorFn = Component* (*)(GameObject*);
        public:
        ComponentFactory();
        ~ComponentFactory() = default;

        void Register(std::string name, CreatorFn fn)
        {
            creators_[std::move(name)] = fn;
        }
        //called to create a component. CASE SENSITIVE
        Component* Create(std::string_view type, GameObject* owner) const
        {
            auto it = creators_.find(std::string(type));

            if (it == creators_.end())
            {
                assert(false && "Component was not added to the factory.");
            }
            else
            {
                return it->second(owner);
            }
        }

        private:
        // a map of component type names to their constructor functions
        std::unordered_map<std::string, CreatorFn> creators_;
    };
}