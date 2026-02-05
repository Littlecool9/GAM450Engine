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
        ~MeshRenderer() override = default;

        void Init() override;
        void Update(float dt) override;
        void LateUpdate(float dt) override;
        void Render() override;
        void Deserialize(JsonValue const& jsonObj) override;
        void Serialize(JsonDocument& doc) const override;
        void DrawSprite();

        void SetMesh(Mesh* newMesh) { mMesh = newMesh; }
        void SetShader(Shader* newShader) { mShader = newShader; }
        void SetTexture(Texture* newTexture) { mTexture = newTexture; }
        void SetFrame(int frameIndex);
        void SetAlpha(float alpha) { mAlpha = alpha; }
        float GetAlpha() const { return mAlpha; }
        bool HasTextureAlpha() const { return mHasTextureAlpha; }
        void SetTintColor(glm::vec3 tintColor) { mTintColor = tintColor; }
        glm::vec3 GetTintColor() const { return mTintColor; }

       private:
        Mesh* mMesh = nullptr;
        Shader* mShader = nullptr;
        Texture* mTexture = nullptr;
        int mNumRows = 1;
        int mNumCols = 1;
        int mFrameIndex = 0;
        float mAlpha = 1.0f;
        bool mHasTextureAlpha = true;
        glm::vec3 mTintColor = {1.0f, 1.0f, 1.0f};
        glm::vec2 mUvOffset = {0.0f, 0.0f};
    };

}  // namespace mts