/**
 * @file AudioListener.h
 * @author Dong Ta
 * @brief Component to allow gameobjects to detect/listen to audio
 *        
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */




#pragma once

#include "Engine/EntitySystem/Component/Component.h"
#include "Engine/Audio/AudioTypes.h"
namespace mts
{
    class Transform;
    class AudioManager;
    class AudioListener : public Component
    {
    public:
        void Init() override;
        void Update(float dt) override;
        void Deserialize(JsonValue const& jsonObj) override;
        void Serialize(JsonDocument& doc) const override;
        void LateUpdate(float dt) override;
        void Render() override;
    private:
        bool mFirstFrame = true;
        glm::vec2 mPrevPos{0.f, 0.f};

        Transform* GetTransform() const;

    };

}
