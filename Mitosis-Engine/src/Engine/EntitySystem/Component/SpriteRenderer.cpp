/**
 * @file SpriteRenderer.cpp
 * @author sumin.park
 * @brief  Renderer Component
 * @version 0.1
 * @date 1/8/2026 11:30:10 AM
 *
 * @copyright Copyright (c) 2025 - Mitosis Engine
 *
 */

#include "pch.h"
#include "SpriteRenderer.h"
#include "Engine/Resource/Mesh.h"
#include "Transform.h"
#include "../GameObject/GameObject.h"
#include "Engine/Renderer.h"
#include "Engine/Resource/ResourceManager.h"
#include "UITransform.h"

#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace mts
{
    SpriteRenderer::SpriteRenderer(GameObject* owner) : Component(owner)
    {
    }

    void SpriteRenderer::Init()
    {
    }

    void SpriteRenderer::Update(float dt)
    {
    }

    void SpriteRenderer::LateUpdate(float dt)
    {
    }

    void SpriteRenderer::Deserialize(JsonValue const& jsonObj)
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

        // Read in starting frame index
        int frameIndex = 0;
        if (jsonObj.Has("FrameIndex"))
        {
            frameIndex = jsonObj["FrameIndex"].GetInt(frameIndex);
        }

        if (jsonObj.Has("ParallaxFactorX"))
        {
            mParallaxFactorX = jsonObj["ParallaxFactorX"].GetFloat(mParallaxFactorX);
        }

        if (jsonObj.Has("RenderLayer"))
        {
            mRenderLayer = jsonObj["RenderLayer"].GetInt();
        }

        // Read in texture resource
        auto textureResourcePath = "";
        if (jsonObj.Has("TextureResource"))
        {
            textureResourcePath = jsonObj["TextureResource"].GetString(textureResourcePath);
        }
        mTexture = ResourceManager::GetTexture(textureResourcePath);
        mHasTextureAlpha = mTexture->HasTextureAlpha();

        // Read in mesh resource
        auto meshResourcePath = "";
        if (jsonObj.Has("MeshResource"))
        {
            meshResourcePath = jsonObj["MeshResource"].GetString(meshResourcePath);
        }
        mMesh = ResourceManager::GetMesh(meshResourcePath);

        mNumCols = mMesh->GetNumCols();
        mNumRows = mMesh->GetNumRows();

        SetFrame(frameIndex);

        // Read in shader resource
        auto shaderResourcePath = "";
        if (jsonObj.Has("ShaderResource"))
        {
            shaderResourcePath = jsonObj["ShaderResource"].GetString(shaderResourcePath);
        }
        mShader = ResourceManager::GetShader(shaderResourcePath);

        // Read in text string (for sprite text rendering)
        if (jsonObj.Has("Text"))
        {
            mText = jsonObj["Text"].GetString("");
            mHasText = !mText.empty();
        }
    }

    void SpriteRenderer::SetCols(int c)
    {
        mNumCols = c;
    }

    void SpriteRenderer::SetRows(int r)
    {
        mNumRows = r;
    }

    void SpriteRenderer::Serialize(JsonDocument& doc) const
    {
        doc.WriteString("Type", "SpriteRenderer");
        doc.BeginObject("Data");

        doc.WriteVec3("TintColor", mTintColor);

        doc.WriteFloat("Alpha", mAlpha);

        doc.WriteInt("FrameIndex", mFrameIndex);

        doc.WriteFloat("ParallaxFactorX", mParallaxFactorX);

        doc.WriteInt("RenderLayer", mRenderLayer);

        if (mTexture)
            doc.WriteString("TextureResource", mTexture->GetPath().c_str());
        else
            doc.WriteString("TextureResource", "");

        if (mMesh)
            doc.WriteString("MeshResource", mMesh->GetPath().c_str());
        else
            doc.WriteString("MeshResource", "");

        if (mShader)
            doc.WriteString("ShaderResource", mShader->GetPath().c_str());
        else
            doc.WriteString("ShaderResource", "");

        if (mHasText)
            doc.WriteString("Text", mText.c_str());

        doc.EndObject();
    }

    // Sprites don't render themselves, done through Renderer calling DrawSprite
    void SpriteRenderer::Render()
    {
        SubmitRenderCommand();
    }

    void SpriteRenderer::SubmitRenderCommand() const
    {
        if (!mMesh || !mShader || !mTexture)
            return;

        // Resolve model matrix — UITransform takes priority over Transform
        glm::mat4 model;
        bool isUI = false;
        if (auto* uit = GetGameObject()->GetComponent<UITransform>())
        {
            model = uit->GetLocalMatrix();
            isUI = true;
        }
        else if (auto* tr = GetGameObject()->GetComponent<Transform>())
        {
            model = tr->GetLocalMatrix();
        }
        else
            return;

        if (mFlipX)
            model *= glm::scale(glm::mat4(1.0f), {-1, 1, 1});
        if (mFlipY)
            model *= glm::scale(glm::mat4(1.0f), {1, -1, 1});

        const auto& cam = Renderer::GetActiveCamera();

        glm::mat4 viewProj;
        float offsetFactorX = 1.0f - mParallaxFactorX;
        glm::vec3 parallaxOffset = cam.position * glm::vec3(offsetFactorX, 0.0f, 0.0f);
        glm::mat4 parallaxView = glm::translate(cam.view, parallaxOffset);

        if (cam.valid)
        {
            viewProj = cam.proj * parallaxView;
        }
        else
        {
            // Fallback (so nothing breaks if no camera exists yet)
            glm::ivec4 viewportSize;
            glGetIntegerv(GL_VIEWPORT, &viewportSize[0]);
            viewProj = glm::ortho(0.0f,
                                  static_cast<float>(viewportSize.z),
                                  static_cast<float>(viewportSize.w),
                                  0.0f,
                                  -1.0f,
                                  1.0f);
        }

        if (mHasText)
        {

            glm::mat4 currentModel = model;


            float charAdvance = 1.0f;
            if (auto* tr = GetGameObject()->GetComponent<Transform>())
                charAdvance = tr->GetScale().x;

            for (const char ch : mText)
            {
                int glyphIndex = static_cast<int>(ch) - static_cast<int>(' ');


                glm::vec2 glyphUv = {(1.0f / static_cast<float>(mNumCols)) *
                                         static_cast<float>(glyphIndex % mNumCols),
                                     (1.0f / static_cast<float>(mNumRows)) *
                                         static_cast<int>(glyphIndex / mNumCols)};

                mShader->SetMatrix4("model", currentModel);
                mShader->SetMatrix4("viewProj", viewProj);
                mShader->SetVector2f("uvOffset", glyphUv);
                mShader->SetVector3f("tintColor", mTintColor);
                mShader->SetFloat("alpha", mAlpha);

                RenderCommand cmd;
                cmd.shader = mShader;
                cmd.texture = mTexture;
                cmd.mesh = mMesh;
                cmd.model = currentModel;
                cmd.uvOffset = glyphUv;
                cmd.tintColor = mTintColor;
                cmd.alpha = mAlpha;
                cmd.sortKey = static_cast<float>(mRenderLayer);
                cmd.hasAlpha = mHasTextureAlpha;
                cmd.parallaxFactorX = mParallaxFactorX;
                cmd.isUI = isUI;

                Renderer::Submit(cmd);

                currentModel = glm::translate(currentModel, glm::vec3(charAdvance, 0.0f, 0.0f));
            }
        }
        else
        {
            // Normal single-sprite rendering
            mShader->SetMatrix4("model", model);
            mShader->SetMatrix4("viewProj", viewProj);
            mShader->SetVector2f("uvOffset", mUvOffset);
            mShader->SetVector3f("tintColor", mTintColor);
            mShader->SetFloat("alpha", mAlpha);

            RenderCommand cmd;
            cmd.shader = mShader;
            cmd.texture = mTexture;
            cmd.mesh = mMesh;
            cmd.model = model;
            cmd.uvOffset = mUvOffset;
            cmd.tintColor = mTintColor;
            cmd.alpha = mAlpha;
            cmd.sortKey = static_cast<float>(mRenderLayer);
            cmd.hasAlpha = mHasTextureAlpha;
            cmd.parallaxFactorX = mParallaxFactorX;
            cmd.isUI = isUI;

            Renderer::Submit(cmd);
        }
    }

    void SpriteRenderer::SetFrame(int frameIndex)
    {
        // Set member variable
        mFrameIndex = frameIndex;

        // Calculate texture offset
        mUvOffset = {
            (1.0f / static_cast<float>(mNumCols)) * static_cast<float>(mFrameIndex % mNumCols),
            (1.0f / static_cast<float>(mNumRows)) * static_cast<int>(mFrameIndex / mNumCols)};
    }

    Component* SpriteRenderer::Clone(GameObject* newOwner) const
    {
        SpriteRenderer* c = new SpriteRenderer(*this);
        c->InitFromClone(newOwner);
        return c;
    }
}  // namespace mts