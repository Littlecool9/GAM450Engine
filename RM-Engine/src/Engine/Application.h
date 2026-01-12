/**
 * @file Application.h
 * @author sumin.park
 * @brief The application class that the app will inherit.
 *
 * @copyright Copyright (c) 2025 - RM Engine
 *
 */
#pragma once
#include "Engine/Windows/Window.h"
#include <memory>

namespace rm
{
	class Application
	{
	public:
		Application() = default;
		virtual ~Application() = default;
		void Init();
		void Run();
		void Shutdown();

		void OnEvent(Event &e);

	private:
		bool OnWindowClose(class WindowCloseEvent &e);
		bool OnWindowResize(class WindowResizeEvent &e);

		std::unique_ptr<Window> window;
		bool isRunning = false;
	};

	Application *CreateApplication();
}