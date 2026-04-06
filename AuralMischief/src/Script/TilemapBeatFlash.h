/**
 * @file TilemapBeatFlash.h
 * @author sumin.park
 * @brief  makes tilemap flash on given beat
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#pragma once
#include "Engine/Scripting/ScriptBase.h"
#include "Engine/Conductor/ConductorSystem.h"
#include "Engine/Tilemap/Tilemap.h"

class TilemapBeatFlash : public mts::ScriptBase
{
   public:
    SCRIPT_CLASS(TilemapBeatFlash)
    void Init() override;
    void Start() override;
    void OnUpdate(float dt) override;
    void OnDisable() override;

    void DeserializeFields(mts::JsonValue const& fields) override;
    void SerializeFields(mts::JsonDocument& out) const override;
    void GetInspectorFields(std::vector<mts::ScriptField>& fields) override;

   private:
    void OnBeat(mts::BeatInfo info);

   private:
    mts::Tilemap::Tilemap* mTilemap = nullptr;

    glm::vec3 mFlashColor{1.0f, 1.0f, 1.0f};
    float mFlashDuration = 0.12f;
    int mBeatInterval = 1;
    bool mHoldTint = false;

    float mFlashTimer = 0.f;
    bool mTintActive = false;
    mts::Delegate<mts::BeatInfo>::Id mBeatDelegateId = 0;
};
