/**
 * @file SandboxScene.h
 * @author sumin.park
 * @brief  Example Scene class
 * @version 0.1
 * @date 1/8/2026 10:15:55 AM
 *
 * @copyright Copyright (c) 2025 - Mitosis Engine
 *
 */

#pragma once
#include <MitosisEngine.h>

class SandboxScene : public mts::Scene
{
   public:
    ~SandboxScene() override;

   protected:
    void OnInit() override;
};
