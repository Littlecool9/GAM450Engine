#include "pch.h"
#include "Prototype_One.h"

mts::Application* mts::CreateApplication()
{
    return new Prototype_One();
}

Prototype_One::Prototype_One()
{
}

Prototype_One::~Prototype_One()
{
}

mts::Scene* Prototype_One::GetInitialScene()
{
    return new Prototype_OneScene();
}