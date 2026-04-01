/**
 * @file Framebuffer.cpp
 * @brief OpenGL framebuffer object wrapper for off-screen rendering
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#include "pch.h"
#include "Framebuffer.h"

namespace mts
{
    Framebuffer::~Framebuffer()
    {
        Destroy();
    }

    Framebuffer::Framebuffer(Framebuffer&& other) noexcept
        : mFBO(other.mFBO),
          mColorTexture(other.mColorTexture),
          mDepthRBO(other.mDepthRBO),
          mHasDepth(other.mHasDepth),
          mWidth(other.mWidth),
          mHeight(other.mHeight)
    {
        other.mFBO = 0;
        other.mColorTexture = 0;
        other.mDepthRBO = 0;
        other.mHasDepth = false;
        other.mWidth = 0;
        other.mHeight = 0;
    }

    Framebuffer& Framebuffer::operator=(Framebuffer&& other) noexcept
    {
        if (this != &other)
        {
            Destroy();
            mFBO = other.mFBO;
            mColorTexture = other.mColorTexture;
            mDepthRBO = other.mDepthRBO;
            mHasDepth = other.mHasDepth;
            mWidth = other.mWidth;
            mHeight = other.mHeight;
            other.mFBO = 0;
            other.mColorTexture = 0;
            other.mDepthRBO = 0;
            other.mHasDepth = false;
            other.mWidth = 0;
            other.mHeight = 0;
        }
        return *this;
    }

    void Framebuffer::Create(int width, int height, bool withDepth)
    {
        MTS_ASSERT(width > 0 && height > 0);

        mWidth = width;
        mHeight = height;
        mHasDepth = withDepth;

        // Create FBO
        glGenFramebuffers(1, &mFBO);
        glBindFramebuffer(GL_FRAMEBUFFER, mFBO);

        // Create color attachment texture
        glGenTextures(1, &mColorTexture);
        glBindTexture(GL_TEXTURE_2D, mColorTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE,
                     nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        // Attach color to FBO
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mColorTexture,
                               0);

        // Optional depth renderbuffer (needed for scene rendering with depth test)
        if (withDepth)
        {
            glGenRenderbuffers(1, &mDepthRBO);
            glBindRenderbuffer(GL_RENDERBUFFER, mDepthRBO);
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width, height);
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER,
                                     mDepthRBO);
            glBindRenderbuffer(GL_RENDERBUFFER, 0);
        }

        // Check completeness
        GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        if (status != GL_FRAMEBUFFER_COMPLETE)
        {
            LOG_ERROR("Framebuffer not complete! Status: 0x{:X}", status);
        }

        // Unbind
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void Framebuffer::Resize(int width, int height)
    {
        if (width == mWidth && height == mHeight)
            return;

        bool hadDepth = mHasDepth;
        Destroy();
        Create(width, height, hadDepth);
    }

    void Framebuffer::Bind() const
    {
        MTS_ASSERT(mFBO != 0);
        glBindFramebuffer(GL_FRAMEBUFFER, mFBO);
        glViewport(0, 0, mWidth, mHeight);
    }

    void Framebuffer::BindDefault()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void Framebuffer::Destroy()
    {
        if (mDepthRBO)
        {
            glDeleteRenderbuffers(1, &mDepthRBO);
            mDepthRBO = 0;
        }
        if (mColorTexture)
        {
            glDeleteTextures(1, &mColorTexture);
            mColorTexture = 0;
        }
        if (mFBO)
        {
            glDeleteFramebuffers(1, &mFBO);
            mFBO = 0;
        }
        mHasDepth = false;
        mWidth = 0;
        mHeight = 0;
    }

}  // namespace mts
