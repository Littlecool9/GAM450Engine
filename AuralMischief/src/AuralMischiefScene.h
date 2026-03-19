#pragma once
#include "pch.h"

class RhythmSystem;

class AuralMischiefScene : public mts::Scene
{
   public:
    ~AuralMischiefScene() override;

   protected:
    void OnInit() override;
    void OnUpdate() override;
    void OnEnterPlayMode() override;
    void OnExitPlayMode() override;

   private:
    mts::GameObject* camera;
    mts::GameObject* player;

    RhythmSystem* mRhythmSystem;
};