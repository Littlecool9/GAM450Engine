/**
 * @file Sandbox.h
 * @author sumin.park
 * @brief  Example App class
 * @version 0.1
 * @date 1/8/2026 10:03:59 AM
 *
 * @copyright Copyright (c) 2025 - RM Engine
 *
 */

#pragma once
#include <RMEngine.h>

#include "SandboxScene.h"

class Sandbox : public rm::Application {
   public:
    Sandbox();
    ~Sandbox();
   private:
    rm::Scene* GetInitialScene() override { return exampleScene; }
    
private:
    SandboxScene* exampleScene;
};