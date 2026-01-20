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

#include "Engine/Math/Vec2.h"
#include "Engine/Math/Vec3.h"
namespace mts
{
    struct Vertex
    {
        math::Vec2 pos;
        math::Vec2 uv;
        math::Vec3 color;
    };

    class Mesh
    {
       public:
        void BuildQuad(int numCols, int numRows, const math::Vec3 color);
        void Draw() const;
       private:
        GLuint vao = 0, vbo = 0, ibo = 0;
        GLsizei indexCount = 0;
    };

}  // namespace mts