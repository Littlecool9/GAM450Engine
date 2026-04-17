/**
 * @file AudioLayer.h
 * @author sumin.park
 * @brief  Layer for audio system
 *
 * @copyright Copyright (c) 2026 DigiPen (USA) Corporation
 *
 */

#pragma once
#include "Engine/Core/Layer.h"

namespace mts
{
    class ConductorSystem;
    class AudioManager;
    class AudioLayer : public Layer
    {
       public:
        explicit AudioLayer();

        void OnAttach() override;
        void OnDetach() override;

        void OnLateUpdate(float dt) override;

        void OnExitPlayMode() override;

       private:
        std::unique_ptr<AudioManager> mAudio;
        std::unique_ptr<ConductorSystem> mConductor;
    };
}  // namespace mts