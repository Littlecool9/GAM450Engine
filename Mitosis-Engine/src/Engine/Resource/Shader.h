/**
 * @file Shader.h
 * @author josh.siegel
 * @brief // Shader object with OpenGL program ID and self-compilation function
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#pragma once

#include <glad/glad.h>
#include <glm/mat3x3.hpp>
#include <glm/mat4x4.hpp>

#include "Engine/Entity/Entity.h"

namespace mts
{
    class Shader : public Entity
    {
        MTS_CLASS_NEW_DELETE();

       public:
        void UseProgram() const;
        void CreateProgram(const GLchar* vertexShaderSource, const GLchar* fragmentShaderSource);
        GLuint GetID() const { return ID; }

        // Uniform setting wrapper functions
        // Tech Dept : cache location
        void SetInt(const char* name, int i) const;
        void SetFloat(const char* name, float f) const;
        void SetVector2f(const char* name, const glm::vec2& fv) const;
        void SetVector3f(const char* name, const glm::vec3& fv) const;
        void SetVector4f(const char* name, const glm::vec4& fv) const;
        void SetVector2i(const char* name, const glm::ivec2& iv) const;
        void SetVector3i(const char* name, const glm::ivec3& iv) const;
        void SetVector4i(const char* name, const glm::ivec4& iv) const;
        void SetMatrix3(const char* name, const float* fm) const;
        void SetMatrix3(const char* name, const glm::mat3& fm) const;
        void SetMatrix4(const char* name, const float* fm) const;
        void SetMatrix4(const char* name, const glm::mat4& fm) const;

        const std::string& GetPath() const { return mPath; }
        void SetPath(std::string path) { mPath = std::move(path); }

       private:
        // shader program ID
        GLuint ID = 0;
        // saves resource paths
        std::string mPath;

        static GLuint CompileShader(GLenum shaderType, const GLchar* shaderSource);
    };

}  // namespace mts