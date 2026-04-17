/**
 * @file DamageNumberScript.h
 * @author sumin.park
 * @brief  Animates a single digit sprite in a configurable direction while fading out, then
 * disables itself
 *
 * @copyright (c) 2026 DigiPen (USA) Corporation
 *
 */

#pragma once
#include "Engine/Scripting/ScriptBase.h"
#include <glm/vec3.hpp>

namespace mts
{
    class Transform;
    class SpriteRenderer;
}  // namespace mts

class DamageNumberScript : public mts::ScriptBase
{
   public:
    void Init() override;
    void OnEnable() override;
    void OnUpdate(float dt) override;
    void DeserializeFields(mts::JsonValue const& fields) override;
    void SerializeFields(mts::JsonDocument& out) const override;
    void GetInspectorFields(std::vector<mts::ScriptField>& fields) override;

    void Activate(int digitVal, glm::vec3 pos);
    void ApplyMotion(float speed, float dirX, float dirY);

    SCRIPT_CLASS(DamageNumberScript)

   private:
    mts::Transform* mTransform = nullptr;
    mts::SpriteRenderer* mSprite = nullptr;

    float mFloatSpeed = 1.5f;
    float mDuration = 0.8f;
    float mFloatDirX = 0.0f;
    float mFloatDirY = 1.0f;

    float mElapsed = 0.f;
};
