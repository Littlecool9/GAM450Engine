#include "pch.h"
#include "BeatBarController.h"

#include "Engine/Audio/AudioManager.h"
#include "Engine/Conductor/ConductorSystem.h"
#include "Engine/EntitySystem/Component/Transform.h"
#include "Engine/EntitySystem/GameObject/GameObjectFactory.h"
#include "Engine/Renderer.h"
#include "Engine/Serialization/SerializationUtility.h"

void BeatBarController::DeserializeFields(mts::JsonValue const& fields)
{
    if (fields.Has("SpeedMultiplier"))
        mSpeedMultiplier = fields["SpeedMultiplier"].GetFloat(mSpeedMultiplier);
}

void BeatBarController::SerializeFields(mts::JsonDocument& out) const
{
    out.WriteFloat("SpeedMultiplier", mSpeedMultiplier);
}

void BeatBarController::OnCreate()
{
    // Spawn the left beat marker
    mts::GameObject* leftMarker = mts::GameObjectFactory::CreateGameObject();
    mts::SerializationUtility::DeserializeFromFile(leftMarker, "Data/BeatMarker.prefab");
    mLeftMarkerTransform = leftMarker->GetComponent<mts::Transform>();

    // Spawn the right beat marker
    mts::GameObject* rightMarker = mts::GameObjectFactory::CreateGameObject();
    mts::SerializationUtility::DeserializeFromFile(rightMarker, "Data/BeatMarker.prefab");
    mRightMarkerTransform = rightMarker->GetComponent<mts::Transform>();
}

void BeatBarController::OnUpdate(float /*dt*/)
{
    const mts::RenderCamera& cam = mts::Renderer::GetActiveCamera();
    if (!cam.valid)
        return;

    const float camX = cam.position.x;
    const float camY = cam.position.y;

    // Keep the beatline overlay centered on the camera (vec2 preserves z=10 set in JSON)
    mts::Transform* beatlineTransform = GetGameObject()->GetComponent<mts::Transform>();
    beatlineTransform->SetPosition(glm::vec2(camX, camY));

    // Animate markers: phase 0 = at screen edges, phase 1 = at beat circle center.
    // Use bar-level phase (0..1 over all 4 beats) so markers travel across the
    // full screen over one measure instead of one beat -- much easier to track.
    mts::ConductorSystem& conductor = mts::ConductorSystem::Get();
    const int beatsPerBar = mts::AudioManager::Get().GetBeatsPerBar();
    float phase = (conductor.GetBeatInBar() + conductor.GetBeatPhase()) /
                  static_cast<float>(beatsPerBar);
    phase = std::fmod(phase * mSpeedMultiplier, 1.0f);
    float offsetX = kHalfViewWidth * (1.0f - phase);
    float markerY = camY + kBeatCircleOffsetY;

    // vec2 overload preserves z=11 set in BeatMarker.json
    mLeftMarkerTransform->SetPosition(glm::vec2(camX - offsetX, markerY));
    mRightMarkerTransform->SetPosition(glm::vec2(camX + offsetX, markerY));
}
