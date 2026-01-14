/**
 * @file WindowEvent.h
 * @author rahul
 * @brief // Engine window events such as resize and close notifications.
 *
 * @copyright Copyright (c) 2025 - Mitosis Engine
 *
 */

#pragma once
#include <sstream>

#include "Event.h"

namespace mts
{
    class WindowResizeEvent : public Event
    {
       public:
        WindowResizeEvent(uint32_t width, uint32_t height) : m_Width(width), m_Height(height) {}

        uint32_t GetWidth() const { return m_Width; }
        uint32_t GetHeight() const { return m_Height; }

        std::string ToString() const override
        {
            std::stringstream ss;
            ss << "WindowResizeEvent: " << m_Width << ", " << m_Height;
            return ss.str();
        }

        MTS_EVENT_CLASS_TYPE(WindowResize)
        MTS_EVENT_CLASS_CATEGORY(EventCategoryWindow)

       private:
        uint32_t m_Width, m_Height;
    };

    class WindowCloseEvent : public Event
    {
       public:
        WindowCloseEvent() = default;

        MTS_EVENT_CLASS_TYPE(WindowClose)
        MTS_EVENT_CLASS_CATEGORY(EventCategoryWindow)
    };

    class WindowFocusEvent : public Event
    {
       public:
        WindowFocusEvent() = default;

        MTS_EVENT_CLASS_TYPE(WindowFocus)
        MTS_EVENT_CLASS_CATEGORY(EventCategoryWindow)
    };

    class WindowLostFocusEvent : public Event
    {
       public:
        WindowLostFocusEvent() = default;

        MTS_EVENT_CLASS_TYPE(WindowLostFocus)
        MTS_EVENT_CLASS_CATEGORY(EventCategoryWindow)
    };

}  // namespace mts