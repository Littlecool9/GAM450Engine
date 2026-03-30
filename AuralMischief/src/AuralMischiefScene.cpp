#include "pch.h"
#include "AuralMischiefScene.h"

#include "Engine/EntitySystem/Component/Animation.h"
#include "Engine/EntitySystem/Component/AnimationSystem.h"
#include "Engine/EntitySystem/Component/EnemyBehavior/EnemyAgent.h"
#include "Engine/SceneSystem/SceneSystem.h"
#include "Engine/Scripting/ScriptFactory.h"
#include "Engine/Audio/AudioManager.h"
#include "Script/CameraController.h"
#include "Script/RhythmSystem.h"
#include "Script/Player/PlayerController.h"

static const mts::MusicTrack kMausoleumMash = {"Playable - BGM",
                                               "event:/Playable - BGM",
                                               120.f,
                                               4,
                                               0.f};

AuralMischiefScene::~AuralMischiefScene()
{
    if (mRhythmSystem)
    {
        mRhythmSystem->Shutdown();
        mRhythmSystem = nullptr;
    }
}

void AuralMischiefScene::OnInit()
{
    mts::SceneSystem::LoadScene(this, "Data/Resources/CurrentScene.json");
}

void AuralMischiefScene::OnUpdate()
{
    if (mRhythmSystem)
    {
        mRhythmSystem->Update();
    }
}

void AuralMischiefScene::OnEnterPlayMode()
{
    mts::AudioManager& audio = mts::AudioManager::Get();
    mts::ConductorSystem& conductor = mts::ConductorSystem::Get();
    //conductor.SetOffsetSec(.13f);  // TODO: expose this in editor for fine-tuning music sync
    if (!audio.StartMusic(kMausoleumMash))
        LOG_ERROR("[AuralMischiefScene] StartMusic failed �C check event path 'event:/Playable'.");
    else
        LOG_INFO("[AuralMischiefScene] Playable music started.");

    camera = FindGameObjectByName("CameraObject");
    player = FindGameObjectByName("Player");

    mRhythmSystem = &RhythmSystem::Get();
    mRhythmSystem->Init(this);
    
    if (camera && player)
    {
        auto* playerTr = player->GetComponent<mts::Transform>();
        // CameraFollow target is resolved by Init() from saved UUID
        camera->GetComponent<mts::Transform>()->SetPosition(playerTr->GetPosition());

        CameraController* camController = camera->GetScript<CameraController>();
        if (camController)
            camController->GetAndInitCamera(conductor);
        PlayerController* playerController = player->GetScript<PlayerController>();
        if (playerController)
            playerController->Init(conductor, camController);
    }
}

void AuralMischiefScene::OnExitPlayMode()
{
    mts::AudioManager::Get().StopMusic();
    if (mRhythmSystem)
    {
        mRhythmSystem->Shutdown();
        mRhythmSystem = nullptr;
    }
}