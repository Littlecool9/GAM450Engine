/**
 * @file DebugRenderLayer.cpp
 * @author sumin.park
 * @brief  layer for debug rendering
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#include "pch.h"
#include "DebugRenderLayer.h"

#include "Engine/Renderer.h"
#include "Engine/Core/Editor/DebugDraw.h"

namespace mts
{
    DebugRenderLayer::DebugRenderLayer() : Layer("DebugRenderLayer")
    {
    }

    void DebugRenderLayer::OnAttach()
    {
        debug::DebugDraw::Init();
        Layer::OnAttach();
    }

    void DebugRenderLayer::OnDetach()
    {
        debug::DebugDraw::Shutdown();
        Layer::OnDetach();
    }

    void DebugRenderLayer::OnDebugRender()
    {
        auto camera = Renderer::GetActiveCamera();
        debug::DebugDraw::Draw(camera.viewProj);
    }
}  // namespace mts