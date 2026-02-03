/**
 * @file MeshRenderer.cpp
 * @author sumin.park
 * @brief  Renderer Component
 * @version 0.1
 * @date 1/8/2026 11:30:10 AM
 *
 * @copyright Copyright (c) 2025 - Mitosis Engine
 *
 */

#include "pch.h"
#include "MeshRenderer.h"
#include "Engine/Resource/Mesh.h"
#include "Transform.h"
#include "../GameObject/GameObject.h"
#include "Engine/Windows/WinGLFWwindow.h"

#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Engine/Renderer.h"

namespace mts
{
    MeshRenderer::MeshRenderer(GameObject* owner) : Component(owner)
    {
    }


    MeshRenderer::~MeshRenderer()
    {
        // Todo: mesh, shader, & texture should be controlled by resource mananger
        if (mMesh != nullptr)
        {
            delete mMesh;
            mMesh = nullptr;
        }
        if (mShader != nullptr)
        {
            delete mShader;
            mShader = nullptr;
        }
        if (mTexture != nullptr)
        {
            delete mTexture;
            mTexture = nullptr;
        }
    }

    void MeshRenderer::Init()
    {
    }

    void MeshRenderer::Update(float dt)
    {
    }

    void MeshRenderer::LateUpdate(float dt)
    {
    }

    void MeshRenderer::Deserialize(JsonValue const& jsonObj)
    {
        // Read in texture image file
        auto textureFile = "";
        if (jsonObj.Has("Texture"))
        {
            textureFile = jsonObj["Texture"].GetString("");
        }

        // TODO: move texture creation to resource manager
        // Build texture
        if (textureFile != "")
        {
            mTexture = new Texture();
            mTexture->GenerateTexture(textureFile, true);
        }

        // Read in uv size

        if (jsonObj.Has("NumRows"))
        {
            mNumRows = jsonObj["NumRows"].GetInt(mNumRows);
        }
        if (jsonObj.Has("NumCols"))
        {
            mNumCols = jsonObj["NumCols"].GetInt(mNumCols);
        }

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
        SetFrame(frameIndex);

        // TODO: move mesh creation to resource manager
        // Build mesh
        mMesh = new Mesh();
        mMesh->BuildQuad(mNumCols, mNumRows);

        // Read in shaders
        auto vertFile = "";
        auto fragFile = "";
        
        if (jsonObj.Has("VertexShader"))
        {
            vertFile = jsonObj["VertexShader"].GetString("");
        }
        if (jsonObj.Has("FragmentShader"))
        {
            fragFile = jsonObj["FragmentShader"].GetString("");
        }
        
        std::string vertSrcLoc("../Mitosis-Engine/src/Shaders/");
        vertSrcLoc.append(vertFile);
        std::string fragSrcLoc("../Mitosis-Engine/src/Shaders/");
        fragSrcLoc.append(fragFile);

        // TODO: move shader creation to resource manager
        // Build shader
        mShader = new Shader();
        auto vertSrc = Shader::LoadTextFile(vertSrcLoc.c_str());
        auto fragSrc = Shader::LoadTextFile(fragSrcLoc.c_str());
        mShader->CreateProgram(vertSrc.c_str(), fragSrc.c_str());
    }

    // TODO: serialize meshRenderer data
    void MeshRenderer::Serialize(JsonDocument& doc) const
    {
    }

    void MeshRenderer::Render() {
        if (!mMesh || !mShader || !mTexture)
            return;

        mShader->UseProgram();

        auto transform = GetGameObject()->GetComponent<Transform>();
        MTS_ASSERT(transform != nullptr);

        glm::mat4 model = transform->GetLocalMatrix();


        //glm::ivec4 viewportSize;
        //glGetIntegerv(GL_VIEWPORT, &viewportSize[0]);
        //glm::mat4 viewProj = glm::ortho(0.0f, static_cast<float>(viewportSize.z), static_cast<float>(viewportSize.w), 0.0f, -1.0f, 1.0f);
        const auto& cam = mts::Renderer::GetActiveCamera();
        glm::mat4 viewProj;
        if (cam.valid)
        {
            viewProj = cam.viewProj;
        }
        else
        {
            // Fallback (so nothing breaks if no camera exists yet)
             glm::ivec4 viewportSize;
             glGetIntegerv(GL_VIEWPORT, &viewportSize[0]);
             glm::mat4 viewProj = glm::ortho(0.0f, static_cast<float>(viewportSize.z), static_cast<float>(viewportSize.w), 0.0f, -1.0f, 1.0f);
        }
        // set shader uniforms
        mShader->SetMatrix4("model", model);
        mShader->SetMatrix4("viewProj", viewProj);
        mShader->SetVector2f("uvOffset", mUvOffset);
        mShader->SetVector3f("tintColor", mTintColor);
        mShader->SetFloat("alpha", mAlpha);

        mTexture->BindTexture();
        mMesh->Draw();
        mTexture->UnbindTexture();
    }
    
    void MeshRenderer::SetFrame(int frameIndex)
    {
        // Set member variable
        mFrameIndex = frameIndex;

        // Calculate texture offset
        mUvOffset = {(1.0f / static_cast<float>(mNumCols)) * static_cast<float>(mFrameIndex % mNumCols),
                        (1.0f / static_cast<float>(mNumRows)) * static_cast<int>(mFrameIndex / mNumCols)};
    }
}  // namespace mts