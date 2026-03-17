#pragma once
#include "pch.h"
#include "ImplicitPrototypeScene.h"

class ImplicitPrototype : public mts::Application
{
   public:
    ImplicitPrototype();
    ~ImplicitPrototype();

   private:
    mts::Scene* GetInitialScene() override;

   private:
    ImplicitPrototypeScene* m_scene;
};