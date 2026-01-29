/**
 * @file MeshRenderer.h
 * @author sumin.park
 * @brief  Renderer Component
 * @version 0.1
 * @date 1/8/2026 11:30:10 AM
 *
 * @copyright Copyright (c) 2025 - Mitosis Engine
 *
 */

#pragma once
#include "Component.h"
#include "Engine/Resource/Mesh.h"
#include "Engine/Resource/Shader.h"
#include "Engine/Resource/Texture.h"

namespace mts
{
    class MeshRenderer : public Component
    {
       public:
        MeshRenderer(GameObject* owner);
        MeshRenderer() = default;
        ~MeshRenderer() override;

        void Init() override;
        void Update(float dt) override;
        void LateUpdate(float dt) override;
        void Render() override;
        void Deserialize(JsonValue const& jsonObj) override;
        void Serialize(JsonDocument& doc) const override;
        void SetMesh(Mesh* newMesh) { mMesh = newMesh; }
        void SetShader(Shader* newShader) { mShader = newShader; }
        void SetTexture(Texture* newTexture) { mTexture = newTexture; }
        void SetFrame(int frameIndex);

       private:
        Mesh* mMesh = nullptr;
        Shader* mShader = nullptr;
        Texture* mTexture = nullptr;
        int mNumRows = 1;
        int mNumCols = 1;
        int mFrameIndex = 0;
        // TODO: implement alpha blending (need to figure out how we want to sort sprites so they can be drawn back to front for alpha to work)
        float mAlpha = 1.0f;
        glm::vec3 mTintColor = {1.0f, 1.0f, 1.0f};
        glm::vec2 mUvOffset = {0.0f, 0.0f};
    };

}  // namespace mts