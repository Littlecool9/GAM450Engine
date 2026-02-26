#include "pch.h"
#include "PlatformerDemoScene.h"

#include "Engine/EntitySystem/Component/Camera/CameraFollow.h"
#include "Engine/EntitySystem/GameObject/GameObjectFactory.h"
#include "Engine/Scripting/ScriptFactory.h"
#include "Script/PlayerController.h"

#include <glm/vec2.hpp>

PlatformerDemoScene::~PlatformerDemoScene()
{
}

void PlatformerDemoScene::OnInit()
{
    mts::ScriptFactory::RegisterScript("PlayerController", []() { return new PlayerController(); });

    auto player = mts::GameObjectFactory::InstantiateFromJSON("Data/Player.json");
    player->AddTag("Player");

    camera = mts::GameObjectFactory::InstantiateFromJSON("Data/Camera.json");
    camera->SetName("CameraObject");
    camera->GetComponent<mts::CameraFollow>()->SetObjectToFollow(
        player->GetComponent<mts::Transform>());

    background = mts::GameObjectFactory::InstantiateFromJSON("Data/Sky.json");

    auto tilemap = mts::GameObjectFactory::InstantiateFromJSON("Data/DemoTilemapEntity.json");
}

void PlatformerDemoScene::OnUpdate()
{
    // sync the background to the camera
    mts::Transform* camTransform = camera->GetComponent<mts::Transform>();
    if (camTransform)
    {
        mts::Transform* backgroundTransform = background->GetComponent<mts::Transform>();
        if (backgroundTransform)
        {
            backgroundTransform->SetPosition(
                glm::vec2(camTransform->GetPosition().x, camTransform->GetPosition().y));
        }
    }
}