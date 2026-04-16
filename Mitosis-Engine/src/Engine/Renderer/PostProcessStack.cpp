/**
 * @file PostProcessStack.cpp
 * @author Jackey Cen
 * @brief Post-processing pipeline execution with ping-pong FBOs
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#include "pch.h"
#include "PostProcessStack.h"

#include <glad/glad.h>
#include "Engine/Resource/ResourceManager.h"
#include "Engine/Resource/Shader.h"
#include "Engine/Serialization/JsonAbstraction.h"

namespace mts
{
    static const char* kPassthroughShader = "Engine/Resources/PP_Passthrough.shader";

    PostProcessStack::~PostProcessStack()
    {
        Shutdown();
    }

    void PostProcessStack::Shutdown()
    {
        mFBO[0].Destroy();
        mFBO[1].Destroy();
        mPrevFrameFBO.Destroy();
        mHasPrevFrame = false;

        if (mQuadVAO)
        {
            glDeleteVertexArrays(1, &mQuadVAO);
            mQuadVAO = 0;
        }
        if (mQuadVBO)
        {
            glDeleteBuffers(1, &mQuadVBO);
            mQuadVBO = 0;
        }

        mEffects.clear();
    }

    bool PostProcessStack::HasActiveEffects() const
    {
        for (auto const& e : mEffects)
        {
            if (e && e->enabled)
                return true;
        }
        return false;
    }

    // -----------------------------------------------------------------------
    // FBO management
    // -----------------------------------------------------------------------
    void PostProcessStack::EnsureFBOs(int w, int h)
    {
        if (w <= 0 || h <= 0)
            return;

        // mFBO[0] is the scene target - needs depth for opaque rendering
        if (!mFBO[0].IsValid())
            mFBO[0].Create(w, h, true);
        else
            mFBO[0].Resize(w, h);

        // mFBO[1] is the ping-pong target - color only
        if (!mFBO[1].IsValid())
            mFBO[1].Create(w, h, false);
        else
            mFBO[1].Resize(w, h);

        // Motion blur previous frame FBO
        bool needsPrevFrame = false;
        for (auto const& e : mEffects)
        {
            if (e && e->enabled && e->NeedsPreviousFrame())
            {
                needsPrevFrame = true;
                break;
            }
        }
        if (needsPrevFrame)
        {
            if (!mPrevFrameFBO.IsValid())
                mPrevFrameFBO.Create(w, h);
            else
                mPrevFrameFBO.Resize(w, h);
        }
    }

    // -----------------------------------------------------------------------
    // Full-screen quad
    // -----------------------------------------------------------------------
    void PostProcessStack::EnsureFullScreenQuad()
    {
        if (mQuadVAO != 0)
            return;

        // NDC quad: position (x,y) + uv (u,v)
        // clang-format off
        float verts[] = {
            // pos       // uv
            -1.0f, -1.0f,  0.0f, 0.0f,
             1.0f, -1.0f,  1.0f, 0.0f,
             1.0f,  1.0f,  1.0f, 1.0f,

            -1.0f, -1.0f,  0.0f, 0.0f,
             1.0f,  1.0f,  1.0f, 1.0f,
            -1.0f,  1.0f,  0.0f, 1.0f,
        };
        // clang-format on

        glGenVertexArrays(1, &mQuadVAO);
        glGenBuffers(1, &mQuadVBO);

        glBindVertexArray(mQuadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, mQuadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);

        // position: location 0
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*) 0);
        // uv: location 1
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1,
                              2,
                              GL_FLOAT,
                              GL_FALSE,
                              4 * sizeof(float),
                              (void*) (2 * sizeof(float)));

        glBindVertexArray(0);
    }

    void PostProcessStack::DrawFullScreenQuad()
    {
        glBindVertexArray(mQuadVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);
    }

    // -----------------------------------------------------------------------
    // Scene target
    // -----------------------------------------------------------------------
    void PostProcessStack::BindSceneTarget(int viewportW, int viewportH)
    {
        EnsureFBOs(viewportW, viewportH);
        EnsureFullScreenQuad();

        // Scene renders into mFBO[0]
        mFBO[0].Bind();

        // Adjust scissor to match the FBO viewport (0,0 to w,h)
        // since RenderingLayer may have set scissor to the game viewport offset
        glScissor(0, 0, viewportW, viewportH);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    // -----------------------------------------------------------------------
    // Execute post-processing chain
    // -----------------------------------------------------------------------
    void PostProcessStack::Execute(int viewportX, int viewportY, int viewportW, int viewportH)
    {
        int currentRead = 0;  // mFBO[0] has the scene

        // Disable depth test for fullscreen quad passes
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_BLEND);

        for (auto const& effect : mEffects)
        {
            if (!effect || !effect->enabled)
                continue;

            Shader* shader = effect->GetShader(0);
            if (!shader)
                continue;

            int passCount = effect->GetPassCount();

            for (int pass = 0; pass < passCount; ++pass)
            {
                // May use a different shader per pass (though currently all built-ins use one)
                shader = effect->GetShader(pass);
                if (!shader)
                    continue;

                int writeTarget = 1 - currentRead;
                mFBO[writeTarget].Bind();
                glClear(GL_COLOR_BUFFER_BIT);

                shader->UseProgram();
                effect->SetUniforms(shader,
                                    pass,
                                    mFBO[currentRead].GetWidth(),
                                    mFBO[currentRead].GetHeight());

                // Bind source texture
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, mFBO[currentRead].GetColorTexture());

                // Bind previous frame for motion blur
                if (effect->NeedsPreviousFrame() && mHasPrevFrame)
                {
                    glActiveTexture(GL_TEXTURE1);
                    glBindTexture(GL_TEXTURE_2D, mPrevFrameFBO.GetColorTexture());
                }

                DrawFullScreenQuad();

                // Reset active texture
                glActiveTexture(GL_TEXTURE0);

                currentRead = writeTarget;
            }
        }

        // Save current result as previous frame if any effect needs it
        bool needsPrevFrame = false;
        for (auto const& e : mEffects)
        {
            if (e && e->enabled && e->NeedsPreviousFrame())
            {
                needsPrevFrame = true;
                break;
            }
        }
        if (needsPrevFrame && mPrevFrameFBO.IsValid())
        {
            // Copy current result to previous frame FBO via blit
            glBindFramebuffer(
                GL_READ_FRAMEBUFFER,
                mFBO[currentRead].IsValid() ? mFBO[currentRead].GetColorTexture() : 0);
            // Use glBlitFramebuffer for the copy
            GLuint srcFBO = 0;
            // We need the actual FBO handle. Since Framebuffer only exposes Bind(),
            // we'll do a fullscreen quad copy instead.
            mPrevFrameFBO.Bind();
            glClear(GL_COLOR_BUFFER_BIT);

            if (!mPassthroughShader)
                mPassthroughShader = ResourceManager::GetShader(kPassthroughShader);

            if (mPassthroughShader)
            {
                mPassthroughShader->UseProgram();
                mPassthroughShader->SetInt("screenTexture", 0);
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, mFBO[currentRead].GetColorTexture());
                DrawFullScreenQuad();
            }
            mHasPrevFrame = true;
        }

        // Blit final result to default framebuffer
        BlitToScreen(mFBO[currentRead].GetColorTexture(),
                     viewportX,
                     viewportY,
                     viewportW,
                     viewportH);
    }

    void PostProcessStack::BlitToScreen(GLuint texture, int vpX, int vpY, int vpW, int vpH)
    {
        Framebuffer::BindDefault();
        glViewport(vpX, vpY, vpW, vpH);
        glScissor(vpX, vpY, vpW, vpH);

        if (!mPassthroughShader)
            mPassthroughShader = ResourceManager::GetShader(kPassthroughShader);

        if (mPassthroughShader)
        {
            mPassthroughShader->UseProgram();
            mPassthroughShader->SetInt("screenTexture", 0);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, texture);
            DrawFullScreenQuad();
        }
    }

    // -----------------------------------------------------------------------
    // Effect management
    // -----------------------------------------------------------------------
    void PostProcessStack::AddEffect(std::unique_ptr<PostProcessEffect> effect)
    {
        if (effect)
            mEffects.push_back(std::move(effect));
    }

    void PostProcessStack::RemoveEffect(int index)
    {
        if (index >= 0 && index < static_cast<int>(mEffects.size()))
            mEffects.erase(mEffects.begin() + index);
    }

    void PostProcessStack::MoveEffect(int from, int to)
    {
        int size = static_cast<int>(mEffects.size());
        if (from < 0 || from >= size || to < 0 || to >= size || from == to)
            return;

        auto effect = std::move(mEffects[from]);
        mEffects.erase(mEffects.begin() + from);
        mEffects.insert(mEffects.begin() + to, std::move(effect));
    }

    const std::vector<std::unique_ptr<PostProcessEffect>>& PostProcessStack::GetEffects() const
    {
        return mEffects;
    }

    std::vector<std::unique_ptr<PostProcessEffect>>& PostProcessStack::GetEffects()
    {
        return mEffects;
    }

    // -----------------------------------------------------------------------
    // Serialization
    // -----------------------------------------------------------------------
    void PostProcessStack::Deserialize(JsonValue const& json)
    {
        mEffects.clear();

        if (!json.IsArray())
            return;

        for (size_t i = 0; i < json.Size(); ++i)
        {
            JsonValue item = json[i];
            if (!item.IsObject() || !item.Has("Effect"))
                continue;

            const char* typeName = item["Effect"].GetString("");
            auto effect = CreatePostProcessEffect(typeName);
            if (!effect)
                continue;

            effect->Deserialize(item);
            mEffects.push_back(std::move(effect));
        }
    }

    void PostProcessStack::Serialize(JsonDocument& doc) const
    {
        doc.BeginArray("PostProcessing");
        for (auto const& effect : mEffects)
        {
            if (!effect)
                continue;
            doc.BeginObject();
            effect->Serialize(doc);
            doc.EndObject();
        }
        doc.EndArray();
    }

}  // namespace mts
