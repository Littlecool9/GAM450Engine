#pragma once
#include "pch.h"

class EnemyPrototypeScene : public mts::Scene
{
   public:
    ~EnemyPrototypeScene() override;

   protected:
    void OnInit() override;
    void OnUpdate() override;
    void OnEnterPlayMode() override;
    void OnExitPlayMode() override;


    mts::Delegate<mts::BeatInfo>::Id mBeatSubId = 0;
    mts::Delegate<mts::BeatInfo>::Id mBarSubId = 0;
    int mBeatCount = 0;
    int mBarCount = 0;

    mts::GameObject* camera;
    mts::GameObject* background;
    mts::GameObject* background2;
    mts::GameObject* player;
    mts::GameObject* enemy;
    mts::GameObject* mBeatLine;
    mts::GameObject* enemyhp;
    mts::GameObject* playerhp;
};