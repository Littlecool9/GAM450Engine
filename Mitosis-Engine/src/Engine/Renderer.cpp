#include "pch.h"
#include "Renderer.h"
#include "EntitySystem/GameObject/GameObject.h"
#include "EntitySystem/Component/Transform.h"
#include "EntitySystem/Component/SpriteRenderer.h"
#include "EntitySystem/Component/Particles/ParticleEmitter.h"
#include "Renderer/PostProcessStack.h"
#include "Input/Input.h"

#include <algorithm>
#include <glm/ext/matrix_clip_space.hpp>

namespace mts
{
    int Renderer::s_windowW = 0;
    int Renderer::s_windowH = 0;
    RenderCamera Renderer::ActiveCamera{};
    PostProcessStack* Renderer::s_postProcessStack = nullptr;
    Viewport Renderer::targetViewport{};
    AspectMode Renderer::s_aspectMode = AspectMode::Free;
    float Renderer::s_customX = 16.0f;
    float Renderer::s_customY = 9.0f;
    std::vector<RenderCommand> Renderer::OpaqueCommands;
    std::vector<RenderCommand> Renderer::TransparentCommands;
    std::vector<RenderCommand> Renderer::ScreenSpaceCommands;
    GLboolean Renderer::blendEnabled = GL_FALSE;
    GLboolean Renderer::depthTestEnabled = GL_FALSE;
    GLboolean Renderer::depthMaskEnabled = GL_FALSE;

    glm::vec2 Renderer::ScreenToWorld(glm::vec2 screenPos)
    {
        glm::vec4 vp = GetViewportInfo();
        int width = static_cast<int>(vp.z);
        int height = static_cast<int>(vp.w);

        // 1. Screen -> NDC
        float x = (2.0f * screenPos.x) / width - 1.0f;
        float y = 1.0f - (2.0f * (height - screenPos.y)) / height;

        // 2. NDC -> World
        glm::mat4 invVP = glm::inverse(ActiveCamera.viewProj);
        glm::vec4 world = invVP * glm::vec4(x, y, 0.0f, 1.0f);
        return glm::vec2(world.x, world.y);
    }

    glm::vec2 Renderer::ScreenToWorld(glm::vec3 screenPos)
    {
        return ScreenToWorld(glm::vec2(screenPos.x, screenPos.y));
    }

    glm::vec2 Renderer::MouseToWorld()
    {
        Viewport gv    = GetGameViewport();
        float vpLocalX = Input::GetMouseX() - static_cast<float>(gv.x);
        // gv.y is OpenGL bottom-relative; s_windowH bridges to GLFW top-relative mouseY
        float flippedY = static_cast<float>(s_windowH) - Input::GetMouseY() - static_cast<float>(gv.y);
        return ScreenToWorld({vpLocalX, flippedY});
    }

    glm::vec2 Renderer::MouseToUISpace()
    {
        Viewport gv   = GetGameViewport();
        // gv.y is OpenGL bottom-relative; use s_windowH to recover the viewport's top in GLFW space
        float gvH     = gv.valid ? static_cast<float>(gv.h) : static_cast<float>(s_windowH);
        float vpLocalX = Input::GetMouseX() - static_cast<float>(gv.x);
        float vpLocalY = Input::GetMouseY() - (static_cast<float>(s_windowH) - static_cast<float>(gv.y) - gvH);
        return {vpLocalX / PixelsPerUnit, vpLocalY / PixelsPerUnit};
    }

    glm::vec2 Renderer::WorldToScreen(glm::vec3 worldPos)
    {
        glm::vec2 screenPos = WorldToScreen(glm::vec2(worldPos.x, worldPos.y));
        return screenPos;
    }

    // Returns (x, y, width, height) of the viewport in pixels
    // The returned coordinates are in converted Screen Space, where (0,0) is the middle of the
    // screen
    glm::vec4 Renderer::GetViewportInfo()
    {
        // Prefer the cached game viewport (set by SetViewport / editor layout) to avoid a
        // glGetIntegerv GPU pipeline stall. Fall back to GL query only in standalone mode.
        Viewport gv = GetGameViewport();
        if (gv.valid)
            return glm::vec4(gv.x, -gv.y, gv.w, gv.h);

        GLint vp[4];
        glGetIntegerv(GL_VIEWPORT, vp);
        return glm::vec4(vp[0], -vp[1], (float) vp[2], (float) vp[3]);
    }

