#include "pch.h"
#include "AuralMischief.h"

#include "Engine/Scripting/ScriptFactory.h"
#include "Script/AttackHitboxScript.h"
#include "Script/CameraController.h"
#include "Script/CheckPoint.h"
#include "Script/MovablePlatform.h"
#include "Script/PlayerController.h"
#include "Script/HazardCollider.h"
#include "Script/PlatformPuzzleScript.h"
#include "Script/EnemyBehaviors/EnemyController.h"
#include "Script/AudioController.h"

mts::Application* mts::CreateApplication()
{
    return new AuralMischief();
}

AuralMischief::AuralMischief()
{
    m_scene = new AuralMischiefScene();
}

AuralMischief::~AuralMischief()
{
    delete m_scene;
}

mts::Scene* AuralMischief::GetInitialScene()
{
    return m_scene;
}

std::vector<mts::SplashScreenConfig> AuralMischief::GetSplashScreens()
{
    return mts::SplashScreen::LoadFromJson("Data/Splashscreen.json");
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
    mts::ScriptFactory::RegisterScript(AudioController::StaticClassName(),
                                       []() { return new AudioController(); });
}