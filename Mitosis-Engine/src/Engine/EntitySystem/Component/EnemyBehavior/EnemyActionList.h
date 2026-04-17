/**
 * @file EnemyActionList.h
 * @author Dong Ta
 * @brief // TODO: Add a brief description of this header
 *
 * @copyright Copyright (c) 2026 DigiPen (USA) Corporation
 *
 */

#pragma once

// helper macros to concatenate tokens
// token concat helpers
#define MTS_CONCAT_IMPL(a, b) a##b
#define MTS_CONCAT(a, b) MTS_CONCAT_IMPL(a, b)

// Registration macro 
#define REGISTER_ENEMY_ACTION(plannerId, ActionType)                                     \
namespace                                                                                \
{                                                                                        \
    const bool MTS_CONCAT(_mts_registered_action_, __COUNTER__) = []()                   \
    {                                                                                    \
        mts::EnemyActionList::Register((plannerId),                                      \
                                       []() { return std::make_unique<ActionType>(); }); \
        return true;                                                                     \
    }();                                                                                 \
}

namespace mts
{
    class BaseEnemyAction;

    using ActionFactory = std::function<std::unique_ptr<BaseEnemyAction>()>;
    using ActionRegistry = std::unordered_map<std::string, std::vector<ActionFactory>>;

    struct EnemyActionList
    {
        static ActionRegistry& Registry();  // ← remove const
        static void Register(const std::string& plannerId, ActionFactory factory);
    };
}  // namespace mts

