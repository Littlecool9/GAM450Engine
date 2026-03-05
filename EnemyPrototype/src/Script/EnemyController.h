/**
 * @file EnemyController.h
 * @author sumin.park
 * @brief  Enemy behavior script
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#pragma once
#include "Engine/Scripting/ScriptBase.h"

namespace mts
{
    class EnemyController : public ScriptBase
    {
       public:
        void OnCreate() override;
        void OnUpdate(float dt) override;
        void DeserializeFields(JsonValue const& fields) override;
        void SerializeFields(JsonDocument& out) const override;
        void OnCollisionEnter2D(const Collision2D& collision) override;

        void SetBar(GameObject* bar) { hpBar = bar; }

        SCRIPT_CLASS(EnemyController)

       private:
        GameObject* hpBar = nullptr;
        int maxhp = 10;
        int curhp = 0;
    };

}
