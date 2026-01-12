/**
 * @file InputEvent.h
 * @author rahul
 * @brief // Engine-defined input events for keyboard and mouse interactions.
 *
 * @copyright Copyright (c) 2025 - RM Engine
 *
 */

#pragma once
#include <sstream>

#include "Event.h"
#include "Engine/Input/InputCodes.h"

namespace rm
{
	class KeyEvent : public Event
	{
	public:
		Key GetKeyCode() const { return m_Key; }

		RM_EVENT_CLASS_CATEGORY(EventCategoryInput | EventCategoryKeyboard)

	protected:
		explicit KeyEvent(Key key) : m_Key(key) {}
		Key m_Key = Key::Unknown;
	};

	class KeyPressedEvent : public KeyEvent
	{
	public:
		KeyPressedEvent(Key key, int repeatCount)
			: KeyEvent(key), m_RepeatCount(repeatCount)
		{
		}

		int GetRepeatCount() const { return m_RepeatCount; }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "KeyPressedEvent: " << static_cast<int>(m_Key) << " (" << m_RepeatCount << " repeats)";
			return ss.str();
		}

		RM_EVENT_CLASS_TYPE(KeyPressed)

	private:
		int m_RepeatCount = 0;
	};

	class KeyReleasedEvent : public KeyEvent
	{
	public:
		explicit KeyReleasedEvent(Key key)
			: KeyEvent(key)
		{
		}

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "KeyReleasedEvent: " << static_cast<int>(m_Key);
			return ss.str();
		}

		RM_EVENT_CLASS_TYPE(KeyReleased)
	};

	class MouseMovedEvent : public Event
	{
	public:
		MouseMovedEvent(float x, float y) : m_MouseX(x), m_MouseY(y) {}

		float GetX() const { return m_MouseX; }
		float GetY() const { return m_MouseY; }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "MouseMovedEvent: " << m_MouseX << ", " << m_MouseY;
			return ss.str();
		}

		RM_EVENT_CLASS_TYPE(MouseMoved)
		RM_EVENT_CLASS_CATEGORY(EventCategoryInput | EventCategoryMouse)

	private:
		float m_MouseX, m_MouseY;
	};

	class MouseScrolledEvent : public Event
	{
	public:
		MouseScrolledEvent(float xOffset, float yOffset) : m_XOffset(xOffset), m_YOffset(yOffset) {}

		float GetXOffset() const { return m_XOffset; }
		float GetYOffset() const { return m_YOffset; }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "MouseScrolledEvent: " << m_XOffset << ", " << m_YOffset;
			return ss.str();
		}

		RM_EVENT_CLASS_TYPE(MouseScrolled)
		RM_EVENT_CLASS_CATEGORY(EventCategoryInput | EventCategoryMouse)

	private:
		float m_XOffset, m_YOffset;
	};

	class MouseButtonEvent : public Event
	{
	public:
		MouseButton GetMouseButton() const { return m_Button; }

		RM_EVENT_CLASS_CATEGORY(EventCategoryInput | EventCategoryMouse | EventCategoryMouseButton)

	protected:
		explicit MouseButtonEvent(MouseButton button) : m_Button(button) {}
		MouseButton m_Button = MouseButton::Unknown;
	};

	class MouseButtonPressedEvent : public MouseButtonEvent
	{
	public:
		explicit MouseButtonPressedEvent(MouseButton button) : MouseButtonEvent(button) {}

		RM_EVENT_CLASS_TYPE(MouseButtonPressed)
	};

	class MouseButtonReleasedEvent : public MouseButtonEvent
	{
	public:
		explicit MouseButtonReleasedEvent(MouseButton button) : MouseButtonEvent(button) {}

		RM_EVENT_CLASS_TYPE(MouseButtonReleased)
	};

} // rm namespace