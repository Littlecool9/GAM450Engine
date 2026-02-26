#pragma once
#include "pch.h"
#include "PlatformerDemoScene.h"

class PlatformerDemo : public mts::Application
{
   public:
    PlatformerDemo();
    ~PlatformerDemo();

   private:
    mts::Scene* GetInitialScene() override;

   private:
    PlatformerDemoScene* m_scene;
};