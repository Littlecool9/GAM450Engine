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
#include "Engine/EntitySystem/Component/Camera/CameraFollow.h"
#include "Engine/EntitySystem/Component/PhysicsComponents/BoxCollider2D.h"
#include "Engine/EntitySystem/Component/PhysicsComponents/RigidBody2D.h"
#include "Engine/Input/Input.h"

SandboxScene::~SandboxScene() = default;

void SandboxScene::OnInit()
{
    /*
    mts::GameObject* go = CreateGameObject();
    go->SetName("SandBoxTestGameObject");

    mts::SerializationUtility::DeserializeFromFile(go, "Data/TestEntity.json");

    go->GetComponent<mts::ActionList>()->AddTranslateAction(
        go->GetComponent<mts::Transform>()->GetPosition() * 5.0f,
        go,
        4,
        0,
        0,
        true);

    mts::GameObject* cam = CreateGameObject();
    cam->SetName("CameraObject");
    mts::SerializationUtility::DeserializeFromFile(cam, "Data/Camera.json");
    cam->GetComponent<mts::CameraFollow>()->SetObjectToFollow(go->GetComponent<mts::Transform>());
    */
    test1 = CreateGameObject();
    test1->SetName("Test1");
    auto tr = test1->AddComponent<mts::Transform>();
    tr->SetScale({250.0, 250.0, 1.0});
    auto rb = test1->AddComponent<mts::RigidBody2D>();
    rb->SetBody(mts::RigidBody2D::BodyType::Dynamic);
    auto col = test1->AddComponent<mts::BoxCollider2D>();

    test2 = CreateGameObject();
    test2->SetName("Test2");
    tr = test2->AddComponent<mts::Transform>();
    tr->SetScale({250.0, 250.0, 1.0});
    tr->SetPosition({500, 500, 0});
    rb = test2->AddComponent<mts::RigidBody2D>();
    col = test2->AddComponent<mts::BoxCollider2D>();
}

void SandboxScene::OnUpdate()
{
    if (mts::Input::IsKeyDown(mts::Key::Up))
        test1->GetComponent<mts::RigidBody2D>()->AddForce({0, -100});
    if (mts::Input::IsKeyDown(mts::Key::Down))
        test1->GetComponent<mts::RigidBody2D>()->AddForce({0, 100});
    if (mts::Input::IsKeyDown(mts::Key::Left))
        test1->GetComponent<mts::RigidBody2D>()->AddForce({-100, 0});
    if (mts::Input::IsKeyDown(mts::Key::Right))
        test1->GetComponent<mts::RigidBody2D>()->AddForce({100, 0});
}