#pragma once
#include "SceneSystem/Scene.h"
#include <glad/glad.h>
#include <glm/mat4x4.hpp>

namespace mts
{
    inline constexpr float PixelsPerUnit = 80.0f;

    class Mesh;
    class Texture;
    class Shader;
    class PostProcessStack;

    struct Viewport
    {
        int x = 0, y = 0, w = 0, h = 0;
        bool valid = false;
    };

    enum class AspectMode
    {
        Free,
        R16x9,
        R4x3,
        R21x9,
        Custom
    };

    struct RenderCamera
    {
        glm::vec3 position{0.0f, 0.0f, 0.0f};
        glm::mat4 view{1.0f};
        glm::mat4 proj{1.0f};
        glm::mat4 viewProj{1.0f};
        bool valid = false;  // so SpriteRenderer can fallback if no camera exists
    };

    struct RenderCommand
    {
        // The RenderCommand struct encapsulates all the information needed to render a single
        // sprite or tile. Set if applicable:

        // === Asset ref ===
        Shader* shader = nullptr;
        Texture* texture = nullptr;

        // === Geometry Data ===
        Mesh* mesh = nullptr;    // for SpriteRenderer
        GLuint vao = 0;          // custom VAO
        GLsizei indexCount = 0;  // custom EBO

        // === Transformation ===
        glm::mat4 model{1.0f};

        // === Texture ===
        glm::vec2 uvOffset{0.0f};
        glm::vec3 tintColor{1.0f};
        float alpha = 1.0f;

        // === Render Control ===
        float sortKey = 0.0f;          // Key for sorting render commands (e.g., based on depth)
        bool hasAlpha = false;         // turn on blending if true
        float parallaxFactorX = 1.0f;  // optional parallax factor
        bool isUI = false;  // render in screen-space (HUD/UI); drawn last, ignores camera
    };

    class Renderer
    {
       public:
        // Convert viewport-local coordinates (pixels, Y from BOTTOM) to world units.
        static glm::vec2 ScreenToWorld(glm::vec2 screenPos);
        static glm::vec2 ScreenToWorld(glm::vec3 screenPos);
        // Convert world coordinates (in units) to screen coordinates (in pixels)
        static glm::vec2 WorldToScreen(glm::vec2 worldPos);
        static glm::vec2 WorldToScreen(glm::vec3 worldPos);

        // Returns the current mouse position in world space.
        // Handles viewport offset and Y-flip internally — safe to call from any component.
        static glm::vec2 MouseToWorld();
        // Returns the current mouse position in normalized UI space.
        // (0,0) = top-left of game viewport, (1,1) = bottom-right.
        // Matches the coordinate system used by UITransform.
        static glm::vec2 MouseToUISpace();
        // Get the info of viewport in screen scale, unit is pixel
        // Returns: left, right, screen width, screen height
        static glm::vec4 GetViewportInfo();

        static void SetActiveCamera(const RenderCamera& cam) { ActiveCamera = cam; }
        static const RenderCamera& GetActiveCamera() { return ActiveCamera; }

        // Set the aspect ratio constraint applied when computing the game viewport.
        static void SetAspectMode(AspectMode mode, float x = 16.0f, float y = 9.0f);
        static AspectMode GetAspectMode();
        static float GetTargetAspect();  // returns 0 when Free
        static float GetCustomAspectX();
        static float GetCustomAspectY();

        // Call once on window creation and on every WindowResizeEvent so that
        // MouseToWorld / MouseToUISpace can bridge GLFW-top-relative mouse coords
        // to the OpenGL-bottom-relative game viewport stored in targetViewport.
        static void SetWindowSize(int w, int h);
        static int  GetWindowWidth()  { return s_windowW; }
        static int  GetWindowHeight() { return s_windowH; }

        // UI reference resolution. Call SetUIReferenceSize once at app init.
        // GetUIScaleFactor returns (vpW_units / refW_units, vpH_units / refH_units).
        static void      SetUIReferenceSize(int w, int h);
        static glm::vec2 GetUIScaleFactor();

        // Provide the available screen region; Renderer applies aspect constraint and stores
        // result.
        static void SetAvailableViewport(int x, int y, int w, int h);
        static void ClearGameViewport();
        static Viewport GetGameViewport();

        // --- Post-processing ---
        static void SetPostProcessStack(PostProcessStack* stack);
        static PostProcessStack* GetPostProcessStack();

        // Frame-based rendering API: collect, sort, and draw render commands
        static void BeginFrame();

        static void Submit(const RenderCommand& cmd);

        // Bind the scene FBO if post-processing is active. Call before FlushScene().
        static void BeginSceneRender();

        // Sort and draw opaque + transparent commands (scene only, no UI).
        static void FlushScene();

        // Execute post-processing and blit to default framebuffer.
        // Call after all scene/debug rendering is done.
        static void EndSceneRender();

        // Sort and draw screen-space UI commands (to default FB).
        static void FlushUI();

        // Convenience: BeginSceneRender + FlushScene + EndSceneRender + FlushUI.
        static void Flush();

        static void EndFrame();

       private:
        static RenderCamera ActiveCamera;
        static PostProcessStack* s_postProcessStack;
        static Viewport targetViewport;
        static int s_windowW;
        static int s_windowH;
        static AspectMode s_aspectMode;
        static float s_customX;
        static float s_customY;
        static int s_uiRefW;
        static int s_uiRefH;

        // Render Command queue
        static std::vector<RenderCommand> OpaqueCommands;
        static std::vector<RenderCommand> TransparentCommands;
        static std::vector<RenderCommand> ScreenSpaceCommands;

        static GLboolean blendEnabled;
        static GLboolean depthTestEnabled;
        static GLboolean depthMaskEnabled;

        // Internal rendering functions
        static void ExecuteCommand(const RenderCommand& cmd);
        static glm::mat4 ComputeViewProjWithParallax(float parallaxFactorX);
    };
}  // namespace mts