/**
 * @file BarScaler.cpp
 * @author cyclo
 * @brief // TODO: Add a brief description of this class
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#include "pch.h"
#include "BarScaler.h"
#include "Engine/EntitySystem/Component/Transform.h"
#include "Engine/EntitySystem/GameObject/GameObject.h"
namespace mts
{
	void BarScaler::OnCreate()
	{
		OriginalPosition = GetGameObject()->GetComponent<Transform>()->GetPosition();
        glm::vec3 scale = GetGameObject()->GetComponent<Transform>()->GetScale();
        MaxRealScale = scale.x;
    }

	void BarScaler::OnUpdate(float dt)
	{
		if (InterpolationTime > 0)
		{

            InterpolationTime = glm::clamp(InterpolationTime - dt, 0.f, 1.f);
            SetScaleStandard(TargetScale +
                                 ((CurrentScale - TargetScale) * InterpolationTime) / ScaleTime);
		}
    }

	void BarScaler::DeserializeFields(JsonValue const& fields)
	{
		if (fields.Has("Rotated"))
		{
			Rotated = fields["Rotated"].GetBool();
		}
    }

	void BarScaler::SerializeFields(mts::JsonDocument& out) const
	{
    }

    void BarScaler::SetScaleRotated(float newScale)
    {
        
    }

    void BarScaler::SetScaleStandard(float newScale)
    {
        // We create a new Vector3 and only "replace" the X value, then assign the new vector to
        // the localScale property because the individual values are not writable (a common pattern
        // in Unity).
        GetGameObject()->GetComponent<Transform>()->SetScale(
            glm::vec3{MaxRealScale * newScale,
                      GetGameObject()->GetComponent<Transform>()->GetScale().y,
                      GetGameObject()->GetComponent<Transform>()->GetScale().z});
        //// The actual position of the object needs to be moved to the left by half the reduction in
        //// scale because we want it to be anchor on the left (while the position is the center). But
        //// because this object (or usually its parent) is rotated, we need to move it in the Y
        //// direction (in local space) instead of the X direction.
        float positionAdjustment = (MaxRealScale - MaxRealScale * newScale) / 2.0f;
        //// We create a new Vector3 and only "replace" the Y value, then assign the new vector to
        //// the position
        GetGameObject()->GetComponent<Transform>()->SetPosition(
            glm::vec3{OriginalPosition.x - positionAdjustment,
                      OriginalPosition.y,
                      OriginalPosition.z});
        //GetGameObject()->GetComponent<Transform>()->SetRotation( -90.f);
        
    }

    void BarScaler::InterpolateToScale(float percent, float time)
    {
        CurrentScale = GetGameObject()->GetComponent<Transform>()->GetScale().x;
        // Determine the non-normalized target scale.
        TargetScale = percent * MaxRealScale;
        // Store the total interpolation time (with some padding in case a 0.0 value is used).
        ScaleTime = time + 0.001f;
        // Set the interpolation time counter.
        InterpolationTime = time + 0.001f;
    }
}  // namespace mts