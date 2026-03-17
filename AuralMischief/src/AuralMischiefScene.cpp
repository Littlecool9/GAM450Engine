#include "pch.h"
#include "AuralMischiefScene.h"

#include "Engine/SceneSystem/SceneSystem.h"

AuralMischiefScene::~AuralMischiefScene()
{
}

void AuralMischiefScene::OnInit()
{
    mts::SceneSystem::LoadScene(this, "Data/Resources/CurrentScene.json");
}

void AuralMischiefScene::OnUpdate()
{
}