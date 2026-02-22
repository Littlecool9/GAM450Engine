#include "pch.h"
#include "TechDemo.h"

#include "Engine/SceneSystem/SceneSystem.h"

mts::Application* mts::CreateApplication()
{
    return new TechDemo();
}

TechDemo::TechDemo()
{
    exampleScene = new TechDemoScene();
}

TechDemo::~TechDemo()
{
    delete exampleScene;
}

mts::Scene* TechDemo::GetInitialScene()
{
    return exampleScene;
}
