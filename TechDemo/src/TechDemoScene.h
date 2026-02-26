#pragma once
#include <MitosisEngine.h>

class TechDemoScene : public mts::Scene
{
   public:
    ~TechDemoScene() override;

   protected:
    void OnInit() override;

   private:
    mts::GameObject* camera;
    mts::GameObject* background;
    mts::GameObject* background2;
};
