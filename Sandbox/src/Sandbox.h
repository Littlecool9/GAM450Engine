/**
 * @file Sandbox.h
 * @author sumin.park
 * @brief  Example App class
 * @version 0.1
 * @date 1/8/2026 10:03:59 AM
 *
 * @copyright Copyright (c) 2025 - Mitosis Engine
 *
 */

#pragma once
#include <MitosisEngine.h>

#include "SandboxScene.h"

class Sandbox : public mts::Application
{
   public:
    Sandbox();
    ~Sandbox();

   private:
    mts::Scene* GetInitialScene() override { return exampleScene; }

   private:
    SandboxScene* exampleScene;
};