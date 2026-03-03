#include "pch.h"
#include "TechDemoScene.h"

#include "Engine/Audio/AudioManager.h"
#include "Engine/SceneSystem/SceneSystem.h"
#include "Engine/Scripting/ScriptFactory.h"
#include "Engine/Input/Input.h"

#include "Script/BeatBarController.h"
#include "Script/CollectableController.h"
#include "Script/ConductorScript.h"
#include "Script/EnemyController.h"
#include "Script/PlayerController.h"
#include "Script/CameraController.h"
#include "Script/MovablePlatform.h"
#include "Script/PlatformPuzzleScript.h"
#include "Script/RhythmSystem.h"
#include "Script/CheckPoint.h"

static const mts::MusicTrack kMausoleumMash = {"MausoleumMash",
                                               "event:/MausoleumMash",
                                               140.f,
                                               4,
                                               0.f};

TechDemoScene::~TechDemoScene()
{
    mts::ConductorSystem& conductor = mts::ConductorSystem::Get();
    conductor.onBeat.Unsubscribe(mBeatSubId);
    conductor.onBar.Unsubscribe(mBarSubId);
    mRhythmSystem->Shutdown();
}

void TechDemoScene::OnEnterPlayMode()
{
    mts::AudioManager& audio = mts::AudioManager::Get();
    if (!audio.StartMusic(kMausoleumMash))
        LOG_ERROR("[TechDemoScene] StartMusic failed – check event path 'event:/MausoleumMash'.");
    else
        LOG_INFO("[TechDemoScene] MausoleumMash music started.");

    Scene::OnEnterPlayMode();

    auto* player = FindGameObjectByName("Player");
    camera = FindGameObjectByName("CameraObject");
    // background = FindGameObjectByName("Sky");
    // background2 = FindGameObjectByName("BackgroundTest");
    mBeatLine = FindGameObjectByName("BeatLine");

    if (camera && player)
    {
        auto* playerTr = player->GetComponent<mts::Transform>();
        // CameraFollow target is resolved by Init() from saved UUID
        camera->GetComponent<mts::Transform>()->SetPosition(playerTr->GetPosition());
    }
}

void TechDemoScene::OnExitPlayMode()
{
    mts::AudioManager::Get().StopMusic();
}

void TechDemoScene::OnUpdate()
{
    // sync the background to the camera
    mts::Transform* camTransform = camera->GetComponent<mts::Transform>();

    // Hit-window validation: press F to test IsInBeatWindow() on/off beat.
    if (mts::Input::IsKeyPressed(mts::Key::A))
    {
        mts::AudioManager& audio = mts::AudioManager::Get();
        bool inWindow = audio.IsInBeatWindow(0.100f, 0.100f);
        float phase = audio.GetBeatPhase01();
        int beatIdx = audio.GetBeatIndex();

        if (inWindow)
            LOG_INFO("[HIT] A -> ON-BEAT  | beat={} phase={:.3f}", beatIdx, phase);
        else
            LOG_WARN("[HIT] A -> OFF-BEAT | beat={} phase={:.3f}", beatIdx, phase);
    }

    mRhythmSystem->Update();
}

void TechDemoScene::OnBeat(mts::BeatInfo info)
{
    ++mBeatCount;
    // if (info.isHalfBeat)
    //{
    //     LOG_INFO("[BEAT] half  | halfBeatIdx={} beatIdx={} barIdx={} beatInBar={} phase={:.3f}",
    //              info.halfBeatIndex,
    //              info.beatIndex,
    //              info.barIndex,
    //              info.beatInBar,
    //              info.beatPhase);
    // }
    // else
    //{
    //     LOG_INFO("[BEAT] full  | beatIdx={}  barIdx={}  beatInBar={}  phase={:.3f}  (total={})",
    //              info.beatIndex,
    //              info.barIndex,
    //              info.beatInBar,
    //              info.beatPhase,
    //              mBeatCount);
    // }
}

void TechDemoScene::OnBar(mts::BeatInfo info)
{
    ++mBarCount;
    // LOG_INFO("[BAR ] new bar | barIdx={}  started at beatIdx={}  (total bars={})",
    //          info.barIndex,
    //          info.beatIndex,
    //          mBarCount);
}
void TechDemoScene::OnInit()
{
    mts::ConductorSystem& conductor = mts::ConductorSystem::Get();
    mBeatSubId = conductor.onBeat.Subscribe([this](mts::BeatInfo info) { OnBeat(info); });
    mBarSubId = conductor.onBar.Subscribe([this](mts::BeatInfo info) { OnBar(info); });

#pragma region Script Registry

    mts::ScriptFactory::RegisterScript(ConductorScript::StaticClassName(),
                                       []() { return new ConductorScript(); });
    mts::ScriptFactory::RegisterScript("BeatBarController",
                                       []() { return new BeatBarController(); });

    mts::ScriptFactory::RegisterScript("PlayerController", []() { return new PlayerController(); });
    mts::ScriptFactory::RegisterScript("EnemyController", []() { return new EnemyController(); });

    mts::ScriptFactory::RegisterScript("CollectableController",
                                       []() { return new CollectableController(); });
    mts::ScriptFactory::RegisterScript("CameraController", []() { return new CameraController(); });
    mts::ScriptFactory::RegisterScript("MovablePlatform", []() { return new MovablePlatform(); });
    mts::ScriptFactory::RegisterScript(PlatformPuzzleScript::StaticClassName(),
                                       []() { return new PlatformPuzzleScript(); });
    mts::ScriptFactory::RegisterScript("CheckPointController", []() { return new CheckPoint(); });

#pragma endregion

    mts::SceneSystem::LoadScene(this, "../TechDemo/Data/Resources/CurrentScene.json");

    camera = FindGameObjectByName("CameraObject");
    // background = FindGameObjectByName("Sky");

    mRhythmSystem = &RhythmSystem::Get();
    mRhythmSystem->Init(this);  // Essential to initialize RhythmSystem after loading the scene so
                                // it can resolve references
}
