/**
 * @file Event.h
 * @author rahul
 * @brief // Core event base types and dispatcher for engine-wide event handling.
 *
 * @copyright Copyright (c) 2025 - RM Engine
 *
 */

#pragma once
#include <functional>

namespace rm
{
    enum class EventType
    {
        None = 0,

        // Window
        WindowClose,
        WindowResize,
        WindowFocus,
        WindowLostFocus,

        // Keyboard
        KeyPressed,
        KeyReleased,

        // Mouse
        MouseButtonPressed,
        MouseButtonReleased,
        MouseMoved,
        MouseScrolled
    };

    enum EventCategory
    {
        None = 0,
        EventCategoryApplication = 1 << 0,
        EventCategoryInput = 1 << 1,
        EventCategoryKeyboard = 1 << 2,
        EventCategoryMouse = 1 << 3,
        EventCategoryMouseButton = 1 << 4,
        EventCategoryWindow = 1 << 5
    };

#define RM_EVENT_CLASS_TYPE(type)                                               \
    static EventType GetStaticType() { return EventType::type; }                \
    virtual EventType GetEventType() const override { return GetStaticType(); } \
    virtual const char *GetName() const override { return #type; }

#define RM_EVENT_CLASS_CATEGORY(categoryFlags) \
    virtual int GetCategoryFlags() const override { return categoryFlags; }

    class Event
    {
    public:
        virtual ~Event() = default;

        bool Handled = false;

        virtual EventType GetEventType() const = 0;
        virtual const char *GetName() const = 0;
        virtual int GetCategoryFlags() const = 0;
        virtual std::string ToString() const { return GetName(); }

        bool IsInCategory(EventCategory category) const
        {
            return GetCategoryFlags() & category;
        }
    };

    class EventDispatcher
    {
    public:
        explicit EventDispatcher(Event &e) : m_Event(e) {}

        template <typename T, typename F>
        bool Dispatch(const F &func)
        {
            if (m_Event.GetEventType() == T::GetStaticType())
            {
                m_Event.Handled |= func(static_cast<T &>(m_Event));
                return true;
            }
            return false;
        }

    private:
        Event &m_Event;
    };

    using EventCallbackFn = std::function<void(Event &)>;

} // rm namespace