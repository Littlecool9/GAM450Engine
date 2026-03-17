#include "pch.h"
#include "TechDemo.h"

#include "Engine/SceneSystem/SceneSystem.h"

mts::Application* mts::CreateApplication()
{
    return new TechDemo();
}

TechDemo::TechDemo()
{
}

TechDemo::~TechDemo()
{
}

mts::Scene* TechDemo::GetInitialScene()
{
    return new TechDemoScene();
}
