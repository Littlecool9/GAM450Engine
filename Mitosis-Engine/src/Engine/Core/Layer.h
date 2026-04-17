/**
 * @file Layer.h
 * @author sumin.park
 * @brief  encapsulation for engine system
 *
 * @copyright Copyright (c) 2026 DigiPen (USA) Corporation
 *
 */

#pragma once
#include "Engine/Logging/EngineLog.h"
namespace mts
{
    class Event;
    class Layer
    {
       public:
        explicit Layer(std::string name = "Layer") : mName(std::move(name)) {}
        virtual ~Layer() = default;

        virtual void OnAttach() { LOG_INFO("Attach {}", mName); }
        virtual void OnDetach() { LOG_INFO("Detach {}", mName); }

        virtual void OnBeginFrame() {}
        virtual void OnUpdate(float dt) {}
        virtual void OnLateUpdate(float dt) {}
        virtual void OnFixedUpdate(float fixedDt) {}

        virtual void OnRender() {}
        virtual void OnDebugRender() {}
        virtual void OnImGuiRender(float dt) {}

        virtual void OnEnterPlayMode() {}
        virtual void OnExitPlayMode() {}

        virtual void OnEndFrame() {}

        virtual void OnEvent(Event& e) {}

        const std::string& GetName() const { return mName; }

        void SetEnabled(bool enabled) { mEnabled = enabled; }
        bool IsEnabled() const { return mEnabled; }

       private:
        std::string mName;
        bool mEnabled = true;
    };

}  // namespace mts