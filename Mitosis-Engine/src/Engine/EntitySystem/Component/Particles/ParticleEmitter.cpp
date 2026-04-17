/**
 * @file ParticleEmitter.cpp
 * @author josh.siegel
 * @brief // This component class is responsible for emitting particles.
 *
 * @copyright Copyright (c) 2026 DigiPen (USA) Corporation
 *
 */

#include "pch.h"
#include "ParticleEmitter.h"
#include "ParticleContainer.h"
#include "Engine/EntitySystem/GameObject/GameObject.h"
#include "Engine/EntitySystem/Component/Transform.h"
#include "Engine/Helpers/Random.h"
#include "Engine/Resource/Mesh.h"
#include "Engine/Renderer.h"
#include "Engine/Resource/ResourceManager.h"
#include "Engine/Resource/Mesh.h"
#include "Engine/Resource/Shader.h"
#include "Engine/Resource/Texture.h"

namespace mts
{
    ParticleEmitter::ParticleEmitter(GameObject* owner) : Component(owner)
    {
    }

    ParticleEmitter::~ParticleEmitter()
    {
        if (mContainer != nullptr)
            delete mContainer;
    }

    void ParticleEmitter::Init()
    {
        // no-op
    }

    void ParticleEmitter::Update(float dt)
    {
        // lambda function to update a particle
        auto lambda = [this](Particle& particle, float dt)
        {
            if (particle.lifetime > 0)
            {
                particle.lifetime -= dt;

                if (particle.lifetime > 0)
                {
                    // Update velocity and rotation
                    particle.velocity += particle.acceleration * dt;
                    particle.position += particle.velocity * dt;
                    particle.rotation += particle.rotationSpeed * dt;
                }
                else
                {
                    mContainer->KillParticle(particle);
                }
            }
        };

        // pass lambda function to container update
        mContainer->Update(lambda, dt);

        mEmitAccumulator += mEmitRate * dt;
        while (mEmitAccumulator > 0.0f)
        {
            if (!mContainer->IsFull())
            {
                EmitParticle();
            }

            mEmitAccumulator -= 1.0f;
        }

        if (mContainer->IsEmpty() && mContainer->IsFull())
        {
            GetGameObject()->Destroy();
        }
    }

    void ParticleEmitter::LateUpdate(float dt)
    {
        // no-op
    }

    void ParticleEmitter::Render()
    {
        SubmitRenderCommand();
    }

    void ParticleEmitter::SubmitRenderCommand() const
    {
        if (!mMesh || !mShader || !mTexture)
            return;

        auto transform = GetGameObject()->GetComponent<Transform>();
        MTS_ASSERT(transform != nullptr);

        float emitterZ = transform->GetPosition().z;

        auto submitLambda = [this, emitterZ](const Particle& particle)
        {
            if (particle.lifetime <= 0)
                return;

            glm::mat4 S =
                glm::scale(glm::mat4(1.0f), glm::vec3(particle.scale.x, particle.scale.y, 1.0f));
            glm::mat4 R =
                glm::rotate(glm::mat4(1.0f), particle.rotation, glm::vec3(0.0f, 0.0f, 1.0f));
            glm::mat4 T =
                glm::translate(glm::mat4(1.0f),
                               glm::vec3(particle.position.x, particle.position.y, emitterZ));

            RenderCommand cmd;
            cmd.shader = mShader;
            cmd.texture = mTexture;
            cmd.mesh = mMesh;
            cmd.model = T * R * S;
            cmd.uvOffset = particle.uvOffset;
            cmd.tintColor = mTintColor;
            cmd.alpha = mAlpha * (particle.lifetime / mLifetimeMin);
            cmd.sortKey = emitterZ;
            cmd.hasAlpha = true;
            cmd.parallaxFactorX = 1.0f;

            Renderer::Submit(cmd);
        };

        mContainer->Render(submitLambda);
    }

