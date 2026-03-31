/**
 * @file BeatPulseScript.h
 * @brief Scales a sprite with exponential decay on every beat:
 *        s(t) = 1 + A * exp(-k * t), applied uniformly on X and Y.
 */
#pragma once

#include "Engine/Scripting/ScriptBase.h"
#include "Engine/Conductor/ConductorSystem.h"
#include <glm/glm.hpp>

class BeatPulseScript : public mts::ScriptBase
{
   public:
    void Init() override;
    void Awake() override;
    void OnUpdate(float dt) override;
    void OnDisable() override;
    ~BeatPulseScript();

    void DeserializeFields(mts::JsonValue const& fields) override;
    void SerializeFields(mts::JsonDocument& out) const override;
    void GetInspectorFields(std::vector<mts::ScriptField>& fields) override;

    SCRIPT_CLASS(BeatPulseScript)

   private:
    void OnBeat(mts::BeatInfo info);

    mts::UITransform* mTransform = nullptr;
    glm::vec3 mBaseScale = {1.f, 1.f, 1.f};

    mts::Delegate<mts::BeatInfo>::Id mBeatSubId = 0;

    float mAmplitude = 0.25f;  // A  — peak overshoot above 1.0
    float mDecay = 10.0f;      // k  — decay speed (higher = faster return)

    float mXPos = 0.0f;
    float mYPos = 0.0f;

    float mTimer = 0.0f;
    bool mAnimating = false;
};
