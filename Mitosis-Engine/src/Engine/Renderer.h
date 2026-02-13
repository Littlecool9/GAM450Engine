
#pragma once
#include "SceneSystem/Scene.h"
#include <glm/mat4x4.hpp>

namespace mts
{
    inline constexpr float PixelsPerUnit = 80.0f;

    struct RenderCamera
    {
        glm::vec3 position{0.0f, 0.0f, 0.0f};
        glm::mat4 view{1.0f};
        glm::mat4 proj{1.0f};
        glm::mat4 viewProj{1.0f};
        bool valid = false;  // so SpriteRenderer can fallback if no camera exists
    };

    class Renderer
    {
       public:
        // Convert screen coordinates (in pixels) to world coordinates (in units)
        // The screen coordinates should be in the Screen Space, where (0,0) is the left-top of the
        // screen, and y increases downwards.
        static glm::vec2 ScreenToWorld(glm::vec2 screenPos);
        static glm::vec2 ScreenToWorld(glm::vec3 screenPos);
        // Convert world coordinates (in units) to screen coordinates (in pixels)
        static glm::vec2 WorldToScreen(glm::vec2 worldPos);
        static glm::vec2 WorldToScreen(glm::vec3 worldPos);
        // Get the info of viewport in screen scale, unit is pixel
        // Returns: left, right, screen width, screen height
        static glm::vec4 GetViewportInfo();

        static void SetActiveCamera(const RenderCamera& cam) { ActiveCamera = cam; }
        static const RenderCamera& GetActiveCamera() { return ActiveCamera; }

        // Sort sprites with SpriteRenderer components and draw them to screen
        static void DrawSprites(Scene* scene);

       private:
        static RenderCamera ActiveCamera;
    };
}  // namespace mts