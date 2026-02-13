/**
 * @file ScriptFactory.h
 * @author rahul
 * @brief Global registry mapping script class names to native C++ script factory functions.
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#pragma once

namespace mts
{
    class ScriptBase;

    class ScriptFactory
    {
       public:
        using ScriptCreator = std::function<ScriptBase*()>;

        static bool RegisterScript(const std::string& className, ScriptCreator creator);
        static bool IsRegistered(const std::string& className);
        static ScriptBase* Create(const std::string& className);

       private:
        static std::unordered_map<std::string, ScriptCreator> s_creators;
    };

}  // namespace mts