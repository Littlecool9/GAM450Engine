#pragma once
#include "Event.h"

namespace mts
{

    class ButtonClickedEvent : public Event
    {
       public:
        explicit ButtonClickedEvent(std::string action) : m_Action(std::move(action)) {}

        const std::string& GetAction() const { return m_Action; }

        MTS_EVENT_CLASS_TYPE(UIButtonClicked)
        MTS_EVENT_CLASS_CATEGORY(EventCategoryInput | EventCategoryUI)

       private:
        std::string m_Action;
    };

}  // namespace mts
