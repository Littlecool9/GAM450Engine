#pragma once
#include "pch.h"

class PlatformerDemoScene : public mts::Scene
{
   public:
    ~PlatformerDemoScene() override;

   protected:
    void OnInit() override;
    void OnUpdate() override;
    void OnEnterPlayMode() override;
    void OnExitPlayMode() override;

   private:
    mts::GameObject* camera;
    mts::GameObject* background;
};