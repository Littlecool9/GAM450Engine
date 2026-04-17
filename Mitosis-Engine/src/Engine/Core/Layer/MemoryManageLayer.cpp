/**
 * @file MemoryManageLayer.cpp
 * @author sumin.park
 * @brief  layer for memory management
 *
 * @copyright Copyright (c) 2026 DigiPen (USA) Corporation
 *
 */

#include "pch.h"
#include "MemoryManageLayer.h"

namespace mts
{
    MemoryManageLayer::MemoryManageLayer() : Layer("MemoryManageLayer")
    {
    }

    void MemoryManageLayer::OnBeginFrame()
    {
        MemoryManager::BeginFrame();
    }
}  // namespace mts