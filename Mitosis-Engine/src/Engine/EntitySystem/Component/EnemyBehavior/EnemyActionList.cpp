/**
 * @file EnemyActionList.cpp
 * @author Dong Ta
 * @brief // TODO: Add a brief description of this class
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#include "pch.h"
#include "EnemyActionList.h"

namespace mts
{
    // in case new code doesnt work, this will be uncommented and the other block will be removed
    //const ActionRegistry& EnemyActionList::Registry()
    //{
    //    // “construct on first use” 
    //    static const ActionRegistry reg = {
    //        //Add actions we want the specified enemies to be able to perform
    //        {"Enemy1",
    //         {
    //             /*[] { return std::make_unique<EnemyAction1>(); },
    //            [] { return std::make_unique<EnemyAction2>(); },*/
    //         }},
    //        {"Enemy2",
    //         {
    //             /*[] { return std::make_unique<EnemyAction1>(); },
    //             [] { return std::make_unique<EnemyAction2>(); },*/

    //         }},
    //    };

    //    return reg;
    //}
    ActionRegistry& EnemyActionList::Registry()
    {
        static ActionRegistry reg;  
        return reg;
    }

    void EnemyActionList::Register(const std::string& plannerId, ActionFactory factory)
    {
        Registry()[plannerId].push_back(std::move(factory));
    }
}  // namespace mts
