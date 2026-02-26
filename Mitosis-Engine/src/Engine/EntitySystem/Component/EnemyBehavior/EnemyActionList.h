/**
 * @file EnemyActionList.h
 * @author Dong Ta
 * @brief // TODO: Add a brief description of this header
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#pragma once
#define REGISTER_ENEMY_ACTION(plannerId, ActionType)                                         \
namespace                                                                                \
{                                                                                        \
    const bool registered_##ActionType = []()                                            \
    {                                                                                    \
        mts::EnemyActionList::Register(plannerId,                                        \
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

