/**
 * @file AuralMischief.h
 * @author sumin.park
 * @brief  Application subclass for AuralMischief; registers all scripts, configures window props,
 * initial scene, scene registry, and splash screens.
 *
 * @copyright (c) 2026 DigiPen (USA) Corporation
 *
 */
#include "pch.h"
#include "AuralMischief.h"

#include "Script/AttackHitboxScript.h"
#include "Script/CameraController.h"
#include "Script/CheckPoint.h"
#include "Script/MovablePlatform.h"
#include "Script/Player/PlayerController.h"
#include "Script/HazardCollider.h"
#include "Script/PlatformPuzzleScript.h"
#include "Script/EnemyBehaviors/EnemyController.h"
#include "Script/EnemyBehaviors/EnemyAudioController.h"
#include "Script/AudioController.h"
#include "Script/PauseButton.h"
#include "Script/DamageNumberScript.h"
#include "Script/DamageNumberSpawner.h"
#include "Script/RhythmSystemScript.h"
#include "Script/EnemyBehaviors/BulletBehavior.h"
#include "Script/BeatPulseScript.h"
#include "Script/EndPointScript.h"
#include "Script/SceneSwitcherScript.h"
#include "Script/Player/PlayerAudioController.h"
#include "Script/MainMenuButton.h"
#include "Script/FadeInScript.h"
#include "Script/BeatBarController.h"
#include "Script/TilemapBeatFlash.h"
#include "Script/PauseMenuPanel.h"
#include "Script/PauseMenuNavigator.h"
#include "Script/VolumeStepButton.h"
#include "Script/ResumeButton.h"
#include "Script/ReturnToMainMenuButton.h"
#include "Script/CreditsScrollScript.h"
#include "Script/UIPanels.h"
#include "Script/ReturnButton.h"
#include "Script/InputGuidanceScript.h"
#include "Script/IntroScreenScript.h"

#include "Engine/Renderer.h"
#include "Engine/Scripting/ScriptFactory.h"
#include "Engine/Audio/AudioManager.h"
#include "Engine/Input/Input.h"
#include "Engine/SceneSystem/SceneSystem.h"
#include "Engine/EntitySystem/Component/Transform.h"
#include "Engine/EntitySystem/GameObject/GameObject.h"
#include "Engine/EntitySystem/GameObject/GameObjectFactory.h"
#include "Script/BossRoomTrigger.h"
#include "Script/CreditsButton.h"
#include "Script/QuitButton.h"
#include "Script/MenuNavigator.h"

mts::Application* mts::CreateApplication()
{
    return new AuralMischief();
}

AuralMischief* AuralMischief::sInstance = nullptr;

AuralMischief::AuralMischief()
{
    sInstance = this;
    mWindowProps = {.Title = "Aural Mischief", .Width = 1600, .Height = 900, .VSync = false};
    mts::Renderer::SetUIReferenceSize(1600, 900);
}

AuralMischief::~AuralMischief()
{
    if (sInstance == this)
        sInstance = nullptr;
}

std::string& AuralMischief::GetInitialScene()
{
    return mInitialScene;
}

std::string& AuralMischief::GetSceneRegistry()
{
    return mSceneRegistry;
}

std::vector<mts::SplashScreenConfig> AuralMischief::GetSplashScreens()
{
    return mts::SplashScreen::LoadFromJson(mSplashScreenConfigPath);
}

