#include "pch.h"
#include "TechDemo.h"

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
