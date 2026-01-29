/**
 * @file Mesh.h
 * @author sumin.park
 * @brief  Mesh class which has geometric info
 * @version 0.1
 * @date 1/8/2026 12:48:36 PM
 *
 * @copyright Copyright (c) 2025 - Mitosis Engine
 *
 */

#pragma once
#include <glad/glad.h>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include "Engine/Entity/Entity.h"

namespace mts
{
    struct Vertex
    {
        glm::vec2 pos;
        glm::vec2 uv;
        glm::vec3 color;
    };

    class Mesh : public Entity
    {
        MTS_CLASS_NEW_DELETE();
       public:
        void BuildQuad(int numCols, int numRows, const glm::vec3 color);
        void Draw() const;
       private:
        GLuint vao = 0, vbo = 0, ibo = 0;
        GLsizei indexCount = 0;
    };

}  // namespace mts