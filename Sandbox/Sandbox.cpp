/**
 * @file Sandbox.cpp
 * @author sumin.park
 * @brief  Example App class
 * @version 0.1
 * @date 1/8/2026 10:03:59 AM
 *
 * @copyright Copyright (c) 2025 - RM Engine
 *
 */

#include "pch.h"
#include "Sandbox.h"


rm::Application* rm::CreateApplication() {
    return new Sandbox();
}

Sandbox::Sandbox() {
    exampleScene = new SandboxScene();
}

Sandbox::~Sandbox() {
    delete exampleScene;
}