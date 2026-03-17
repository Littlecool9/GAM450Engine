#pragma once
#include "pch.h"
#include "Prototype_OneScene.h"

class Prototype_One : public mts::Application
{
   public:
    Prototype_One();
    ~Prototype_One();

   private:
    mts::Scene* GetInitialScene() override;
};