#include "pch.h"
#include "EnemyPrototypeScene.h"



#include "EnemyAI/EnemyAttackAction.h"
#include "EnemyAI/EnemyAttackAction2.h"
#include "EnemyAI/EnemyDodgeAction.h"
#include "EnemyAI/EnemyMoveToPlayerAction.h"
#include "Engine/Audio/AudioManager.h"
#include "Engine/EntitySystem/GameObject/GameObjectFactory.h"
#include "Engine/EntitySystem/Component/Camera/CameraFollow.h"
#include "Engine/EntitySystem/Component/EnemyBehavior/EnemyActionList.h"
#include "Engine/EntitySystem/Component/EnemyBehavior/EnemyAgent.h"
#include "Engine/SceneSystem/SceneSystem.h"
#include "Engine/Scripting/ScriptFactory.h"
#include "Script/PlayerController.h"

REGISTER_ENEMY_ACTION("Enemy1", mts::EnemyAttackAction);
REGISTER_ENEMY_ACTION("Enemy1", mts::EnemyMoveToPlayerAction)
REGISTER_ENEMY_ACTION("Enemy1", mts::EnemyDodgeAction);
REGISTER_ENEMY_ACTION("Enemy1", mts::EnemyAttackAction2)


static const mts::MusicTrack kMausoleumMash = {"MausoleumMash",
                                               "event:/MausoleumMash",
                                               140.f,
                                               4,
                                               0.f};


EnemyPrototypeScene::~EnemyPrototypeScene()
{
}


void EnemyPrototypeScene::OnInit()
{
    mts::ScriptFactory::RegisterScript("PlayerController", []() { return new PlayerController(); });
    player = mts::GameObjectFactory::InstantiateFromJSON("Data/Player.prefab");
    player->AddTag("Player");

    mts::GameObjectFactory::InstantiateFromJSON("Data/Ground.prefab");

    camera = mts::GameObjectFactory::InstantiateFromJSON("Data/Camera.prefab");
    camera->SetName("CameraObject");

    background = mts::GameObjectFactory::InstantiateFromJSON("Data/Sky.prefab");
    background2 = mts::GameObjectFactory::InstantiateFromJSON("Data/BackgroundTest.prefab");

    //mBeatLine = mts::GameObjectFactory::InstantiateFromJSON("Data/BeatLine.prefab");

    enemy = mts::GameObjectFactory::InstantiateFromJSON("Data/Enemy.prefab");
    enemy->SetName("Enemy");

    //mts::SceneSystem::LoadScene(this, "../TechDemo/Data/Resources/CurrentScene.json");
    // if this is not created last it crashes
    auto tilemap = mts::GameObjectFactory::InstantiateFromJSON("Data/DemoTilemapEntity.prefab");
}

void EnemyPrototypeScene::OnEnterPlayMode()
{
    Scene::OnEnterPlayMode();
    //player->AddTag("Player");
    //camera->SetName("CameraObject");
    //mts::Transform* playerTr = player->GetComponent<mts::Transform>();
    //camera->GetComponent<mts::CameraFollow>()->SetObjectToFollow(playerTr);
    //enemy->GetComponent<mts::EnemyAgent>()->SetPlayerReference(player);
    //enemy->GetComponent<mts::EnemyAgent>()->Initialize(mts::AIPlanner::GetPlannerById("Enemy1"));
    player = FindGameObjectByName("Player");
    enemy = FindGameObjectByName("Enemy");
    //auto a = enemy->GetComponent<mts::EnemyAgent>();
    enemy->GetComponent<mts::EnemyAgent>()->SetPlayerReference(player);
    enemy->GetComponent<mts::EnemyAgent>()->Initialize(mts::AIPlanner::GetPlannerById("Enemy1"));
    camera = FindGameObjectByName("CameraObject");
    if (camera)
    {
        camera->GetComponent<mts::CameraFollow>()->SetObjectToFollow(
            player->GetComponent<mts::Transform>());
    }

    mts::AudioManager& audio = mts::AudioManager::Get();
    if (!audio.StartMusic(kMausoleumMash))
        LOG_ERROR("[TechDemoScene] StartMusic failed – check event path 'event:/MausoleumMash'.");
    else
        LOG_INFO("[TechDemoScene] MausoleumMash music started.");
}

void EnemyPrototypeScene::OnExitPlayMode()
{
    mts::AudioManager::Get().StopMusic();
}

void EnemyPrototypeScene::OnUpdate()
{

}