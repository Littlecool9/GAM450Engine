#pragma once
#include "pch.h"
#include "AuralMischiefScene.h"

class AuralMischief : public mts::Application
{
   public:
    AuralMischief();
    ~AuralMischief();

   private:
    mts::Scene* GetInitialScene() override;

   private:
    AuralMischiefScene* m_scene;
};