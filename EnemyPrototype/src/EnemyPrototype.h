#pragma once
#include "pch.h"
#include "EnemyPrototypeScene.h"

class EnemyPrototype : public mts::Application
{
   public:
    EnemyPrototype();
    ~EnemyPrototype();

   private:
    mts::Scene* GetInitialScene() override;

   private:
    EnemyPrototypeScene* m_scene;
};