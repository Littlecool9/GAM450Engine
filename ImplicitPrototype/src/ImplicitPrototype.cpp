#include "pch.h"
#include "ImplicitPrototype.h"

mts::Application* mts::CreateApplication()
{
    return new ImplicitPrototype();
}

ImplicitPrototype::ImplicitPrototype()
{
}

ImplicitPrototype::~ImplicitPrototype()
{
    
}

mts::Scene* ImplicitPrototype::GetInitialScene()
{
    return new ImplicitPrototypeScene();
}