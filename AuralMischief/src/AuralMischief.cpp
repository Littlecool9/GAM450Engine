#include "pch.h"
#include "AuralMischief.h"

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