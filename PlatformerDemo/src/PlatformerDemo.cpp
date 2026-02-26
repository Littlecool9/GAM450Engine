#include "pch.h"
#include "PlatformerDemo.h"

mts::Application* mts::CreateApplication()
{
    return new PlatformerDemo();
}

PlatformerDemo::PlatformerDemo()
{
    m_scene = new PlatformerDemoScene();
}

PlatformerDemo::~PlatformerDemo()
{
    delete m_scene;
}

mts::Scene* PlatformerDemo::GetInitialScene()
{
    return m_scene;
}