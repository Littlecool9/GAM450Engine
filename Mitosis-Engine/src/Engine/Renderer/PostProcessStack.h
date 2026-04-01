/**
 * @file PostProcessStack.h
 * @brief Manages a chain of post-processing effects with ping-pong FBOs
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#pragma once

#include <memory>
#include <vector>
#include "Framebuffer.h"
#include "PostProcessEffect.h"

namespace mts
{
    class Shader;

    class PostProcessStack
    {
       public:
        PostProcessStack() = default;
        ~PostProcessStack();

        PostProcessStack(const PostProcessStack&) = delete;
        PostProcessStack& operator=(const PostProcessStack&) = delete;

        void Shutdown();

        // Returns true if there is at least one enabled effect.
        bool HasActiveEffects() const;

        // Bind the scene framebuffer. All subsequent rendering goes here.
        // Lazily creates / resizes FBOs to match the given viewport size.
        void BindSceneTarget(int viewportW, int viewportH);

        // Execute all enabled effects, blit final result to the default framebuffer,
        // and restore the viewport.
        void Execute(int viewportX, int viewportY, int viewportW, int viewportH);

        // ----- effect management -----
        void AddEffect(std::unique_ptr<PostProcessEffect> effect);
        void RemoveEffect(int index);
        void MoveEffect(int from, int to);  // Adjust the order of effects in the stack
        const std::vector<std::unique_ptr<PostProcessEffect>>& GetEffects() const;
        std::vector<std::unique_ptr<PostProcessEffect>>& GetEffects();

        // ----- serialization -----
        void Deserialize(JsonValue const& json);
        void Serialize(JsonDocument& doc) const;

       private:
        Framebuffer mFBO[2];        // Ping-pong pair
        Framebuffer mPrevFrameFBO;  // Stores previous frame for motion blur
        bool mHasPrevFrame = false;

        GLuint mQuadVAO = 0;
        GLuint mQuadVBO = 0;

        Shader* mPassthroughShader = nullptr;

        std::vector<std::unique_ptr<PostProcessEffect>> mEffects;

        void EnsureFBOs(int w, int h);
        void EnsureFullScreenQuad();
        void DrawFullScreenQuad();
        void BlitToScreen(GLuint texture, int vpX, int vpY, int vpW, int vpH);
    };

}  // namespace mts
