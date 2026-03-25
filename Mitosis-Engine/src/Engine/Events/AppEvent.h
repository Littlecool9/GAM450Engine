/**
 * @file AppEvent.h
 * @author sumin.park
 * @brief  event from apps
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#pragma once
#include "Event.h"

namespace mts
{
    class AppQuitRequestEvent : public Event
    {
       public:
        AppQuitRequestEvent() = default;
        MTS_EVENT_CLASS_TYPE(AppQuitRequest)
        MTS_EVENT_CLASS_CATEGORY(EventCategoryApplication)
    };

}  // namespace mts