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

namespace mts
{
    Texture::Texture()
        : Texture(GL_RGB, GL_RGB, GL_UNSIGNED_BYTE, GL_LINEAR, GL_LINEAR, GL_REPEAT, GL_REPEAT, GL_REPEAT)
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
        : width(0),
          height(0),
          internal_format(internal_format),
          data_format(data_format),
          data_type(data_type),
          min_filter(min_filter),
          mag_filter(mag_filter),
          wrap_s(wrap_s),
          wrap_t(wrap_t),
          wrap_r(wrap_r)
    {
        glGenTextures(1, &this->ID);
    }
    
    void Texture::GenerateTexture(GLsizei width, GLsizei height, const GLuint* data)
    {
        this->width = width;
        this->height = height;
        glBindTexture(GL_TEXTURE_2D, this->ID);
        glTexImage2D(GL_TEXTURE_2D,
                     0,this->internal_format,
                     this->width,
                     this->height,
                     0,
                     this->data_format,
                     this->data_type,
                     data);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min_filter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mag_filter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap_s);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap_t);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, wrap_r);
    }

    void Texture::BindTexture() const
    {
        glBindTexture(GL_TEXTURE_2D, this->ID);
    }
}  // namespace mts