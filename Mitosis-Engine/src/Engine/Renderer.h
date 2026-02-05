
#pragma once
#include "SceneSystem/Scene.h"
#include <glm/mat4x4.hpp>

namespace mts
{
    struct RenderCamera
    {
        glm::mat4 view{1.0f};
        glm::mat4 proj{1.0f};
        glm::mat4 viewProj{1.0f};
        bool valid = false;  // so MeshRenderer can fallback if no camera exists
    };

    class Renderer
    {
       public:
        static void SetActiveCamera(const RenderCamera& cam) { ActiveCamera = cam; }
        static const RenderCamera& GetActiveCamera() { return ActiveCamera; }

        // Sort sprites with MeshRenderer components and draw them to screen
        static void DrawSprites(Scene* scene);

       private:
        static RenderCamera ActiveCamera;
    };
}  // namespace mts