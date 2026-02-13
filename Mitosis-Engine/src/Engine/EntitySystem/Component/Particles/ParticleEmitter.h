/**
 * @file ParticleEmitter.h
 * @author josh.siegel
 * @brief // This component class is responsible for emitting particles.
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#pragma once
#include "Engine/EntitySystem/Component/Component.h"
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

namespace mts
{
    // Forward Declaration
    struct Particle;
    class ParticleContainer;
    class Mesh;
    class Texture;
    class Shader;

    class ParticleEmitter : public Component
    {
        MTS_CLASS_NEW_DELETE();
       public:
        ParticleEmitter(GameObject* owner);
        ParticleEmitter() = default;
        ~ParticleEmitter() override;

        void Init() override;
        void Update(float dt) override;
        void LateUpdate(float dt) override;
        void Render() override;
        void DrawParticles();
        void Deserialize(JsonValue const& jsonObj) override;
        void Serialize(JsonDocument& doc) const override;

        void SetFrame(int frameIndex);
        void EmitParticle();
        void ForEachParticle(std::function<void(Particle& particle, float dt)> const& lambda,
                             float dt);

        glm::vec2 GetAcceleration() const { return mAcceleration; }

        void ToggleEmitter() { mIsRunning ^= true; }

       private:
        Mesh* mMesh{nullptr};
        Texture* mTexture{nullptr};
        Shader* mShader{nullptr};
        float mAlpha{1.0f};
        glm::vec3 mTintColor{1.0f, 1.0f, 1.0f};
        glm::vec2 mUvOffset{0.0f, 0.0f};
        int mNumRows = 1;
        int mNumCols = 1;
        int mFrameIndex = 0;

        glm::vec2 mEmitterScale{0.0f, 0.0f};
        glm::vec2 mScaleMin{1.0f, 1.0f};
        glm::vec2 mScaleMax{1.0f, 1.0f};
        glm::vec2 mAcceleration{0.0f, 0.0f};
        glm::vec2 mDirectionRange{0.0f, 6.28f};

        float mEmitAccumulator{0.0f};
        float mEmitRate{1.0f};
        unsigned mEmitMax{250};
        float mLifetimeMin{1.0f};
        float mLifetimeMax{1.0f};
        float mSpeedMin{1.0f};
        float mSpeedMax{1.0f};
        float mRotationSpeedMin{0.0f};
        float mRotationSpeedMax{0.0f};
        float mRotationMin{0.0f};
        float mRotationMax{0.0f};
        bool mOneShot{false};
        bool mIsRunning{true};

        ParticleContainer* mContainer{};
    };

}  // namespace mts