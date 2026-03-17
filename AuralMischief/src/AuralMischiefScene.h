#pragma once
#include "pch.h"

class AuralMischiefScene : public mts::Scene
{
   public:
    ~AuralMischiefScene() override;

   protected:
    void OnInit() override;
    void OnUpdate() override;
};