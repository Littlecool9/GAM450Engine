/**
 * @file BarScaler.h
 * @author cyclo
 * @brief // TODO: Add a brief description of this header
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#pragma once
#include "Engine/Scripting/ScriptBase.h"

namespace mts
{
    class BarScaler : public ScriptBase
    {
       public:
        void OnCreate() override;
        void OnUpdate(float dt) override;
        void DeserializeFields(JsonValue const& fields) override;
        void SerializeFields(mts::JsonDocument& out) const override;


        void SetScaleStandard(float newScale);
        void SetScaleRotated(float newScale);
        void InterpolateToScale(float percent, float time);
        SCRIPT_CLASS(BarScaler)
       private:
        bool Rotated = false;  // If the object is rotated, this flag needs to be set

        // All of these variables are used to track the scaling, position, and interpolation
        float MaxRealScale = 0;

        glm::vec3 OriginalPosition;

        float CurrentScale = 1.0f;

        float TargetScale = 1.0f;

        float ScaleTime = 0.0f;

        float InterpolationTime = 0.2f;
    };

}  // namespace mts