/**
 * @file BeatBarController.h
 * @author dream
 * @brief  Positions a beatline overlay and animates multiple converging beat markers
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#pragma once
#include "Engine/Scripting/ScriptBase.h"
#include <vector>

namespace mts
{
    class UITransform;
    class SpriteRenderer;
}

class BeatBarController : public mts::ScriptBase
{
   public:
    SCRIPT_CLASS(BeatBarController)

    void Awake() override;
    void OnUpdate(float dt) override;

    void DeserializeFields(mts::JsonValue const& fields) override;
    void SerializeFields(mts::JsonDocument& out) const override;
    void GetInspectorFields(std::vector<mts::ScriptField>& fields) override;

   private:
    // Y position of the beat circle in normalized [0,1] screen coords (Y-down).
    // Circle sits 130px above the bottom of a 900px viewport: (900-130)/900.
    static constexpr float kBeatCircleNormY = 770.0f / 900.0f;

    // Number of marker pairs. Each bar takes mBarCount beats to travel edge→center,
    // with one bar arriving at center every beat. Higher = more bars, slower motion.
    int mBarCount = 4;

    // Fraction of the phase range from center at which alpha reaches 1.0.
    // 0.05 = fully opaque at 5% of half-screen width from center.
    float mAlphaFadeDistance = 0.05f;

    std::vector<mts::UITransform*>    mLeftMarkerTransforms;
    std::vector<mts::UITransform*>    mRightMarkerTransforms;
    std::vector<mts::SpriteRenderer*> mLeftMarkerRenderers;
    std::vector<mts::SpriteRenderer*> mRightMarkerRenderers;
};
