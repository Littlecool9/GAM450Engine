#pragma once
#include "pch.h"

class RhythmSystem;

class Prototype_OneScene : public mts::Scene
{
   public:
    ~Prototype_OneScene() override;

   protected:
    void OnInit() override;
    void OnUpdate() override;
    void OnEnterPlayMode() override;
    void OnExitPlayMode() override;

   private:
    void OnBeat(mts::BeatInfo info);
    void OnBar(mts::BeatInfo info);

    mts::Delegate<mts::BeatInfo>::Id mBeatSubId = 0;
    mts::Delegate<mts::BeatInfo>::Id mBarSubId = 0;
    int mBeatCount = 0;
    int mBarCount = 0;

    mts::GameObject* camera;
    mts::GameObject* mBeatLine;

    RhythmSystem* mRhythmSystem;
};