/**
 * @file LogPanel.h
 * @author sumin.park
 * @brief  log ui
 *
 * @copyright Copyright (c) 2026 DigiPen (USA) Corporation
 *
 */

#pragma once

namespace mts
{
    class Scene;
    struct EditorContext;
}  // namespace mts

namespace mts
{
    class LogPanel
    {
       public:
        LogPanel() = default;
        void Draw(Scene* scene, EditorContext& ctx);

       private:
        bool mAutoScroll = true;
    };
}  // namespace mts
