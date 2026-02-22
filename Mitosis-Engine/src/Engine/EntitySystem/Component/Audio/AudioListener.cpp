/**
 * @file AudioListener.cpp
 * @author Dong Ta
 * @brief Functionality for audio listener
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#include "pch.h"
#include "AudioListener.h"
#include "Engine/Audio/AudioManager.h"
#include "Engine/EntitySystem/Component/Transform.h"
#include "Engine/EntitySystem/GameObject/GameObject.h"

namespace mts
{

    Transform* AudioListener::GetTransform() const
    {
        if (GetGameObject()->GetComponent<Transform>())
        {
            return GetGameObject()->GetComponent<Transform>();
        }
        LOG_INFO("No transform found");
        return nullptr;
    }

    void AudioListener::Init()
    {
        if (auto* t = GetTransform())
        {
            glm::vec3 worldPos3 = t->GetPosition();
            glm::vec2 worldPos2{worldPos3.x, worldPos3.y};

            mPrevPos = worldPos2;
            mFirstFrame = true;

            // Initialize listener
            AudioManager::Get().SetListener2D(mPrevPos, glm::vec2{0.f, 0.f});
        }
    }

    void AudioListener::Deserialize(JsonValue const& jsonObj)
    {
        // no op comment
    }

    void AudioListener::Serialize(JsonDocument& doc) const
    {
        doc.WriteString("Type", "AudioListener");
    }

    void AudioListener::LateUpdate(float dt)
    {
        // no op comment
    }

    void AudioListener::Render()
    {
        // no op comment
    }

    void AudioListener::Update(float dt)
    {
        auto* t = GetTransform();
        if (!t)
            return;
        // TODO: Potentially change this to get world position depending on if we want to change
        // things Changing from 3d to 2d, can modify this in the future to do 3d
        glm::vec3 worldPos3 = t->GetPosition();
        glm::vec2 worldPos2{worldPos3.x, worldPos3.y};
        glm::vec2 currentPos = worldPos2;

        glm::vec2 vel{0.f, 0.f};
        if (!mFirstFrame && dt > 0.f)
        {
            vel = (currentPos - mPrevPos) / glm::clamp(dt, 0.0001f, dt);
        }

        AudioManager::Get().SetListener2D(currentPos, vel);

        mPrevPos = currentPos;
        mFirstFrame = false;
    }
}  // namespace mts