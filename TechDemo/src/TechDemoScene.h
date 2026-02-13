#pragma once
#include <MitosisEngine.h>

class TechDemoScene : public mts::Scene
{
   public:
    ~TechDemoScene() override;

   protected:
    void OnInit() override;
    void OnUpdate() override;

   private:
    mts::GameObject* camera;
    mts::GameObject* background;
};
