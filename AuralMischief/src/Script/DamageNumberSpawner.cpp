/**
 * @file DamageNumberSpawner.cpp
 * @author sumin.park
 * @brief  Manages a pool of DamageNumber GOs and spawns multi-digit damage numbers on hit
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#include "pch.h"
#include "DamageNumberSpawner.h"

#include "Engine/EntitySystem/GameObject/GameObjectFactory.h"
#include "Engine/EntitySystem/GameObject/GameObject.h"
#include "Engine/EntitySystem/Component/Transform.h"
#include <string>
#include <algorithm>

void DamageNumberSpawner::OnUpdate(float /*dt*/)
{
    if (!mPoolBuilt)
        BuildPool();
}

void DamageNumberSpawner::BuildPool()
{
    mPool.reserve(mPoolSize);
    mScripts.reserve(mPoolSize);

    for (int i = 0; i < mPoolSize; ++i)
    {
        mts::GameObject* go =
            mts::GameObjectFactory::CreateFromArchetype("Data/DamageNumber.prefab");
        go->SetName("DamageNumber_" + std::to_string(i));
        mPool.push_back(go);
        mScripts.push_back(go->GetScript<DamageNumberScript>());
        go->SetActive(false);
    }
    mPoolBuilt = true;
    LOG_INFO("[DamageNumberSpawner] Pool ready - {} slots created", mPoolSize);
}

void DamageNumberSpawner::Spawn(float damage, glm::vec3 pos)
{
    pos.x += mSpawnOffsetX;
    pos.y += mSpawnOffsetY;
    LOG_INFO("[DamageNumberSpawner] Spawn called - damage={} at ({:.2f},{:.2f},{:.2f})",
             damage,
             pos.x,
             pos.y,
             pos.z);

    std::string digits = std::to_string(std::max(0, static_cast<int>(damage)));
    int numDigits = static_cast<int>(digits.size());
    float startX = pos.x - (numDigits - 1) * mDigitSpacing * 0.5f;

    for (int i = 0; i < numDigits; ++i)
    {
        int digitVal = digits[i] - '0';
        bool placed = false;

        for (int j = 0; j < static_cast<int>(mPool.size()); ++j)
        {
            if (!mPool[j]->IsActive())
            {
                glm::vec3 spawnPos = {startX + i * mDigitSpacing, pos.y, pos.z};
                mScripts[j]->ApplyMotion(mFloatSpeed, mFloatDirX, mFloatDirY);
                mScripts[j]->Activate(digitVal, spawnPos);
                if (auto* tr = mPool[j]->GetComponent<mts::Transform>())
                    tr->SetScale({mDigitScale, mDigitScale, 1.f});
                mPool[j]->SetActive(true);
                LOG_INFO("[DamageNumberSpawner] {} activated - digit={} at ({:.2f},{:.2f},{:.2f})",
                         mPool[j]->GetName(),
                         digitVal,
                         spawnPos.x,
                         spawnPos.y,
                         spawnPos.z);
                placed = true;
                break;
            }
        }

        if (!placed)
        {
            LOG_WARN("[DamageNumberSpawner] Pool exhausted — digit {} dropped", i);
            break;
        }
    }
}

void DamageNumberSpawner::DeserializeFields(mts::JsonValue const& fields)
{
    if (fields.Has("poolSize"))
        mPoolSize = fields["poolSize"].GetInt(mPoolSize);
    if (fields.Has("spawnOffsetX"))
        mSpawnOffsetX = fields["spawnOffsetX"].GetFloat(mSpawnOffsetX);
    if (fields.Has("spawnOffsetY"))
        mSpawnOffsetY = fields["spawnOffsetY"].GetFloat(mSpawnOffsetY);
    if (fields.Has("digitSpacing"))
        mDigitSpacing = fields["digitSpacing"].GetFloat(mDigitSpacing);
    if (fields.Has("digitScale"))
        mDigitScale = fields["digitScale"].GetFloat(mDigitScale);
    if (fields.Has("floatSpeed"))
        mFloatSpeed = fields["floatSpeed"].GetFloat(mFloatSpeed);
    if (fields.Has("floatDirX"))
        mFloatDirX = fields["floatDirX"].GetFloat(mFloatDirX);
    if (fields.Has("floatDirY"))
        mFloatDirY = fields["floatDirY"].GetFloat(mFloatDirY);
}

void DamageNumberSpawner::SerializeFields(mts::JsonDocument& out) const
{
    out.WriteInt("poolSize", mPoolSize);
    out.WriteFloat("spawnOffsetX", mSpawnOffsetX);
    out.WriteFloat("spawnOffsetY", mSpawnOffsetY);
    out.WriteFloat("digitSpacing", mDigitSpacing);
    out.WriteFloat("digitScale", mDigitScale);
    out.WriteFloat("floatSpeed", mFloatSpeed);
    out.WriteFloat("floatDirX", mFloatDirX);
    out.WriteFloat("floatDirY", mFloatDirY);
}

void DamageNumberSpawner::GetInspectorFields(std::vector<mts::ScriptField>& fields)
{
    fields.push_back({.name = "Pool Size",
                      .type = mts::ScriptField::Type::Int,
                      .ptr = {.i = &mPoolSize},
                      .step = 1});
    fields.push_back({.name = "Spawn Offset X",
                      .type = mts::ScriptField::Type::Float,
                      .ptr = {.f = &mSpawnOffsetX},
                      .step = 0.1f});
    fields.push_back({.name = "Spawn Offset Y",
                      .type = mts::ScriptField::Type::Float,
                      .ptr = {.f = &mSpawnOffsetY},
                      .step = 0.1f});
    fields.push_back({.name = "Digit Spacing",
                      .type = mts::ScriptField::Type::Float,
                      .ptr = {.f = &mDigitSpacing},
                      .step = 0.05f});
    fields.push_back({.name = "Digit Scale",
                      .type = mts::ScriptField::Type::Float,
                      .ptr = {.f = &mDigitScale},
                      .step = 0.05f});
    fields.push_back({.name = "Float Speed",
                      .type = mts::ScriptField::Type::Float,
                      .ptr = {.f = &mFloatSpeed},
                      .step = 0.1f});
    fields.push_back({.name = "Float Dir X",
                      .type = mts::ScriptField::Type::Float,
                      .ptr = {.f = &mFloatDirX},
                      .step = 0.1f});
    fields.push_back({.name = "Float Dir Y",
                      .type = mts::ScriptField::Type::Float,
                      .ptr = {.f = &mFloatDirY},
                      .step = 0.1f});
}