    void ParticleEmitter::Deserialize(JsonValue const& jsonObj)
    {
        // Read in tint color
        glm::vec3 color(1.0f, 1.0f, 1.0f);

        if (jsonObj.Has("TintColor"))
        {
            JsonValue mc = jsonObj["TintColor"];
            if (mc.IsArray() && mc.Size() >= 3)
            {
                mTintColor.x = mc[0].GetFloat(color.x);
                mTintColor.y = mc[1].GetFloat(color.y);
                mTintColor.z = mc[2].GetFloat(color.z);
            }
        }

        // Read in alpha value
        if (jsonObj.Has("Alpha"))
        {
            mAlpha = jsonObj["Alpha"].GetFloat(mAlpha);
        }

        // Read in texture resource
        auto textureResourcePath = "";
        if (jsonObj.Has("TextureResource"))
        {
            textureResourcePath = jsonObj["TextureResource"].GetString(textureResourcePath);
        }
        mTexture = ResourceManager::GetTexture(textureResourcePath);

        // Read in mesh resource
        auto meshResourcePath = "";
        if (jsonObj.Has("MeshResource"))
        {
            meshResourcePath = jsonObj["MeshResource"].GetString(meshResourcePath);
        }
        mMesh = ResourceManager::GetMesh(meshResourcePath);
        if (mMesh)
        {
            mNumCols = mMesh->GetNumCols();
            mNumRows = mMesh->GetNumRows();
        }

        // Read in starting frame index
        if (jsonObj.Has("FrameIndex"))
        {
            mFrameIndex = jsonObj["FrameIndex"].GetInt(mFrameIndex);
        }

        // Read in shader resource
        auto shaderResourcePath = "";
        if (jsonObj.Has("ShaderResource"))
        {
            shaderResourcePath = jsonObj["ShaderResource"].GetString(shaderResourcePath);
        }
        mShader = ResourceManager::GetShader(shaderResourcePath);

        if (jsonObj.Has("Scale"))
            mEmitterScale = jsonObj["Scale"].GetVec2(mEmitterScale);
        if (jsonObj.Has("ParticleScaleMin"))
            mScaleMin = jsonObj["ParticleScaleMin"].GetVec2(mScaleMin);
        if (jsonObj.Has("ParticleScaleMax"))
            mScaleMax = jsonObj["ParticleScaleMax"].GetVec2(mScaleMax);
        if (jsonObj.Has("Acceleration"))
            mAcceleration = jsonObj["Acceleration"].GetVec2(mAcceleration);
        if (jsonObj.Has("DirectionRange"))
            mDirectionRange = jsonObj["DirectionRange"].GetVec2(mDirectionRange);

        if (jsonObj.Has("EmitRate"))
            mEmitRate = jsonObj["EmitRate"].GetFloat(mEmitRate);
        if (jsonObj.Has("EmitMax"))
            mEmitMax = jsonObj["EmitMax"].GetInt(mEmitMax);
        if (jsonObj.Has("LifetimeMin"))
            mLifetimeMin = jsonObj["LifetimeMin"].GetFloat(mLifetimeMin);
        if (jsonObj.Has("LifetimeMax"))
            mLifetimeMax = jsonObj["LifetimeMax"].GetFloat(mLifetimeMax);
        if (jsonObj.Has("SpeedMin"))
            mSpeedMin = jsonObj["SpeedMin"].GetFloat(mSpeedMin);
        if (jsonObj.Has("SpeedMax"))
            mSpeedMax = jsonObj["SpeedMax"].GetFloat(mSpeedMax);
        if (jsonObj.Has("RotationSpeedMin"))
            mRotationSpeedMin = jsonObj["RotationSpeedMin"].GetFloat(mRotationSpeedMin);
        if (jsonObj.Has("RotationSpeedMax"))
            mRotationSpeedMax = jsonObj["RotationSpeedMax"].GetFloat(mRotationSpeedMax);
        if (jsonObj.Has("RotationMin"))
            mRotationMin = jsonObj["RotationMin"].GetFloat(mRotationMin);
        if (jsonObj.Has("RotationMax"))
            mRotationMax = jsonObj["RotationMax"].GetFloat(mRotationMax);
        if (jsonObj.Has("OneShot"))
            mOneShot = jsonObj["OneShot"].GetBool(mOneShot);
        if (jsonObj.Has("IsRunning"))
            mIsRunning = jsonObj["IsRunning"].GetBool(mIsRunning);

        if (mContainer != nullptr)
            delete mContainer;

        mContainer = new ParticleContainer(mEmitMax, mOneShot);
    }

