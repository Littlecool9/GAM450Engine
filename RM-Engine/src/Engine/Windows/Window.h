/**
 * @file Window.h
 * @author sumin.park
 * @brief The interface representing the window.
 *
 * @copyright Copyright (c) 2025 - RM Engine
 *
 */

#pragma once
#include <memory>
#include <string>
#include "Engine/Events/Event.h"

namespace rm
{
	struct WindowProps
	{
		std::string Title = "RM-Engine";
		uint32_t Width = 1600;
		uint32_t Height = 900;
		bool VSync = true;
	};

	class Window
	{
	public:
		virtual ~Window() = default;

		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;
		virtual bool GetVsync() const = 0;

		virtual void SetVsync(bool enable) = 0;

		virtual void Update() = 0;

		virtual void SetEventCallback(const EventCallbackFn &callback) = 0;

		virtual void *NativeWindow() const = 0;

		static std::unique_ptr<Window> Create(const WindowProps &props = {});

	protected:
		Window() = default;
	};

} // rm namespace