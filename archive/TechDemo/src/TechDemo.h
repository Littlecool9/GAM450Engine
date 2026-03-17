#pragma once
#include <MitosisEngine.h>

#include "TechDemoScene.h"

class TechDemo : public mts::Application
{
   public:
    TechDemo();
    ~TechDemo();

   private:
    mts::Scene* GetInitialScene() override;
};
