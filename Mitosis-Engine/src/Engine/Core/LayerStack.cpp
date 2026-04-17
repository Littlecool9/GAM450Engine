/**
 * @file LayerStack.cpp
 * @author sumin.park
 * @brief  stack to control system layers
 *
 * @copyright Copyright (c) 2026 DigiPen (USA) Corporation
 *
 */

#include "pch.h"
#include "LayerStack.h"

namespace mts
{
    LayerStack::~LayerStack()
    {
        Clear();
    }

    void LayerStack::PushLayer(std::unique_ptr<Layer> layer)
    {
        layer->OnAttach();
        mLayers.emplace(mLayers.begin() + static_cast<long>(mLayerInsertIdx), std::move(layer));
        mLayerInsertIdx++;
    }

    void LayerStack::PushOverlay(std::unique_ptr<Layer> overlay)
    {
        overlay->OnAttach();
        mLayers.emplace_back(std::move(overlay));
    }

    void LayerStack::PopLayer(Layer* layer)
    {
        auto it =
            std::find_if(mLayers.begin(),
                         mLayers.begin() + static_cast<long>(mLayerInsertIdx),
                         [layer](const std::unique_ptr<Layer>& ptr) { return ptr.get() == layer; });

        if (it != mLayers.begin() + static_cast<long>(mLayerInsertIdx))
        {
            (*it)->OnDetach();
            mLayers.erase(it);
            mLayerInsertIdx--;
        }
    }

    void LayerStack::PopOverlay(Layer* overlay)
    {
        auto it = std::find_if(mLayers.begin() + static_cast<long>(mLayerInsertIdx),
                               mLayers.end(),
                               [overlay](const std::unique_ptr<Layer>& ptr)
                               { return ptr.get() == overlay; });

        if (it != mLayers.end())
        {
            (*it)->OnDetach();
            mLayers.erase(it);
        }
    }

    void LayerStack::Clear()
    {
        for (auto it = rbegin(); it != rend(); ++it)
        {
            if ((*it))
                (*it)->OnDetach();
        }
        mLayers.clear();
        mLayerInsertIdx = 0;
    }

    Layer* LayerStack::FindLayerByName(const std::string& name)
    {
        for (auto& layer : mLayers)
        {
            if (layer->GetName() == name)
                return layer.get();
        }
        return nullptr;
    }
}  // namespace mts