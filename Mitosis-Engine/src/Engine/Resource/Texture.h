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

        // Generates texture from image file
        void GenerateTexture(const std::string& filepath);

        // Binds object's ID as current texture
        void BindTexture() const;
        void UnbindTexture() const;

        GLuint GetID() const { return mID; }

        // Returns true if texture has an alpha channel
        bool HasTextureAlpha() const { return mHasTextureAlpha; }

        const std::string& GetPath() const { return mPath; }
        void SetPath(std::string path) { mPath = std::move(path); }

       private:
        // texture internal opengl id
        GLuint mID;

        // texture dimensions
        GLsizei mWidth;
        GLsizei mHeight;

        // texture image params
        GLint mInternal_format;
        GLenum mData_format;
        GLenum mData_type;

        // texture params
        GLint mMin_filter;
        GLint mMag_filter;
        GLint mWrap_s;
        GLint mWrap_t;
        GLint mWrap_r;

        bool mHasTextureAlpha = true;

        // saves resource paths
        std::string mPath;
    };

}  // namespace mts