#pragma once
#include "pch.h"

class EnemyPrototypeScene : public mts::Scene
{
   public:
    ~EnemyPrototypeScene() override;

   protected:
    void OnInit() override;
    void OnUpdate() override;
    void OnBeat(mts::BeatInfo info);
    void OnBar(mts::BeatInfo info);

    mts::Delegate<mts::BeatInfo>::Id mBeatSubId = 0;
    mts::Delegate<mts::BeatInfo>::Id mBarSubId = 0;
    int mBeatCount = 0;
    int mBarCount = 0;

    mts::GameObject* camera;
    mts::GameObject* background;
    mts::GameObject* background2;
    mts::GameObject* enemy;
    mts::GameObject* mBeatLine;
};