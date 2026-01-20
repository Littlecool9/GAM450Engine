/**
 * @file SandboxScene.cpp
 * @author sumin.park
 * @brief  Example Scene class
 * @version 0.1
 * @date 1/8/2026 10:15:55 AM
 *
 * @copyright Copyright (c) 2025 - Mitosis Engine
 *
 */

#include "pch.h"
#include "SandboxScene.h"

#include "Engine/Serialization/SerializationUtility.h"
#include "Engine/EntitySystem/GameObject/GameObject.h"
#include "Engine/EntitySystem/Component/ComponentFactory.h"

//void SetTriangleMesh(mts::Mesh* mesh)
//{
//    mts::Vertex vertices[] = {{{-0.5f, -0.5f}}, {{0.5f, -0.5f}}, {{0.0f, 0.5f}}};
//
//    unsigned int indices[] = {0, 1, 2};
//
//    mesh->indexCount = 3;
//
//    glGenVertexArrays(1, &mesh->vao);
//    glBindVertexArray(mesh->vao);
//
//    glGenBuffers(1, &mesh->vbo);
//    glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo);
//    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
//
//    glGenBuffers(1, &mesh->ibo);
//    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->ibo);
//    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
//
//    // layout(location = 0) vec2 position
//    glEnableVertexAttribArray(0);
//    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(mts::Vertex), (void*) 0);
//
//    glBindVertexArray(0);
//}

SandboxScene::~SandboxScene()
{
}

void SandboxScene::OnInit()
{
    mts::ComponentFactory* compFactory = new mts::ComponentFactory();
    mts::GameObject* obj = new mts::GameObject(compFactory);
    AddGameObject(obj);
    mts::SerializationUtility::DeserializeFromFile(obj, "Data/TestMeshEntity.json");

    /*LOG_INFO("CWD: {}", std::filesystem::current_path().string());

    auto vertSrc = LoadTextFile("../Mitosis-Engine/src/Shaders/triangle.vert");
    auto fragSrc = LoadTextFile("../Mitosis-Engine/src/Shaders/triangle.frag");

    auto shader = new mts::Shader();
    auto mesh = new mts::Mesh();

    shader->CreateProgram(vertSrc.c_str(), fragSrc.c_str());
    mesh->BuildQuad(1, 1);

    mr->SetShader(shader);
    mr->SetMesh(mesh);*/
}