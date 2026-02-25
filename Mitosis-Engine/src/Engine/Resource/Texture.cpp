/**
 * @file Texture.cpp
 * @author josh.siegel
 * @brief // Texture object with OpenGL texture data and texture use functions
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#include "pch.h"
#include "Texture.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace mts
{
    Texture::Texture()
        : Texture(GL_RGB,
                  GL_RGB,
                  GL_UNSIGNED_BYTE,
                  GL_NEAREST,
                  GL_NEAREST,
                  GL_REPEAT,
                  GL_REPEAT,
                  GL_REPEAT)
    {
    }

    Texture::Texture(GLint internal_format,
                     GLenum data_format,
                     GLenum data_type,
                     GLint min_filter,
                     GLint mag_filter,
                     GLint wrap_s,
                     GLint wrap_t,
                     GLint wrap_r)
        : mWidth(0),
          mHeight(0),
          mInternal_format(internal_format),
          mData_format(data_format),
          mData_type(data_type),
          mMin_filter(min_filter),
          mMag_filter(mag_filter),
          mWrap_s(wrap_s),
          mWrap_t(wrap_t),
          mWrap_r(wrap_r)
    {
        glGenTextures(1, &mID);
    }

    void Texture::GenerateTexture(const std::string& filepath)
    {
        int channels;

        // Load image file
        unsigned char* data = stbi_load(filepath.c_str(), &mWidth, &mHeight, &channels, 0);

        if (channels == 4)
        {
            mHasTextureAlpha = true;
            mInternal_format = GL_RGBA;
            mData_format = GL_RGBA;
        }
        else
        {
            mHasTextureAlpha = false;
            mInternal_format = GL_RGB;
            mData_format = GL_RGB;
        }

        if (data)
        {
            glBindTexture(GL_TEXTURE_2D, mID);
            glTexImage2D(GL_TEXTURE_2D,
                         0,
                         mInternal_format,
                         mWidth,
                         mHeight,
                         0,
                         mData_format,
                         mData_type,
                         data);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, mMin_filter);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mMag_filter);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, mWrap_s);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, mWrap_t);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, mWrap_r);
            glBindTexture(GL_TEXTURE_2D, 0);
        }
        else
        {
            LOG_ERROR("Failed to load texture");
        }

        // Free image file data
        stbi_image_free(data);
    }

    void Texture::BindTexture() const
    {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, mID);
    }

    void Texture::UnbindTexture() const
    {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
}  // namespace mts