    glm::vec2 Renderer::WorldToScreen(glm::vec2 worldPos)
    {
        // 1. World -> NDC
        glm::vec4 ndc = ActiveCamera.viewProj * glm::vec4(worldPos, 0.0f, 1.0f);

        // 2. NDC -> Screen
        glm::vec4 vp = GetViewportInfo();
        int width = static_cast<int>(vp.z);
        int height = static_cast<int>(vp.w);
        float x = (ndc.x / ndc.w + 1.0f) * 0.5f * width;
        float y = (1.0f - ndc.y / ndc.w) * 0.5f * height;  // flip y
        return glm::vec2(x, y);
    }

    void Renderer::SetWindowSize(int w, int h)
    {
        s_windowW = w;
        s_windowH = h;
    }

    void Renderer::SetAspectMode(AspectMode mode, float x, float y)
    {
        s_aspectMode = mode;
        if (mode == AspectMode::Custom)
        {
            s_customX = x;
            s_customY = y;
        }
    }

    AspectMode Renderer::GetAspectMode()
    {
        return s_aspectMode;
    }

    float Renderer::GetCustomAspectX()
    {
        return s_customX;
    }
    float Renderer::GetCustomAspectY()
    {
        return s_customY;
    }

    float Renderer::GetTargetAspect()
    {
        switch (s_aspectMode)
        {
            case AspectMode::R16x9:
                return 16.0f / 9.0f;
            case AspectMode::R4x3:
                return 4.0f / 3.0f;
            case AspectMode::R21x9:
                return 21.0f / 9.0f;
            case AspectMode::Custom:
                return s_customX / s_customY;
            default:
                return 0.0f;
        }
    }

    void Renderer::SetAvailableViewport(int x, int y, int w, int h)
    {
        const float targetAspect = GetTargetAspect();
        if (targetAspect <= 0.0f)
        {
            targetViewport = {x, y, w, h, true};
            return;
        }

        // the size of available area
        const float nodeAspect = static_cast<float>(w) / static_cast<float>(h);
        int vpW, vpH, vpX, vpY;
        //
        if (nodeAspect > targetAspect)
        {
            // node is wider (pillarbox)
            // fit height, center horizontally
            vpH = h;
            vpW = static_cast<int>(h * targetAspect);
            vpX = x + (w - vpW) / 2;
            vpY = y;
        }
        else
        {
            // node is taller (letterbox)
            // fit width, center vertically
            vpW = w;
            vpH = static_cast<int>(w / targetAspect);
            vpX = x;
            vpY = y + (h - vpH) / 2;
        }
        targetViewport = {vpX, vpY, vpW, vpH, true};
    }

    void Renderer::ClearGameViewport()
    {
        targetViewport.valid = false;
    }

    Viewport Renderer::GetGameViewport()
    {
        return targetViewport;
    }

    void Renderer::BeginFrame()
    {
        OpaqueCommands.clear();
        TransparentCommands.clear();
        ScreenSpaceCommands.clear();
    }

    void Renderer::SetPostProcessStack(PostProcessStack* stack)
    {
        s_postProcessStack = stack;
    }

    PostProcessStack* Renderer::GetPostProcessStack()
    {
        return s_postProcessStack;
    }

    void Renderer::Submit(const RenderCommand& cmd)
    {
        if (cmd.isUI)
            ScreenSpaceCommands.push_back(cmd);
        else if (cmd.hasAlpha)
            TransparentCommands.push_back(cmd);
        else
            OpaqueCommands.push_back(cmd);
    }

    void Renderer::BeginSceneRender()
    {
        PostProcessStack* pp = s_postProcessStack;
        if (pp && pp->HasActiveEffects())
        {
            Viewport gv = GetGameViewport();
            int w = gv.valid ? gv.w : s_windowW;
            int h = gv.valid ? gv.h : s_windowH;
            if (w > 0 && h > 0)
                pp->BindSceneTarget(w, h);
        }
    }

    void Renderer::FlushScene()
    {
        // Sort opaque front-to-back, transparent back-to-front
        std::sort(OpaqueCommands.begin(),
                  OpaqueCommands.end(),
                  [](const RenderCommand& a, const RenderCommand& b)
                  { return a.sortKey > b.sortKey; });
        std::sort(TransparentCommands.begin(),
                  TransparentCommands.end(),
                  [](const RenderCommand& a, const RenderCommand& b)
                  { return a.sortKey < b.sortKey; });

        // Store GL states for restoring
        blendEnabled = glIsEnabled(GL_BLEND);
        depthTestEnabled = glIsEnabled(GL_DEPTH_TEST);
        depthMaskEnabled = GL_TRUE;

        // Render opaque objects
        glDisable(GL_BLEND);
        glEnable(GL_DEPTH_TEST);
        glDepthMask(GL_TRUE);
        for (const auto& cmd : OpaqueCommands)
            ExecuteCommand(cmd);

        // Render transparent objects
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDisable(GL_DEPTH_TEST);
        glDepthMask(GL_FALSE);
        for (const auto& cmd : TransparentCommands)
            ExecuteCommand(cmd);
    }

