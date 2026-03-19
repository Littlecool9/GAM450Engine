#include "pch.h"
#include "AuralMischiefScene.h"

#include "Engine/EntitySystem/Component/Animation.h"
#include "Engine/EntitySystem/Component/AnimationSystem.h"
#include "Engine/EntitySystem/Component/EnemyBehavior/EnemyAgent.h"
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

void AuralMischiefScene::OnEnterPlayMode()
{

}