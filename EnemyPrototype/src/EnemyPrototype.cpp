#include "pch.h"
#include "EnemyPrototype.h"

mts::Application* mts::CreateApplication()
{
    return new EnemyPrototype();
}

EnemyPrototype::EnemyPrototype()
{
    m_scene = new EnemyPrototypeScene();
}

EnemyPrototype::~EnemyPrototype()
{
    //delete m_scene;
}

mts::Scene* EnemyPrototype::GetInitialScene()
{
    return m_scene;
}