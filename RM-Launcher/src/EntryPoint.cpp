/**
 * @file EntryPoint.cpp
 * @author sumin.park
 * @brief The launcher for the engine.
 *
 * @copyright Copyright (c) 2025 - RM Engine
 *
 */

#include "pch.h"
#include <RMEngine.h>

int main(int argc, char **argv)
{
	rm::Log::Init();

	auto app = rm::CreateApplication();
	app->Init();
	app->Run();
	app->Shutdown();
	delete app;

	rm::Log::Shutdown();
	return 0;
}