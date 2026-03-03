#pragma once
#include "pch.h"

class RhythmSystem;

class ImplicitPrototypeScene : public mts::Scene
{
   public:
    ~ImplicitPrototypeScene() override;

   protected:
    void OnInit() override;
    void OnUpdate() override;
    void OnEnterPlayMode() override;
    void OnExitPlayMode() override;
    void OnBeat(mts::BeatInfo info);
    void OnBar(mts::BeatInfo info);

    mts::Delegate<mts::BeatInfo>::Id mBeatSubId = 0;
    mts::Delegate<mts::BeatInfo>::Id mBarSubId = 0;
    int mBeatCount = 0;
    int mBarCount = 0;

    mts::GameObject* camera;

    RhythmSystem* mRhythmSystem;
};