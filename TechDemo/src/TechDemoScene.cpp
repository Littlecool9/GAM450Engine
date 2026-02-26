#include "pch.h"
#include "TechDemoScene.h"

#include "Engine/Audio/AudioManager.h"
#include "Engine/EntitySystem/Component/Camera/CameraFollow.h"
#include "Engine/EntitySystem/GameObject/GameObjectFactory.h"
#include "Engine/SceneSystem/SceneSystem.h"
#include "Engine/Scripting/ScriptFactory.h"
#include "Engine/Serialization/SerializationUtility.h"
#include "Script/BeatBarController.h"
#include "Script/CollectableController.h"
#include "Script/EnemyController.h"
#include "Script/PlayerController.h"
#include "Engine/Input/Input.h"

#define TestLoad 0

TechDemoScene::~TechDemoScene()
{
    mts::ConductorSystem& conductor = mts::ConductorSystem::Get();
    conductor.onBeat.Unsubscribe(mBeatSubId);
    conductor.onBar.Unsubscribe(mBarSubId);
}

void TechDemoScene::OnUpdate()
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
}

void TechDemoScene::OnBeat(mts::BeatInfo info)
{
    ++mBeatCount;
    if (info.isHalfBeat)
    {
        LOG_INFO("[BEAT] half  | halfBeatIdx={} beatIdx={} barIdx={} beatInBar={} phase={:.3f}",
                 info.halfBeatIndex,
                 info.beatIndex,
                 info.barIndex,
                 info.beatInBar,
                 info.beatPhase);
    }
    else
    {
        LOG_INFO("[BEAT] full  | beatIdx={}  barIdx={}  beatInBar={}  phase={:.3f}  (total={})",
                 info.beatIndex,
                 info.barIndex,
                 info.beatInBar,
                 info.beatPhase,
                 mBeatCount);
    }
}

void TechDemoScene::OnBar(mts::BeatInfo info)
{
    ++mBarCount;
    LOG_INFO("[BAR ] new bar | barIdx={}  started at beatIdx={}  (total bars={})",
             info.barIndex,
             info.beatIndex,
             mBarCount);
}

static const mts::MusicTrack kMausoleumMash = {"MausoleumMash",
                                               "event:/MausoleumMash",
                                               140.f,
                                               4,
                                               0.f};

