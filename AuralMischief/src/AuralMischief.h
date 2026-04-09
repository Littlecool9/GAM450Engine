#pragma once
#include "pch.h"

class AuralMischief : public mts::Application
{
   public:
    AuralMischief();
    ~AuralMischief();

   private:
    std::string& GetInitialScene() override;
    std::string& GetSceneRegistry() override;
    void RegisterScripts() override;
    std::vector<mts::SplashScreenConfig> GetSplashScreens() override;
    void OnUpdate(float dt) override;

   private:
    std::string mInitialScene = "Data/Scenes/CurrentScene.scene";
    std::string mSceneRegistry = "Data/Properties/SceneRegistry.json";
    std::string mSplashScreenConfigPath = "Data/Splashscreen.json";

    // Cheat Code System state variables
    bool mCheatsEnabled = false;
    bool mMusicMuted = false;
    float mMusicVolume = 0.0f;
};