#pragma once
#include "pch.h"

class EnemyPrototypeScene : public mts::Scene
{
   public:
    ~EnemyPrototypeScene() override;

   protected:
    void OnInit() override;
    void OnUpdate() override;
};