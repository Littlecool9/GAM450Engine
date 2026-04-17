/**
 * @file ResourceLayer.cpp
 * @author sumin.park
 * @brief  layer for resource manager
 *
 * @copyright Copyright (c) 2026 DigiPen (USA) Corporation
 *
 */

#include "pch.h"
#include "ResourceLayer.h"
#include "Engine/Resource/ResourceManager.h"

namespace mts
{
    ResourceLayer::ResourceLayer() : Layer("ResourceLayer")
    {
    }

    void ResourceLayer::OnAttach()
    {
        ResourceManager::Init();
        Layer::OnAttach();
    }

    void ResourceLayer::OnDetach()
    {
        ResourceManager::Shutdown();
        Layer::OnDetach();
    }
}  // namespace mts