void AuralMischief::RegisterScripts()
{
    mts::ScriptFactory::RegisterScript(PlayerController::StaticClassName(),
                                       []() { return new PlayerController(); });
    mts::ScriptFactory::RegisterScript(AttackHitboxScript::StaticClassName(),
                                       []() { return new AttackHitboxScript(); });
    mts::ScriptFactory::RegisterScript(CameraController::StaticClassName(),
                                       []() { return new CameraController(); });
    mts::ScriptFactory::RegisterScript(CheckPoint::StaticClassName(),
                                       []() { return new CheckPoint(); });
    mts::ScriptFactory::RegisterScript(MovablePlatform::StaticClassName(),
                                       []() { return new MovablePlatform(); });
    mts::ScriptFactory::RegisterScript(HazardCollider::StaticClassName(),
                                       []() { return new HazardCollider(); });
    mts::ScriptFactory::RegisterScript(PlatformPuzzleScript::StaticClassName(),
                                       []() { return new PlatformPuzzleScript(); });
    mts::ScriptFactory::RegisterScript(mts::EnemyController::StaticClassName(),
                                       []() { return new mts::EnemyController(); });
    mts::ScriptFactory::RegisterScript(EnemyAudioController::StaticClassName(),
                                       []() { return new EnemyAudioController(); });
    mts::ScriptFactory::RegisterScript(AudioController::StaticClassName(),
                                       []() { return new AudioController(); });
    mts::ScriptFactory::RegisterScript(PauseButton::StaticClassName(),
                                       []() { return new PauseButton(); });
    mts::ScriptFactory::RegisterScript(DamageNumberScript::StaticClassName(),
                                       []() { return new DamageNumberScript(); });
    mts::ScriptFactory::RegisterScript(DamageNumberSpawner::StaticClassName(),
                                       []() { return new DamageNumberSpawner(); });
    mts::ScriptFactory::RegisterScript(mts::BulletBehavior::StaticClassName(),
                                       []() { return new mts::BulletBehavior(); });
    mts::ScriptFactory::RegisterScript(BeatPulseScript::StaticClassName(),
                                       []() { return new BeatPulseScript(); });
    mts::ScriptFactory::RegisterScript(RhythmSystemScript::StaticClassName(),
                                       []() { return new RhythmSystemScript(); });
    mts::ScriptFactory::RegisterScript(SceneSwitcherScript::StaticClassName(),
                                       []() { return new SceneSwitcherScript(); });
    mts::ScriptFactory::RegisterScript(PlayerAudioController::StaticClassName(),
                                       []() { return new PlayerAudioController(); });
    mts::ScriptFactory::RegisterScript(QuitButton::StaticClassName(),
                                       []() { return new QuitButton(); });
    mts::ScriptFactory::RegisterScript(MainMenuButton::StaticClassName(),
                                       []() { return new MainMenuButton(); });
    mts::ScriptFactory::RegisterScript(CreditsButton::StaticClassName(),
                                       []() { return new CreditsButton(); });
    mts::ScriptFactory::RegisterScript(FadeInScript::StaticClassName(),
                                       []() { return new FadeInScript(); });
    mts::ScriptFactory::RegisterScript(EndPointScript::StaticClassName(),
                                       []() { return new EndPointScript(); });
    mts::ScriptFactory::RegisterScript(BeatBarController::StaticClassName(),
                                       []() { return new BeatBarController(); });
    mts::ScriptFactory::RegisterScript(TilemapBeatFlash::StaticClassName(),
                                       []() { return new TilemapBeatFlash(); });
    mts::ScriptFactory::RegisterScript(PauseMenuPanel::StaticClassName(),
                                       []() { return new PauseMenuPanel(); });
    mts::ScriptFactory::RegisterScript(VolumeStepButton::StaticClassName(),
                                       []() { return new VolumeStepButton(); });
    mts::ScriptFactory::RegisterScript(ResumeButton::StaticClassName(),
                                       []() { return new ResumeButton(); });
    mts::ScriptFactory::RegisterScript(ReturnToMainMenuButton::StaticClassName(),
                                       []() { return new ReturnToMainMenuButton(); });
    mts::ScriptFactory::RegisterScript(PauseMenuNavigator::StaticClassName(),
                                       []() { return new PauseMenuNavigator(); });
    mts::ScriptFactory::RegisterScript(CreditsScrollScript::StaticClassName(),
                                       []() { return new CreditsScrollScript(); });
    mts::ScriptFactory::RegisterScript(UIPanels::StaticClassName(),
                                       []() { return new UIPanels(); });
    mts::ScriptFactory::RegisterScript(ReturnButton::StaticClassName(),
                                       []() { return new ReturnButton(); });
    mts::ScriptFactory::RegisterScript(InputGuidanceScript::StaticClassName(),
                                       []() { return new InputGuidanceScript(); });
    mts::ScriptFactory::RegisterScript(MenuNavigator::StaticClassName(),
                                       []() { return new MenuNavigator(); });
    mts::ScriptFactory::RegisterScript(IntroScreenScript::StaticClassName(),
                                       []() { return new IntroScreenScript(); });
    mts::ScriptFactory::RegisterScript(BossRoomTrigger::StaticClassName(),
                                       []() { return new BossRoomTrigger(); });
}

