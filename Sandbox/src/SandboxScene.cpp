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

#include "Engine/EntitySystem/Component/ActionList.h"
#include "Engine/Serialization/SerializationUtility.h"
#include "Engine/EntitySystem/GameObject/GameObject.h"
#include "Engine/EntitySystem/GameObject/GameObjectFactory.h"
#include "Engine/EntitySystem/Component/ComponentFactory.h"

SandboxScene::~SandboxScene()
{
}

void SandboxScene::OnInit()
{
    mts::GameObject* go = CreateGameObject();
    go->SetName("SandBoxTestGameObject");

    mts::SerializationUtility::DeserializeFromFile(go, "Data/TestEntity.json");

    go->GetComponent<mts::ActionList>()->AddTranslateAction(glm::vec3(2, 2, 2), go, 4);
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