/**
 * @file UIEvent.h
 * @author Dong Ta
 * @brief  Message Broadcasting system, current called UIEvent but name pending
 * @version 0.1
 * @date 1/15/2026 6:55:28 AM
 *
 * @copyright Copyright (c) 2025 - Mitosis Engine
 *
 */
#pragma once


#include "Engine/Events/Event.h"
#include <unordered_map>


//TODO: Potential combine this with parent event system or change the name from ui to broadcast/messaging
namespace mts
{
    class ButtonClickedEvent;

    using EventID = uint32_t;

    constexpr EventID HashAction(std::string_view str)
    {
        uint32_t hash = 2166136261u;  // FNV-1a
        for (char c : str)
        {
            hash ^= static_cast<uint8_t>(c);
            hash *= 16777619u;
        }
        return hash;
    }
    constexpr EventID operator"" _aid(const char* str, size_t len)
    {
        return HashAction({str, len});
    }
    class UIEvent
    {
       public:
        static void Init();      
        static void BeginFrame(); 

        static void OnEvent(Event& e);

        // use this to check if the action was clicked this frame but DONT CLEAR THE STATE, for broadcast
        static bool WasClicked(const std::string& action);
        static bool WasClicked(EventID id);
        // use this to check and clear the clicked state, will return true if the action was clicked this frame
        static bool ConsumeClicked(const std::string& action); 
        static bool ConsumeClicked(EventID id);
        // get the last clicked action, for debugging
        static EventID GetLastClicked();
        static void Prune(uint32_t maxAge);
       private:
        static bool OnButtonClicked(ButtonClickedEvent& e);

       private:
        static std::unordered_map<EventID, uint32_t> ClickedFrame;
        static std::unordered_map<EventID, uint32_t> ConsumedFrame;
        static EventID LastClicked;
        static uint32_t Frame;
    };
}  // namespace mts