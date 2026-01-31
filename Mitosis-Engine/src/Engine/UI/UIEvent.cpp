/**
 * @file UIEvent.cpp
 * @author Dong Ta
 * @brief  functionality for UIEvent
 * @version 0.1
 * @date 1/15/2026 6:55:28 AM
 *
 * @copyright Copyright (c) 2025 - Mitosis Engine
 *
 */

#include "pch.h"
#include "UIEvent.h"

#include "Engine/Events/EventButton.h"



namespace mts
{

    std::unordered_map<uint32_t, uint32_t> UIEvent::ClickedFrame{};
    std::unordered_map<uint32_t, uint32_t> UIEvent::ConsumedFrame{};
    uint32_t UIEvent::Frame = 0;
    uint32_t UIEvent::LastClicked = 0;

    void UIEvent::Init()
    {
        LOG_INFO("UIEvent initialized.");
    }

    void UIEvent::BeginFrame()
    {
        ++Frame;

    }

    void UIEvent::OnEvent(Event& e)
    {
        EventDispatcher dispatcher(e);

        dispatcher.Dispatch<ButtonClickedEvent>([](ButtonClickedEvent& ev)
                                                { return UIEvent::OnButtonClicked(ev); });
    }

    bool UIEvent::OnButtonClicked(ButtonClickedEvent& e)
    {
        EventID id = HashAction(e.GetAction());
        ClickedFrame[id] = Frame;
        LastClicked = id;

        return true;
    }

    // use this to check if the action was clicked this frame but DONT CLEAR THE STATE, for broadcast
    bool UIEvent::WasClicked(EventID id)
    {
        auto it = ClickedFrame.find(id);
        return it != ClickedFrame.end() && it->second == Frame;
    }
    bool UIEvent::WasClicked(const std::string& action)
    {
        return WasClicked(HashAction(action));
    }
    // use this to check and clear the clicked state, will return true if the action was clicked
    // this frame
    bool UIEvent::ConsumeClicked(EventID id)
    {
        auto it = ClickedFrame.find(id);
        if (it == ClickedFrame.end() || it->second != Frame)
            return false;

        uint32_t& consumedFrame = ConsumedFrame[id];
        if (consumedFrame == Frame)
            return false;

        consumedFrame = Frame;
        return true;
    }
    bool UIEvent::ConsumeClicked(const std::string& action)
    {
        return ConsumeClicked(HashAction(action));
    }
    // return the last clicked action, for debug

    EventID UIEvent::GetLastClicked()
    {
        return LastClicked;
    }
    void UIEvent::Prune(uint32_t maxAge)
    {
        for (auto it = ClickedFrame.begin(); it != ClickedFrame.end();)
        {
            if (Frame - it->second > maxAge)
                it = ClickedFrame.erase(it);
            else
                ++it;
        }
    }
}  // namespace mts