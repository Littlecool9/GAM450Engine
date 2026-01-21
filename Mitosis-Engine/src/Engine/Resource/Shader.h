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

namespace mts
{
    class Shader
    {
       public:
        void UseProgram() const;
        void CreateProgram(const GLchar* vertexShaderSource, const GLchar* fragmentShaderSource);
        GLuint GetID() const { return ID; }

        // Uniform setting wrapper functions
        void SetInt(const char* name, const int i) const;
        void SetFloat(const char* name, const float f) const;
        void SetVector2f(const char* name, const float* fv) const;
        void SetVector3f(const char* name, const float* fv) const;
        void SetVector4f(const char* name, const float* fv) const;
        void SetVector2i(const char* name, const int* iv) const;
        void SetVector3i(const char* name, const int* iv) const;
        void SetVector4i(const char* name, const int* iv) const;
        void SetMatrix3(const char* name, const float* fm) const;
        void SetMatrix3(const char* name, const glm::mat3& fm) const;
        void SetMatrix4(const char* name, const float* fm) const;
        void SetMatrix4(const char* name, const glm::mat4& fm) const;

        // Josh TODO: File io should be moved elsewhere
        static std::string LoadTextFile(const char* path);

       private:
        // shader program ID
        GLuint ID;

        static GLuint CompileShader(GLenum shaderType, const GLchar* shaderSource);
    };

}  // namespace mts