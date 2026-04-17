/**
 * @file LayerStack.h
 * @author sumin.park
 * @brief  stack to control system layers
 *
 * @copyright Copyright (c) 2026 DigiPen (USA) Corporation
 *
 */

#pragma once
#include "Layer.h"
namespace mts
{
    class LayerStack
    {
       public:
        LayerStack() = default;
        ~LayerStack();

        void PushLayer(std::unique_ptr<Layer> layer);
        void PushOverlay(std::unique_ptr<Layer> overlay);

        void PopLayer(Layer* layer);
        void PopOverlay(Layer* overlay);

        void Clear();

        auto begin() { return mLayers.begin(); }
        auto end() { return mLayers.end(); }
        auto rbegin() { return mLayers.rbegin(); }
        auto rend() { return mLayers.rend(); }

        Layer* FindLayerByName(const std::string& name);

       private:
        std::vector<std::unique_ptr<Layer>> mLayers;
        size_t mLayerInsertIdx = 0;
    };

}  // namespace mts