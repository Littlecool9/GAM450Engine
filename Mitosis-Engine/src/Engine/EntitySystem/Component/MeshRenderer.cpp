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

namespace mts
{
    MeshRenderer::MeshRenderer(GameObject* owner) : Component(owner)
    {
    }


    MeshRenderer::~MeshRenderer()
    {
        // Todo: mesh, shader, & texture should be controlled by resource mananger
        if (mesh != nullptr)
        {
            delete mesh;
            mesh = nullptr;
        }
        if (shader != nullptr)
        {
            delete shader;
            shader = nullptr;
        }
        if (texture != nullptr)
        {
            delete texture;
            texture = nullptr;
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
        // Read in uv size
        int numRows = 1;
        int numCols = 1;

        if (jsonObj.Has("NumRows"))
        {
            numRows = jsonObj["NumRows"].GetInt(numRows);
        }
        if (jsonObj.Has("NumCols"))
        {
            numCols = jsonObj["NumCols"].GetInt(numCols);
        }

        // Read in mesh color
        glm::vec3 color(1.0f, 1.0f, 1.0f);

        if (jsonObj.Has("MeshColor"))
        {
            JsonValue mc = jsonObj["MeshColor"];
            if (mc.IsArray() && mc.Size() >= 3)
            {
                color.x = mc[0].GetFloat(color.x);
                color.y = mc[1].GetFloat(color.y);
                color.z = mc[2].GetFloat(color.z);
            }
        }

        // TODO: move mesh creation to resource manager
        // Build mesh
        mesh = new Mesh();
        mesh->BuildQuad(numCols, numRows, color);

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
        shader = new Shader();
        auto vertSrc = Shader::LoadTextFile(vertSrcLoc.c_str());
        auto fragSrc = Shader::LoadTextFile(fragSrcLoc.c_str());
        shader->CreateProgram(vertSrc.c_str(), fragSrc.c_str());
    }

    // TODO: serialize meshRenderer data
    void MeshRenderer::Serialize(JsonDocument& doc) const
    {
    }

    void MeshRenderer::Render() {
        if (!mesh || !shader)
            return;

        shader->UseProgram();

        auto transform = GetGameObject()->GetComponent<Transform>();
        MTS_ASSERT(transform != nullptr);

        glm::mat4 model = transform->LocalMatrix();
        glm::ivec4 viewportSize;
        glGetIntegerv(GL_VIEWPORT, &viewportSize[0]);
        glm::mat4 viewProj = glm::ortho(0.0f, static_cast<float>(viewportSize.z), 0.0f, static_cast<float>(viewportSize.w), -1.0f, 1.0f);

        shader->SetMatrix4("u_Model", model);
        shader->SetMatrix4("u_ViewProj", viewProj);

        mesh->Draw();
    }
}  // namespace mts