    void ParticleEmitter::Serialize(JsonDocument& doc) const
    {
        doc.WriteString("Type", "ParticleEmitter");
        doc.BeginObject("Data");

        doc.WriteVec3("TintColor", mTintColor);
        doc.WriteFloat("Alpha", mAlpha);

        doc.WriteString("TextureResource", mTexture->GetPath().c_str());
        doc.WriteString("MeshResource", mMesh->GetPath().c_str());
        doc.WriteInt("FrameIndex", mFrameIndex);
        doc.WriteString("ShaderResource", mShader->GetPath().c_str());

        doc.WriteVec2("Scale", mEmitterScale);
        doc.WriteVec2("ParticleScaleMin", mScaleMin);
        doc.WriteVec2("ParticleScaleMax", mScaleMax);
        doc.WriteVec2("Acceleration", mAcceleration);
        doc.WriteVec2("DirectionRange", mDirectionRange);

        doc.WriteFloat("EmitRate", mEmitRate);
        doc.WriteInt("EmitMax", mEmitMax);

        doc.WriteFloat("LifetimeMin", mLifetimeMin);
        doc.WriteFloat("LifetimeMax", mLifetimeMax);

        doc.WriteFloat("SpeedMin", mSpeedMin);
        doc.WriteFloat("SpeedMax", mSpeedMax);

        doc.WriteFloat("RotationSpeedMin", mRotationSpeedMin);
        doc.WriteFloat("RotationSpeedMax", mRotationSpeedMax);

        doc.WriteFloat("RotationMin", mRotationMin);
        doc.WriteFloat("RotationMax", mRotationMax);

        doc.WriteBool("OneShot", mOneShot);
        doc.WriteBool("IsRunning", mIsRunning);

        doc.EndObject();
    }

    void ParticleEmitter::SetFrame(int frameIndex, Particle* particle)
    {
        // Calculate texture offset
        particle->uvOffset = {
            (1.0f / static_cast<float>(mNumCols)) * static_cast<float>(frameIndex % mNumCols),
            (1.0f / static_cast<float>(mNumRows)) * static_cast<int>(frameIndex / mNumCols)};
    }

    void ParticleEmitter::EmitParticle()
    {
        if (!mIsRunning)
            return;
        Particle* particle = mContainer->AllocateParticle();
        if (particle)
        {
            if (mFrameIndex >= 0 && mFrameIndex < mNumRows * mNumCols)
            {
                SetFrame(mFrameIndex, particle);
            }
            else
            {
                SetFrame(Random::Range(0, mNumRows * mNumCols - 1), particle);    
            }

            // Set random lifetime between min and max
            particle->lifetime = Random::Range(mLifetimeMin, mLifetimeMax);

            // Set position of particle randomly within the bounds of the emitter (Point, Line,
            // Square all work here)
            Transform* transform = GetGameObject()->GetComponent<Transform>();
            glm::vec3 translation = transform->GetPosition();
            float xMin = translation.x - (mEmitterScale.x / 2);
            float xMax = translation.x + (mEmitterScale.x / 2);
            float yMin = translation.y - (mEmitterScale.y / 2);
            float yMax = translation.y + (mEmitterScale.y / 2);
            particle->position = glm::vec2(Random::Range(xMin, xMax), Random::Range(yMin, yMax));

            // Set random particle scale between min and max
            float scaleRatio = mScaleMin.y / mScaleMin.x;
            float scaleX = Random::Range(mScaleMin.x, mScaleMax.x);
            float scaleY = scaleX * scaleRatio;
            particle->scale = glm::vec2(scaleX, scaleY);

            // Set random starting rotation of particle
            particle->rotation = Random::Range(mRotationMin, mRotationMax);

            // Set random rotation speed between min and max, also randomize rotation direction (+
            // or -)
            float rotationSpeed = Random::Range(mRotationSpeedMin, mRotationSpeedMax);
            if (Random::Range(0, 1) == 1)
                particle->rotationSpeed = rotationSpeed;
            else
                particle->rotationSpeed = -rotationSpeed;

            // Set velocity direction, if direction < 0, set to random direction
            glm::vec2 dir;
            float dirRad = Random::Range(mDirectionRange.x, mDirectionRange.y);
            dir = glm::vec2(glm::cos(dirRad), glm::sin(dirRad));

            // Set random speed between min and max
            float speed = Random::Range(mSpeedMin, mSpeedMax);
            dir *= speed;
            particle->velocity = dir;

            // Set particle initial acceleration
            particle->acceleration = mAcceleration;
        }
    }

    void ParticleEmitter::ForEachParticle(
        std::function<void(Particle& particle, float dt)> const& lambda,
        float dt)
    {
        mContainer->Update(lambda, dt);
    }

    Component* ParticleEmitter::Clone(GameObject* newOwner) const
    {
        ParticleEmitter* c = new ParticleEmitter(*this);
        c->InitFromClone(newOwner);
        // Re-allocate owned container; prototype's live particles are not copied
        c->mContainer = (mEmitMax > 0) ? new ParticleContainer(mEmitMax, mOneShot) : nullptr;
        return c;
    }

}  // namespace mts