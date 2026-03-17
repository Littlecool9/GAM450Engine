#include "pch.h"
#include "EnemyPrototypeScene.h"



#include "EnemyAI/EnemyAttackAction.h"
#include "EnemyAI/EnemyAttackAction2.h"
#include "EnemyAI/EnemyDodgeAction.h"
#include "EnemyAI/EnemyMoveToPlayerAction.h"
#include "EnemyAI/params/AttackCountdown2Param.h"
#include "EnemyAI/params/AttackCountdownParam.h"

#include "EnemyAI/params/DodgeCountdownParam.h"

#include "Engine/Audio/AudioManager.h"
#include "Engine/EntitySystem/GameObject/GameObjectFactory.h"
#include "Engine/EntitySystem/Component/Camera/CameraFollow.h"
#include "Engine/EntitySystem/Component/EnemyBehavior/EnemyActionList.h"
#include "Engine/EntitySystem/Component/EnemyBehavior/EnemyAgent.h"
#include "Engine/EntitySystem/Component/EnemyBehavior/EnemyState.h"
#include "Engine/Scripting/ScriptFactory.h"
#include "Script/BarScaler.h"
#include "Script/BeatBarController.h"
#include "Script/EnemyController.h"
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
    mts::ScriptFactory::RegisterScript("EnemyController", []() { return new mts::EnemyController(); });
    mts::ScriptFactory::RegisterScript("BarScaler", []() { return new mts::BarScaler(); });
    mts::ScriptFactory::RegisterScript("BeatBarController", []() { return new BeatBarController(); });

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

    enemyhp = mts::GameObjectFactory::InstantiateFromJSON("Data/HPBar.prefab");
    enemyhp->SetName("HP Bar Enemy");
    playerhp = mts::GameObjectFactory::InstantiateFromJSON("Data/HPBarP.prefab");
    playerhp->SetName("HP Bar Player");
    mBeatLine = mts::GameObjectFactory::InstantiateFromJSON("Data/BeatLine.prefab");
    //mts::SceneSystem::LoadScene(this, "../TechDemo/Data/Resources/CurrentScene.json");
    // if this is not created last it crashes
    auto tilemap = mts::GameObjectFactory::InstantiateFromJSON("Data/DemoTilemapEntity.prefab");
}

void EnemyPrototypeScene::OnEnterPlayMode()
{
    Scene::OnEnterPlayMode();

    player = FindGameObjectByName("Player");
    enemy = FindGameObjectByName("Enemy");

    enemy->GetComponent<mts::EnemyAgent>()->SetPlayerReference(player);
    enemy->GetComponent<mts::EnemyAgent>()->Initialize(mts::AIPlanner::GetPlannerById("Enemy1"));
    camera = FindGameObjectByName("CameraObject");
    enemyhp = FindGameObjectByName("HP Bar Enemy");
    playerhp = FindGameObjectByName("HP Bar Player");
    mBeatLine = FindGameObjectByName("BeatLine");
    if (enemyhp)
    {
        enemyhp->GetComponent<mts::CameraFollow>()->SetObjectToFollow(camera->GetComponent<mts::Transform>());
        enemy->GetScript<mts::EnemyController>()->SetBar(enemyhp);
    }
    if (playerhp)
    {
        player->GetScript<PlayerController>()->SetHpBar(playerhp);
    }
    if (camera)
    {
        camera->GetComponent<mts::CameraFollow>()->SetObjectToFollow(
            player->GetComponent<mts::Transform>());
    }
    auto& p = enemy->GetComponent<mts::EnemyAgent>()->GetState().params;
    p.emplace(mts::HashAction("AttackCooldown"), new mts::AttackCountdownParam());
    p.emplace(mts::HashAction("Attack2Cooldown"), new mts::AttackCountdown2Param());
    p.emplace(mts::HashAction("DodgeCountdown"), new mts::DodgeCountdownParam());




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
    if (player->IsDead())
    {
        if (enemy)
        {
            enemy->GetComponent<mts::EnemyAgent>()->SetPlayerReference(nullptr);
            camera->GetComponent<mts::CameraFollow>()->SetObjectToFollow(
                enemy->GetComponent<mts::Transform>());
        }
    }
}