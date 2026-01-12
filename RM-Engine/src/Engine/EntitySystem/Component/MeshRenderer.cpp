/**
 * @file MeshRenderer.cpp
 * @author sumin.park
 * @brief  Renderer Component
 * @version 0.1
 * @date 1/8/2026 11:30:10 AM
 *
 * @copyright Copyright (c) 2025 - RM Engine
 *
 */

#include "pch.h"
#include "MeshRenderer.h"
#include "Engine/Resource/Mesh.h"
#include "Engine/Resource/Material.h"
#include "Engine/Math/Mat3.h"
#include "Transform.h"
#include "../GameObject/GameObject.h"

namespace rm
{
    MeshRenderer::MeshRenderer(GameObject* owner)
        :Component(owner){
    }

    MeshRenderer::~MeshRenderer() {
        //Todo: mesh and material should be controlled by resource mananger
        if (mesh != nullptr) {
            delete mesh;
            mesh = nullptr;
        }

        if (material != nullptr) {
            delete material;
            material = nullptr;
        }
    }

    void MeshRenderer::Init() {
    }

    void MeshRenderer::Update(float dt) {
    }

    void MeshRenderer::LateUpdate(float dt) {
    }

void MeshRenderer::Render() {
        if (!mesh || !material)
            return;

        glUseProgram(material->program);

        auto transform = GetGameObject()->GetComponent<Transform>();
        RM_ASSERT(transform != nullptr);

        math::Mat3 model = transform->LocalMatrix();
        math::Mat3 viewProj(1.0f);

        GLint uModel = glGetUniformLocation(material->program, "u_Model");
        GLint uViewProj = glGetUniformLocation(material->program, "u_ViewProj");

        glUniformMatrix3fv(uModel, 1, GL_FALSE, &model[0][0]);
        glUniformMatrix3fv(uViewProj, 1, GL_FALSE, &viewProj[0][0]);

        glBindVertexArray(mesh->vao);
        glDrawElements(GL_TRIANGLES, mesh->indexCount, GL_UNSIGNED_INT, nullptr);
        glBindVertexArray(0);
    }
} // rm namespace