/**
 * @file main.cpp
 * @author sumin.park
 * @brief The launcher for the engine.
 *
 * @copyright Copyright (c) 2025 - Mitosis Engine
 *
 */

#include "pch.h"
#include <MitosisEngine.h>
#include <filesystem>
#include "Engine/Serialization/FileManager.h"

int main(int argc, char** argv)
{
    char exeBuf[MAX_PATH];
    GetModuleFileNameA(nullptr, exeBuf, MAX_PATH);
    std::string exeDir = std::filesystem::path(exeBuf).parent_path().string();
    mts::FileManager::SetRoot(exeDir);
    std::filesystem::current_path(exeDir);

    mts::Log::Init();
    mts::MemoryManager::Init();

#ifdef _DEBUG
    // Macro to check memory leaks on windows.
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF | _CRTDBG_CHECK_ALWAYS_DF);
    //_CrtSetBreakAlloc(1879);
#endif

    auto app = mts::CreateApplication();
    app->Init();
    app->Run();
    app->Shutdown();
    delete app;

    mts::MemoryManager::Shutdown();
    mts::Log::Shutdown();
    return 0;
}