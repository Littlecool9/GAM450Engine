/**
 * @file SceneLayer.h
 * @author sumin.park
 * @brief  Layer for scene system
 *
 * @copyright Copyright (c) 2026 DigiPen (USA) Corporation
 *
 */

#pragma once
#include "Engine/Core/Layer.h"
#include "Engine/Core/Editor/EditorContext.h"

namespace mts
{
    class SceneLayer : public Layer
    {
       public:
        explicit SceneLayer(const std::string& scenePath, const std::string& registryPath, EditorContext& ctx);
        void OnAttach() override;
        void OnDetach() override;

        void OnUpdate(float dt) override;
        void OnLateUpdate(float dt) override;
        void OnFixedUpdate(float fixedDt) override;

        void OnRender() override;
        void OnDebugRender() override;

        void OnEndFrame() override;

        void OnEnterPlayMode() override;
        void OnExitPlayMode() override;

       private:
        std::string mInitialScenePath;
        std::string mRegistryPath;
        EditorContext& mCtx;
    };

}  // namespace mts