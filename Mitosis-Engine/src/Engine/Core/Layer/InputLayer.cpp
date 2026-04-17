/**
 * @file InputLayer.cpp
 * @author sumin.park
 * @brief  layer for input system
 *
 * @copyright Copyright (c) 2026 DigiPen (USA) Corporation
 *
 */

#include "pch.h"
#include "InputLayer.h"

#include "Engine/Input/Input.h"

namespace mts
{
    InputLayer::InputLayer() : Layer("InputLayer")
    {
    }

    void InputLayer::OnAttach()
    {
        Input::Init();
        Layer::OnAttach();
    }

    void InputLayer::OnDetach()
    {
        Input::Shutdown();
        Layer::OnDetach();
    }

    void InputLayer::OnEvent(Event& e)
    {
        Input::OnEvent(e);
    }

    void InputLayer::OnBeginFrame()
    {
        Input::BeginFrame();
    }
}  // namespace mts