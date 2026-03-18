#include "pch.h"
#include "AuralMischief.h"

#include "Engine/Scripting/ScriptFactory.h"
#include "Script/AttackHitboxScript.h"
#include "Script/CameraController.h"
#include "Script/CheckPoint.h"
#include "Script/MovablePlatform.h"
#include "Script/PlayerController.h"

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
}