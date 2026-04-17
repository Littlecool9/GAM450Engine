/**
 * @file ScriptFactory.cpp
 * @author rahul
 * @brief Global registry mapping script class names to native C++ script factory functions.
 * @copyright Copyright (c) 2026 DigiPen (USA) Corporation
 *
 */

#include "pch.h"
#include "ScriptFactory.h"

namespace mts
{
    std::unordered_map<std::string, ScriptFactory::ScriptCreator> ScriptFactory::s_creators;
    std::vector<std::string> ScriptFactory::s_sortedNames;

    bool ScriptFactory::RegisterScript(const std::string& className, ScriptCreator creator)
    {
        if (IsRegistered(className))
            return false;

        s_creators[className] = std::move(creator);
        auto it = std::lower_bound(s_sortedNames.begin(), s_sortedNames.end(), className);
        s_sortedNames.insert(it, className);
        return true;
    }

    bool ScriptFactory::IsRegistered(const std::string& className)
    {
        return s_creators.contains(className);
    }

    ScriptBase* ScriptFactory::Create(const std::string& className)
    {
        auto it = s_creators.find(className);
        if (it == s_creators.end())
            return nullptr;
        return (it->second)();
    }

    const std::vector<std::string>& ScriptFactory::GetRegisteredNames()
    {
        return s_sortedNames;
    }
}  // namespace mts