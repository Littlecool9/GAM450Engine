/**
 * @file Shader.cpp
 * @author josh.siegel
 * @brief // Shader object with OpenGL program ID and self-compilation function
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#include "pch.h"
#include "Shader.h"

#include <filesystem>
#include <fstream>
#include <sstream>

namespace mts
{
    void Shader::UseProgram() const
    {
        glUseProgram(this->ID);
    }
    
    void Shader::CreateProgram(const GLchar* vertexShaderSource, const GLchar* fragmentShaderSource)
    {
        GLuint vertexShader = CompileShader(GL_VERTEX_SHADER, vertexShaderSource);
        GLuint fragmentShader = CompileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);

        this->ID = glCreateProgram();
        if (!this->ID)
            return;

        glAttachShader(this->ID, vertexShader);
        glAttachShader(this->ID, fragmentShader);

        glLinkProgram(this->ID);

        GLint success = 0;
        glGetProgramiv(this->ID, GL_LINK_STATUS, &success);
        if (!success)
        {
            GLchar infoLog[512];
            glGetProgramInfoLog(this->ID, 512, nullptr, infoLog);
            LOG_ERROR("Shader linking error: ", infoLog);

            glDeleteProgram(this->ID);
            glDeleteShader(vertexShader);
            glDeleteShader(fragmentShader);

            this->ID = 0;
            return;
        }

        glDetachShader(this->ID, vertexShader);
        glDetachShader(this->ID, fragmentShader);
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
    }

    GLuint Shader::CompileShader(GLenum shaderType, const GLchar* shaderSource)
    {
        GLuint shader = glCreateShader(shaderType);
        glShaderSource(shader, 1, &shaderSource, nullptr);
        glCompileShader(shader);

        GLint success = 0;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            GLchar infoLog[512];
            glGetShaderInfoLog(shader, 512, nullptr, infoLog);
            LOG_ERROR("Shader compilation error: ", infoLog);

            glDeleteShader(shader);
            return NULL;
        }

        return shader;
    }

    void Shader::SetInt(const char* name, const int i) const
    {
        GLint loc = glGetUniformLocation(this->ID, name);
        glUniform1i(loc, i);
    }

    void Shader::SetFloat(const char* name, const float f) const
    {
        GLint loc = glGetUniformLocation(this->ID, name);
        glUniform1f(loc, f);
    }

    void Shader::SetVector2f(const char* name, const float* fv) const
    {
        GLint loc = glGetUniformLocation(this->ID, name);
        glUniform2fv(loc, 1, &(fv[0]));
    }

    void Shader::SetVector3f(const char* name, const float* fv) const
    {
        GLint loc = glGetUniformLocation(this->ID, name);
        glUniform3fv(loc, 1, &(fv[0]));
    }

    void Shader::SetVector4f(const char* name, const float* fv) const
    {
        GLint loc = glGetUniformLocation(this->ID, name);
        glUniform4fv(loc, 1, &(fv[0]));
    }

    void Shader::SetVector2i(const char* name, const int* iv) const
    {
        GLint loc = glGetUniformLocation(this->ID, name);
        glUniform2iv(loc, 1, &(iv[0]));
    }

    void Shader::SetVector3i(const char* name, const int* iv) const
    {
        GLint loc = glGetUniformLocation(this->ID, name);
        glUniform3iv(loc, 1, &(iv[0]));
    }

    void Shader::SetVector4i(const char* name, const int* iv) const
    {
        GLint loc = glGetUniformLocation(this->ID, name);
        glUniform4iv(loc, 1, &(iv[0]));
    }

    void Shader::SetMatrix3(const char* name, const float* fm) const
    {
        GLint loc = glGetUniformLocation(this->ID, name);
        glUniformMatrix3fv(loc, 1, GL_FALSE, fm);
    }

    void Shader::SetMatrix3(const char* name, const math::Mat3 fm) const
    {
        SetMatrix3(name, &fm[0][0]);
    }

    void Shader::SetMatrix4(const char* name, const float* fm) const
    {
        GLint loc = glGetUniformLocation(this->ID, name);
        glUniformMatrix4fv(loc, 1, GL_FALSE, fm);
    }

    void Shader::SetMatrix4(const char* name, const math::Mat4 fm) const
    {
        SetMatrix4(name, &fm[0][0]);
    }

    std::string Shader::LoadTextFile(const char* path)
    {
        std::ifstream file(path);
        std::stringstream ss;
        ss << file.rdbuf();
        return ss.str();
    }

}  // namespace mts