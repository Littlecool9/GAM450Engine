#pragma once
#include "Engine/Scripting/ScriptBase.h"

namespace mts
{
    class Transform;
}

class BeatBarController : public mts::ScriptBase
{
   public:
    void OnCreate() override;
    void OnUpdate(float dt) override;

    void DeserializeFields(mts::JsonValue const& /*fields*/) override {}
    void SerializeFields(mts::JsonDocument& /*out*/) const override {}

    SCRIPT_CLASS(BeatBarController)
   private:
    // Half the viewport width in world units: 1600px / 80 PPU / 2
    static constexpr float kHalfViewWidth = 10.0f;
    // Y offset from camera center to the beat-circle on the beatline image.
    // The circle sits 130px above the bottom of the 900px viewport:
    //   bottom = -5.625 units, circle Y = -5.625 + (130/80) = -4.0 units
    static constexpr float kBeatCircleOffsetY = -4.0f;

    mts::Transform* mLeftMarkerTransform = nullptr;
    mts::Transform* mRightMarkerTransform = nullptr;
};