    void Renderer::EndSceneRender()
    {
        PostProcessStack* pp = s_postProcessStack;
        if (pp && pp->HasActiveEffects())
        {
            Viewport gv = GetGameViewport();
            int vpX = gv.valid ? gv.x : 0;
            int vpY = gv.valid ? gv.y : 0;
            int vpW = gv.valid ? gv.w : s_windowW;
            int vpH = gv.valid ? gv.h : s_windowH;
            if (vpW > 0 && vpH > 0)
                pp->Execute(vpX, vpY, vpW, vpH);
        }
    }

    void Renderer::FlushUI()
    {
        // Render screen-space (HUD) objects last, on top of everything
        std::sort(ScreenSpaceCommands.begin(),
                  ScreenSpaceCommands.end(),
                  [](const RenderCommand& a, const RenderCommand& b)
                  { return a.sortKey < b.sortKey; });
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDisable(GL_DEPTH_TEST);
        for (const auto& cmd : ScreenSpaceCommands)
            ExecuteCommand(cmd);
    }

    void Renderer::Flush()
    {
        BeginSceneRender();
        FlushScene();
        EndSceneRender();
        FlushUI();
    }

    void Renderer::EndFrame()
    {
        // Resetting GL states
        blendEnabled ? glEnable(GL_BLEND) : glDisable(GL_BLEND);
        depthTestEnabled ? glEnable(GL_DEPTH_TEST) : glDisable(GL_DEPTH_TEST);
        glDepthMask(depthMaskEnabled);
        glDisable(GL_SCISSOR_TEST);
    }

    void Renderer::ExecuteCommand(const RenderCommand& cmd)
    {
        if (!cmd.shader)
            return;

        cmd.shader->UseProgram();

        // Compute view projection matrix
        glm::mat4 viewProj;

        // calculate separate viewport for the hud
        if (cmd.isUI)
        {
            Viewport gv = GetGameViewport();
            float w, h;
            if (gv.valid)
            {
                w = gv.w / PixelsPerUnit;
                h = gv.h / PixelsPerUnit;
            }
            else
            {
                GLint vp[4];
                glGetIntegerv(GL_VIEWPORT, vp);
                w = vp[2] / PixelsPerUnit;
                h = vp[3] / PixelsPerUnit;
            }
            // (0,0) = top-left, x right, y down, 1 unit = PixelsPerUnit px
            viewProj = glm::ortho(0.0f, w, h, 0.0f, -1.0f, 1.0f);
        }
        else if (cmd.parallaxFactorX != 1.0f)
            viewProj = ComputeViewProjWithParallax(cmd.parallaxFactorX);
        else
            viewProj = ActiveCamera.viewProj;

        // Set Uniform
        // UI: pre-flip the quad's local Y to counteract the Y-down ortho, keeping UVs correct
        glm::mat4 model = cmd.isUI
            ? cmd.model * glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, -1.0f, 1.0f))
            : cmd.model;
        cmd.shader->SetMatrix4("model", model);
        cmd.shader->SetMatrix4("viewProj", viewProj);
        cmd.shader->SetVector2f("uvOffset", cmd.uvOffset);
        cmd.shader->SetVector3f("tintColor", cmd.tintColor);
        cmd.shader->SetFloat("alpha", cmd.alpha);

        // Bind Texture
        if (cmd.texture)
            cmd.texture->BindTexture();

        // Render
        if (cmd.mesh)
        {
            cmd.mesh->Draw();
        }
        else if (cmd.vao && cmd.indexCount > 0)
        {
            glBindVertexArray(cmd.vao);
            glDrawElements(GL_TRIANGLES, cmd.indexCount, GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);
        }

        if (cmd.texture)
            cmd.texture->UnbindTexture();
    }

    glm::mat4 Renderer::ComputeViewProjWithParallax(float parallaxFactorX)
    {
        glm::mat4 view = ActiveCamera.view;
        glm::mat4 proj = ActiveCamera.proj;
        float offsetFactorX = 1.0f - parallaxFactorX;
        glm::vec3 parallaxOffset = ActiveCamera.position * glm::vec3(offsetFactorX, 0.0f, 0.0f);
        glm::mat4 parallaxView = glm::translate(view, parallaxOffset);
        return proj * parallaxView;
    }
}  // namespace mts