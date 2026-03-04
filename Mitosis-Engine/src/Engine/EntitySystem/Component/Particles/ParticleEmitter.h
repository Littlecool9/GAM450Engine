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
#include "Engine/Interfaces/ISubmitRenderCommand.h"
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

    class ParticleEmitter : public Component, public ISubmitRenderCommand
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
        void Deserialize(JsonValue const& jsonObj) override;
        void Serialize(JsonDocument& doc) const override;

        void SubmitRenderCommand() const override;
        

        void SetFrame(int frameIndex);
        void EmitParticle();
        void ForEachParticle(std::function<void(Particle& particle, float dt)> const& lambda,
                             float dt);

        glm::vec2 GetAcceleration() const { return mAcceleration; }
        void SetAcceleration(glm::vec2 v) { mAcceleration = v; }

        void ToggleEmitter() { mIsRunning ^= true; }
        Component* Clone(GameObject* newOwner) const override;

        Mesh* GetMesh() const { return mMesh; }
        void SetMesh(Mesh* v) { mMesh = v; }
        Texture* GetTexture() const { return mTexture; }
        void SetTexture(Texture* v) { mTexture = v; }
        Shader* GetShader() const { return mShader; }
        void SetShader(Shader* v) { mShader = v; }
        float GetAlpha() const { return mAlpha; }
        void SetAlpha(float v) { mAlpha = v; }
        glm::vec3 GetTintColor() const { return mTintColor; }
        void SetTintColor(glm::vec3 v) { mTintColor = v; }
        int GetNumRows() const { return mNumRows; }
        void SetNumRows(int v) { mNumRows = v; }
        int GetNumCols() const { return mNumCols; }
        void SetNumCols(int v) { mNumCols = v; }
        glm::vec2 GetEmitterScale() const { return mEmitterScale; }
        void SetEmitterScale(glm::vec2 v) { mEmitterScale = v; }
        glm::vec2 GetScaleMin() const { return mScaleMin; }
        void SetScaleMin(glm::vec2 v) { mScaleMin = v; }
        glm::vec2 GetScaleMax() const { return mScaleMax; }
        void SetScaleMax(glm::vec2 v) { mScaleMax = v; }
        glm::vec2 GetDirectionRange() const { return mDirectionRange; }
        void SetDirectionRange(glm::vec2 v) { mDirectionRange = v; }
        float GetEmitRate() const { return mEmitRate; }
        void SetEmitRate(float v) { mEmitRate = v; }
        unsigned GetEmitMax() const { return mEmitMax; }
        void SetEmitMax(unsigned v) { mEmitMax = v; }
        float GetLifetimeMin() const { return mLifetimeMin; }
        void SetLifetimeMin(float v) { mLifetimeMin = v; }
        float GetLifetimeMax() const { return mLifetimeMax; }
        void SetLifetimeMax(float v) { mLifetimeMax = v; }
        float GetSpeedMin() const { return mSpeedMin; }
        void SetSpeedMin(float v) { mSpeedMin = v; }
        float GetSpeedMax() const { return mSpeedMax; }
        void SetSpeedMax(float v) { mSpeedMax = v; }
        float GetRotationSpeedMin() const { return mRotationSpeedMin; }
        void SetRotationSpeedMin(float v) { mRotationSpeedMin = v; }
        float GetRotationSpeedMax() const { return mRotationSpeedMax; }
        void SetRotationSpeedMax(float v) { mRotationSpeedMax = v; }
        float GetRotationMin() const { return mRotationMin; }
        void SetRotationMin(float v) { mRotationMin = v; }
        float GetRotationMax() const { return mRotationMax; }
        void SetRotationMax(float v) { mRotationMax = v; }
        bool GetOneShot() const { return mOneShot; }
        void SetOneShot(bool v) { mOneShot = v; }
        bool GetIsRunning() const { return mIsRunning; }

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