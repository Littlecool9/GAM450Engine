#include "pch.h"
#include "Renderer.h"
#include "EntitySystem/GameObject/GameObject.h"
#include "EntitySystem/Component/Transform.h"
#include "EntitySystem/Component/SpriteRenderer.h"
#include "EntitySystem/Component/Particles/ParticleEmitter.h"

#include <algorithm>

namespace mts
{
    RenderCamera Renderer::ActiveCamera{};
    Viewport Renderer::targetViewport{};
    std::vector<RenderCommand> Renderer::OpaqueCommands;
    std::vector<RenderCommand> Renderer::TransparentCommands;
    GLboolean Renderer::blendEnabled = GL_FALSE;
    GLboolean Renderer::depthTestEnabled = GL_FALSE;
    GLboolean Renderer::depthMaskEnabled = GL_FALSE;

    glm::vec2 Renderer::ScreenToWorld(glm::vec2 screenPos)
    {
        glm::vec4 vp = GetViewportInfo();
        int width  = static_cast<int>(vp.z);
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
        return glm::vec4(vp[0], -vp[1], (float)vp[2], (float)vp[3]);
    }

    glm::vec2 Renderer::WorldToScreen(glm::vec2 worldPos)
    {
        // 1. World -> NDC
        glm::vec4 ndc = ActiveCamera.viewProj * glm::vec4(worldPos, 0.0f, 1.0f);

        // 2. NDC -> Screen
        glm::vec4 vp = GetViewportInfo();
        int width  = static_cast<int>(vp.z);
        int height = static_cast<int>(vp.w);
        float x = (ndc.x / ndc.w + 1.0f) * 0.5f * width;
        float y = (1.0f - ndc.y / ndc.w) * 0.5f * height;  // flip y
        return glm::vec2(x, y);
    }

    void Renderer::SetViewport(int x, int y, int w, int h)
    {
        targetViewport = {x, y, w, h, true};
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
    }

    void Renderer::Submit(const RenderCommand& cmd)
    {
        if (cmd.hasAlpha)
            TransparentCommands.push_back(cmd);
        else
            OpaqueCommands.push_back(cmd);
    }

    void Renderer::Flush()
    {
        // 1. Sort the commands from front to back for opaque and back to front for transparent
        std::sort(OpaqueCommands.begin(),
                  OpaqueCommands.end(),
                  [](const RenderCommand& a, const RenderCommand& b)
                  { return a.sortKey > b.sortKey; });
        std::sort(TransparentCommands.begin(),
                  TransparentCommands.end(),
                  [](const RenderCommand& a, const RenderCommand& b)
                  { return a.sortKey < b.sortKey; });

        // Store GL states before rendering for for restoring
        blendEnabled = glIsEnabled(GL_BLEND);
        depthTestEnabled = glIsEnabled(GL_DEPTH_TEST);
        depthMaskEnabled = GL_TRUE;

        // 2. Render opaque objects
        glDisable(GL_BLEND);
        glEnable(GL_DEPTH_TEST);
        glDepthMask(GL_TRUE);
        for (const auto& cmd : OpaqueCommands)
            ExecuteCommand(cmd);

        // 3. Render transparent objects
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDisable(GL_DEPTH_TEST);
        glDepthMask(GL_FALSE);

        for (const auto& cmd : TransparentCommands)
            ExecuteCommand(cmd);
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

        // Compute view projection matrix with parallax effect if needed
        glm::mat4 viewProj;
        if (cmd.parallaxFactorX != 1.0f)
            viewProj = ComputeViewProjWithParallax(
                cmd.parallaxFactorX);  // FIXED: Removed duplicate declaration
        else
            viewProj = ActiveCamera.viewProj;

        // Set Uniform
        cmd.shader->SetMatrix4("model", cmd.model);
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