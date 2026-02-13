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
#include "Engine/EntitySystem/Component/Animation.h"
#include "Engine/Serialization/SerializationUtility.h"
#include "Engine/EntitySystem/GameObject/GameObject.h"
#include "Engine/EntitySystem/Component/Camera/CameraFollow.h"
#include "Engine/EntitySystem/Component/Camera/Camera.h"
#include "Engine/EntitySystem/Component/Transform.h"
#include "Engine/Tilemap/Tilemap.h"
#include "Engine/Renderer.h"
#include "Engine/EntitySystem/Component/AnimationSystem.h"
#include "Engine/EntitySystem/Component/PhysicsComponents/BoxCollider2D.h"
#include "Engine/EntitySystem/Component/PhysicsComponents/RigidBody2D.h"
#include "Engine/Input/Input.h"
#include "Scripts/RegisterScripts.h"

SandboxScene::~SandboxScene() = default;

void SandboxScene::OnInit()
{
    mts::RegisterScripts();

    test1 = CreateGameObject();
    test1->SetName("SandBoxTestGameObject");

    mts::SerializationUtility::DeserializeFromFile(test1, "Data/TestEntity.json");

    test2 = CreateGameObject();
    test2->SetName("CameraObject");
    mts::SerializationUtility::DeserializeFromFile(test2, "Data/Camera.json");
    test2->GetComponent<mts::CameraFollow>()->SetObjectToFollow(
        test1->GetComponent<mts::Transform>());

    mts::GameObject* tilemap = CreateGameObject();
    tilemap->SetName("TilemapObject");
    mts::SerializationUtility::DeserializeFromFile(tilemap, "Data/TestTilemapEntity.json");
    mts::SerializationUtility::SerializeToFile(test1, "Data/TestWrite.json");

    // test3 = CreateGameObject();
    // test3->SetName("soundDummy");

    // mts::SerializationUtility::DeserializeFromFile(test3, "Data/soundDummy.json");

    // glm::vec3 test = test3->GetComponent<mts::Transform>()->GetPosition();
    // test *= 10.f;

    // test1->GetComponent<mts::ActionList>()->AddTranslateAction(test, test3, 7, 0, 0, true);
    // test *= 2.f;
    // test1->GetComponent<mts::ActionList>()->AddTranslateAction(test, test1, 14, 0, 0, true);

    // test1->GetComponent<mts::Animation>()->AnimationPlay(8, 0.5, true);
}

void SandboxScene::OnUpdate()
{
    /*
    if (mts::Input::IsKeyDown(mts::Key::Up))
        test1->GetComponent<mts::RigidBody2D>()->AddForce({0, -100});
    if (mts::Input::IsKeyDown(mts::Key::Down))
        test1->GetComponent<mts::RigidBody2D>()->AddForce({0, 100});
    if (mts::Input::IsKeyDown(mts::Key::Left))
        test1->GetComponent<mts::RigidBody2D>()->AddForce({-100, 0});
    if (mts::Input::IsKeyDown(mts::Key::Right))
        test1->GetComponent<mts::RigidBody2D>()->AddForce({100, 0});
    if (mts::Input::IsKeyDown(mts::Key::Space))
    {
        test1->GetComponent<mts::RigidBody2D>()->SetVelocity(glm::vec2(0, 0));
    }
    if (glm::length(test1->GetComponent<mts::RigidBody2D>()->GetVelocity()) > 0)
    {
        test1->GetComponent<mts::AnimationSystem>()->ChangeState(
            mts::AnimationSystem::States::Walking);
    }
    else
    {
        test1->GetComponent<mts::AnimationSystem>()->ChangeState(
            mts::AnimationSystem::States::Idle);
    }
    */
    // test1->GetComponent<mts::ActionList>()->AddRotateAction(test1->GetComponent<mts::Transform>()->GetRotation()
    // * 2, test3, 7, 0, 0, true); test1->GetComponent<mts::ActionList>()->AddRotateAction(
    //     test1->GetComponent<mts::Transform>()->GetRotation() * 2,
    //     test3,
    //     7,
    //     0,
    //     0,
    //     true);
}