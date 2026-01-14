/**
 * @file EntryPoint.cpp
 * @author sumin.park
 * @brief The launcher for the engine.
 *
 * @copyright Copyright (c) 2025 - Mitosis Engine
 *
 */

#include "pch.h"
#include <MitosisEngine.h>

int main(int argc, char** argv)
{
    mts::Log::Init();

    auto app = mts::CreateApplication();
    app->Init();
    app->Run();
    app->Shutdown();
    delete app;

    mts::Log::Shutdown();
    return 0;
}