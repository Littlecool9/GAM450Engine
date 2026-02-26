/**
 * @file SpriteRenderer.h
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
#include "Engine/Interfaces/ISubmitRenderCommand.h"

namespace mts
{
    struct RenderCommand;
    class SpriteRenderer : public Component, public ISubmitRenderCommand
    {
       public:
        SpriteRenderer(GameObject* owner);
        SpriteRenderer() = default;
        ~SpriteRenderer() override = default;

        void Init() override;
        void Update(float dt) override;
        void LateUpdate(float dt) override;
        void Render() override;
        void Deserialize(JsonValue const& jsonObj) override;
        void Serialize(JsonDocument& doc) const override;
        void SubmitRenderCommand() const override;

        void SetMesh(Mesh* newMesh) { mMesh = newMesh; }
        void SetShader(Shader* newShader) { mShader = newShader; }
        void SetTexture(Texture* newTexture) { mTexture = newTexture; }
        void SetFrame(int frameIndex);
        void SetAlpha(float alpha) { mAlpha = alpha; }
        void SetFlipX(bool flip) { mFlipX = flip; }
        void SetFlipY(bool flip) { mFlipY = flip; }
        float GetAlpha() const { return mAlpha; }
        bool HasTextureAlpha() const { return mHasTextureAlpha; }
        void SetTintColor(glm::vec3 tintColor) { mTintColor = tintColor; }
        void SetRows(int r);
        void SetCols(int c);
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
        bool mFlipX = false;
        bool mFlipY = false;
        glm::vec3 mTintColor = {1.0f, 1.0f, 1.0f};
        glm::vec2 mUvOffset = {0.0f, 0.0f};

        // Parallax factor for horizontal movement:
        // 0 locks the sprite to the camera (HUD-style, ignores camera movement),
        // 1 makes the sprite move normally in world space (no parallax effect).
        float parallaxFactorX = 1.0f; 
    };

}  // namespace mts