/**
 * @file Framebuffer.h
 * @author Jackey Cen
 * @brief OpenGL framebuffer object wrapper for off-screen rendering
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#pragma once

#include <glad/glad.h>

namespace mts
{
    class Framebuffer
    {
       public:
        Framebuffer() = default;
        ~Framebuffer();

        Framebuffer(const Framebuffer&) = delete;
        Framebuffer& operator=(const Framebuffer&) = delete;
        Framebuffer(Framebuffer&& other) noexcept;
        Framebuffer& operator=(Framebuffer&& other) noexcept;

        // withDepth: attach a depth renderbuffer (needed for scene FBOs, not for PP ping-pong)
        void Create(int width, int height, bool withDepth = false);
        void Resize(int width, int height);
        void Bind() const;
        static void BindDefault();
        void Destroy();

        bool IsValid() const { return mFBO != 0; }
        GLuint GetColorTexture() const { return mColorTexture; }
        int GetWidth() const { return mWidth; }
        int GetHeight() const { return mHeight; }

       private:
        GLuint mFBO = 0;
        GLuint mColorTexture = 0;
        GLuint mDepthRBO = 0;
        bool mHasDepth = false;
        int mWidth = 0;
        int mHeight = 0;
    };

}  // namespace mts