void AuralMischief::OnUpdate(float dt)
{
    if (mts::Input::IsKeyPressed(mts::Key::C))
    {
        if (mts::Input::IsKeyDown(mts::Key::LeftShift) && mts::Input::IsKeyDown(mts::Key::LeftCtrl))
        {
            mCheatsEnabled = !mCheatsEnabled;
        }
        if (!mCheatsEnabled)
        {
            // When cheats are off, remove UI indicator
            mts::GameObject* cheatsText =
                mts::SceneSystem::GetCurrentScene()->FindGameObjectByName("CheatsText");
            if (cheatsText)
                cheatsText->Destroy();
        }
    }

    if (mts::Input::IsKeyPressed(mts::Key::M))
    {
        // Toggle mute music
        if (!mMusicMuted)
        {
            mMusicVolume = mts::AudioManager::Get().GetCategoryVolume(mts::AudioCategory::Music);
            mts::AudioManager::Get().SetCategoryVolume(mts::AudioCategory::Music, 0.0f);
        }
        else
        {
            mts::AudioManager::Get().SetCategoryVolume(mts::AudioCategory::Music, mMusicVolume);
        }
        mMusicMuted = !mMusicMuted;
    }

    if (mCheatsEnabled)
    {
        if (mts::Input::IsKeyPressed(mts::Key::F1))
        {
            // Go to title
            mts::SceneSystem::SwitchScene(1);
        }
        if (mts::Input::IsKeyPressed(mts::Key::F2))
        {
            // Go to level 1
            mts::SceneSystem::SwitchScene(0);
        }
        if (mts::Input::IsKeyPressed(mts::Key::F3))
        {
            // Go to level 2
            mts::SceneSystem::SwitchScene(4);
        }
        if (mts::Input::IsKeyPressed(mts::Key::F4))
        {
            // Go to credits
            // TODO add correct scene index
            mts::SceneSystem::SwitchScene(5);
        }
        if (mts::Input::IsKeyPressed(mts::Key::F5))
        {
            // Toggle god mode
            mts::GameObject* player =
                mts::SceneSystem::GetCurrentScene()->FindGameObjectByName("Player");
            if (player)
            {
                PlayerController* playerController = player->GetScript<PlayerController>();
                if (playerController)
                {
                    playerController->ToggleGodMode();
                }
            }
        }
        if (mts::Input::IsKeyPressed(mts::Key::F6))
        {
            // Kill player
            mts::GameObject* player =
                mts::SceneSystem::GetCurrentScene()->FindGameObjectByName("Player");
            if (player)
            {
                PlayerController* playerController = player->GetScript<PlayerController>();
                if (playerController)
                {
                    playerController->TakeDamage(10000);
                }
            }
        }
        if (mts::Input::IsKeyPressed(mts::Key::F7))
        {
            // Jump to end of level (if applicable)
            mts::GameObject* player =
                mts::SceneSystem::GetCurrentScene()->FindGameObjectByName("Player");
            if (player)
            {
                mts::GameObject* levelEnd =
                    mts::SceneSystem::GetCurrentScene()->FindGameObjectByName("LevelEndTeleport");
                if (levelEnd)
                {
                    mts::Transform* playerTransform = player->GetComponent<mts::Transform>();
                    mts::Transform* levelEndTransform = levelEnd->GetComponent<mts::Transform>();
                    if (playerTransform && levelEndTransform)
                    {
                        playerTransform->SetPosition(levelEndTransform->GetPosition());
                    }
                }
            }
        }

        // When cheats are on, spawn UI indicator
        mts::GameObject* cheatsText =
            mts::SceneSystem::GetCurrentScene()->FindGameObjectByName("CheatsText");
        if (!cheatsText)
            mts::GameObjectFactory::CreateFromArchetype("Data/CheatsText.prefab");
    }
}