void TechDemoScene::OnInit()
{
    // Banks are loaded by Application (MausoleumMash Core + Music groups).
    mts::AudioManager& audio = mts::AudioManager::Get();
    if (!audio.StartMusic(kMausoleumMash))
        LOG_ERROR("[TechDemoScene] StartMusic failed – check event path 'event:/MausoleumMash'.");
    else
        LOG_INFO("[TechDemoScene] MausoleumMash music started.");

    mts::ConductorSystem& conductor = mts::ConductorSystem::Get();
    mBeatSubId = conductor.onBeat.Subscribe([this](mts::BeatInfo info) { OnBeat(info); });
    mBarSubId = conductor.onBar.Subscribe([this](mts::BeatInfo info) { OnBar(info); });

    mts::ScriptFactory::RegisterScript("BeatBarController",
                                       []() { return new BeatBarController(); });

    mts::ScriptFactory::RegisterScript("PlayerController", []() { return new PlayerController(); });

    mts::ScriptFactory::RegisterScript("EnemyController", []() { return new EnemyController(); });

    mts::ScriptFactory::RegisterScript("CollectableController",
                                       []() { return new CollectableController(); });

#if TestLoad 1
    mts::SceneSystem::LoadScene(this, "../TechDemo/Data/Resources/CurrentScene.json");

    auto player = FindGameObjectByName("Player");
    camera = FindGameObjectByName("CameraObject");
    assert(camera != nullptr);

    camera->GetComponent<mts::CameraFollow>()->SetObjectToFollow(
        player->GetComponent<mts::Transform>());

    background = FindGameObjectByName("Sky");
#else

    auto player = mts::GameObjectFactory::InstantiateFromJSON("Data/Player.json");
    player->AddTag("Player");

    mts::GameObjectFactory::InstantiateFromJSON("Data/Ground.json");

    camera = mts::GameObjectFactory::InstantiateFromJSON("Data/Camera.json");
    camera->SetName("CameraObject");
    camera->GetComponent<mts::CameraFollow>()->SetObjectToFollow(
        player->GetComponent<mts::Transform>());

    auto crate1 = mts::GameObjectFactory::InstantiateFromJSON("Data/Crate.json");
    crate1->SetName("Crate1");
    mts::Transform* crate1Transform = crate1->GetComponent<mts::Transform>();
    crate1Transform->SetPosition(glm::vec2(5, 10));

    auto crate2 = mts::GameObjectFactory::InstantiateFromJSON("Data/Crate.json");
    crate2->SetName("Crate2");
    mts::Transform* crate2Transform = crate2->GetComponent<mts::Transform>();
    crate2Transform->SetPosition(glm::vec2(13, 9));

    auto crate3 = mts::GameObjectFactory::InstantiateFromJSON("Data/Crate.json");
    crate3->SetName("Crate3");
    mts::Transform* crate3Transform = crate3->GetComponent<mts::Transform>();
    crate3Transform->SetPosition(glm::vec2(13, 10));

    std::vector<glm::vec2> grassPos = {{7.0f, 7.4f},
                                       {3.0f, 7.4f},
                                       {12.0f, 4.4f},
                                       {20.0f, 7.4f},
                                       {24.0f, 7.4f}};

    for (auto pos : grassPos)
    {
        auto grass = mts::GameObjectFactory::InstantiateFromJSON("Data/GrassPatch.json");
        mts::Transform* grassTransform = grass->GetComponent<mts::Transform>();
        grassTransform->SetPosition(pos);
    }

    std::vector<glm::vec2> fencePos = {{1.0f, 7.4f},
                                       {2.0f, 7.4f},
                                       {3.0f, 7.4f},
                                       {4.0f, 7.4f},
                                       {5.0f, 7.4f},
                                       {6.0f, 7.4f},
                                       {7.0f, 7.4f},
                                       {22.0f, 7.4f},
                                       {23.0f, 7.4f},
                                       {24.0f, 7.4f},
                                       {25.0f, 7.4f},
                                       {26.0f, 7.4f},
                                       {27.0f, 7.4f},
                                       {28.0f, 7.4f}};

    for (auto pos : fencePos)
    {
        auto fence = mts::GameObjectFactory::InstantiateFromJSON("Data/BackgroundFence.json");
        mts::Transform* fenceTransform = fence->GetComponent<mts::Transform>();
        fenceTransform->SetPosition(pos);
    }

    std::vector<glm::vec2> blockPos = {{16.5, 5.5}, {16.5, 4.5}, {15.5, 4.5}};

    for (auto pos : blockPos)
    {
        auto block = mts::GameObjectFactory::InstantiateFromJSON("Data/StaticBlock.json");
        mts::Transform* blockTransform = block->GetComponent<mts::Transform>();
        blockTransform->SetPosition(pos);
    }

    std::vector<glm::vec2> platformPos = {{9.5, 6.5}, {10.5, 6.5}};

    for (auto pos : platformPos)
    {
        auto platform = mts::GameObjectFactory::InstantiateFromJSON("Data/WoodPlatform.json");
        mts::Transform* platformTransform = platform->GetComponent<mts::Transform>();
        platformTransform->SetPosition(pos);
    }

    std::vector<glm::vec2> gemPos =
        {{6, 10.5}, {7, 11}, {8, 10.5}, {20, 10.5}, {21, 11}, {22, 10.5}};

    for (auto pos : gemPos)
    {
        auto gem = mts::GameObjectFactory::InstantiateFromJSON("Data/Gem.json");
        mts::Transform* gemTransform = gem->GetComponent<mts::Transform>();
        gemTransform->SetPosition(pos);
    }

    background = mts::GameObjectFactory::InstantiateFromJSON("Data/Sky.json");
    background2 = mts::GameObjectFactory::InstantiateFromJSON("Data/BackgroundTest.json");

    mBeatLine = mts::GameObjectFactory::InstantiateFromJSON("Data/BeatLine.json");

    // if this is not created last it crashes
    auto tilemap = mts::GameObjectFactory::InstantiateFromJSON("Data/DemoTilemapEntity.json");

    mts::SceneSystem::SaveScene(this, "../TechDemo/Data/Resources/CurrentScene.json");
#endif
}
