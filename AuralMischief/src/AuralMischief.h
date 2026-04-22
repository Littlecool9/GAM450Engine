/**
 * @file AuralMischief.h
 * @author sumin.park
 * @brief  Application subclass for AuralMischief; registers all scripts, configures window props,
 * initial scene, scene registry, and splash screens.
 *
 * @copyright (c) 2026 DigiPen (USA) Corporation
 *
 */

#pragma once
#include "pch.h"

class AuralMischief : public mts::Application
{
   public:
    AuralMischief();
    ~AuralMischief();

    static AuralMischief* Get() { return sInstance; }

    bool HasAppliedInitialVolumes() const { return mInitialVolumesApplied; }
    void MarkInitialVolumesApplied() { mInitialVolumesApplied = true; }

   private:
    std::string& GetInitialScene() override;
    std::string& GetSceneRegistry() override;
    mts::WindowProps& GetWindowProps() override { return mWindowProps; }
    void RegisterScripts() override;
    std::vector<mts::SplashScreenConfig> GetSplashScreens() override;
    void OnUpdate(float dt) override;

   private:
    static AuralMischief* sInstance;

    std::string mInitialScene = "Data/Scenes/MainMenu.scene";
    std::string mSceneRegistry = "Data/Properties/SceneRegistry.json";
    std::string mSplashScreenConfigPath = "Data/Splashscreen.json";
    mts::WindowProps mWindowProps;

    // One-shot seed for AudioManager category volumes — AudioController::Init()
    // applies scene-authored volumes only on the first scene of the session; later
    // scene switches inherit whatever the player has adjusted at runtime.
    bool mInitialVolumesApplied = false;

    // Cheat Code System state variables
    bool mCheatsEnabled = false;
    bool mMusicMuted = false;
    float mMusicVolume = 0.0f;
};