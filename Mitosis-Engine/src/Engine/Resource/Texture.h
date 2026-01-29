/**
 * @file Texture.h
 * @author josh.siegel
 * @brief // Texture object with OpenGL texture data and texture use functions
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#pragma once

#include <glad/glad.h>
#include "Engine/Entity/Entity.h"

namespace mts
{
    class Texture : public Entity
    {
        MTS_CLASS_NEW_DELETE();
       public:
        Texture();
        Texture(GLint internal_format,
                GLenum data_format,
                GLenum data_type,
                GLint min_filter,
                GLint mag_filter,
                GLint wrap_s,
                GLint wrap_t,
                GLint wrap_r);

        // Generates texture from image data
        void GenerateTexture(GLsizei width, GLsizei height, const GLuint* data);
        
        // Binds object's ID as current texture
        void BindTexture() const;

        GLuint GetID() const { return ID; }

       private:
        // texture internal opengl id
        GLuint ID;

        // texture dimensions
        GLsizei width;
        GLsizei height;

        // texture image params
        GLint internal_format;
        GLenum data_format;
        GLenum data_type;

        // texture params
        GLint min_filter;
        GLint mag_filter;
        GLint wrap_s;
        GLint wrap_t;
        GLint wrap_r;
    };

}  // namespace mts