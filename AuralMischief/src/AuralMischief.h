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
    mts::WindowProps& GetWindowProps() override { return mWindowProps; }
    void RegisterScripts() override;
    std::vector<mts::SplashScreenConfig> GetSplashScreens() override;
    void OnUpdate(float dt) override;

   private:
    std::string mInitialScene = "Data/Scenes/MainMenu.scene";
    std::string mSceneRegistry = "Data/Properties/SceneRegistry.json";
    std::string mSplashScreenConfigPath = "Data/Splashscreen.json";
    mts::WindowProps mWindowProps{.IconPath = "icon.png"};

    // Cheat Code System state variables
    bool mCheatsEnabled = false;
    bool mMusicMuted = false;
    float mMusicVolume = 0.0f;
};