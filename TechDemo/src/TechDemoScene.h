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
    void OnBeat(mts::BeatInfo info);
    void OnBar(mts::BeatInfo info);

    mts::Delegate<mts::BeatInfo>::Id mBeatSubId = 0;
    mts::Delegate<mts::BeatInfo>::Id mBarSubId = 0;
    int mBeatCount = 0;
    int mBarCount = 0;

    mts::GameObject* camera;
    mts::GameObject* background;
    mts::GameObject* background2;
    mts::GameObject* mBeatLine;
};
