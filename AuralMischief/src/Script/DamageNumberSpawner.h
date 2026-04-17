/**
 * @file DamageNumberSpawner.h
 * @author sumin.park
 * @brief  Manages a pool of DamageNumber objects and spawns multi-digit damage numbers on hit
 *
 * @copyright (c) 2026 DigiPen (USA) Corporation
 *
 */

#pragma once
#include "Engine/Scripting/ScriptBase.h"
#include "DamageNumberScript.h"
#include <glm/vec3.hpp>
#include <vector>

namespace mts
{
    class GameObject;
}

class DamageNumberSpawner : public mts::ScriptBase
{
   public:
    void OnUpdate(float dt) override;
    void DeserializeFields(mts::JsonValue const& fields) override;
    void SerializeFields(mts::JsonDocument& out) const override;
    void GetInspectorFields(std::vector<mts::ScriptField>& fields) override;

    void Spawn(float damage, glm::vec3 pos);

    SCRIPT_CLASS(DamageNumberSpawner)

   private:
    void BuildPool();

    int mPoolSize = 8;
    float mSpawnOffsetX = 0.0f;
    float mSpawnOffsetY = 0.5f;
    float mDigitSpacing = 0.3f;
    float mDigitScale = 1.0f;
    float mFloatSpeed = 1.5f;
    float mFloatDirX = 0.0f;
    float mFloatDirY = 1.0f;
    bool mPoolBuilt = false;

    std::vector<mts::GameObject*> mPool;
    std::vector<DamageNumberScript*> mScripts;
};
