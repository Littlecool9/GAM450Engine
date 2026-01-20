/**
 * @file Mesh.cpp
 * @author sumin.park
 * @brief  Mesh class which has geometric info
 * @version 0.1
 * @date 1/8/2026 12:48:36 PM
 *
 * @copyright Copyright (c) 2025 - Mitosis Engine
 *
 */

#include "pch.h"
#include "Mesh.h"

namespace mts
{
    void Mesh::BuildQuad(int numCols, int numRows, const math::Vec3 color)
    {
        float uSize = 1.0f / numCols;
        float vSize = 1.0f / numRows;

        Vertex vertices[] = {
            {.pos = math::Vec2(0.0f, 0.0f), .uv = math::Vec2(0.0f, 0.0f), .color = color},
            {.pos = math::Vec2(0.0f, 1.0f), .uv = math::Vec2(0.0f, vSize), .color = color},
            {.pos = math::Vec2(1.0f, 0.0f), .uv = math::Vec2(uSize, 0.0f), .color = color},
            {.pos = math::Vec2(1.0f, 1.0f), .uv = math::Vec2(uSize, vSize), .color = color}
        };

        unsigned int indices[] = {
            1, 2, 0,
            1, 3, 2
        };

        this->indexCount = 6;

        glGenVertexArrays(1, &this->vao);
        glGenBuffers(1, &this->vbo);
        glGenBuffers(1, &this->ibo);
        
        glBindVertexArray(this->vao);
        glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->ibo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) (0));

        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) offsetof(Vertex, uv));

        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) offsetof(Vertex, color));

        glBindVertexArray(0);
    }

    void Mesh::Draw() const
    {
        glBindVertexArray(this->vao);
        glDrawElements(GL_TRIANGLES, this->indexCount, GL_UNSIGNED_INT, nullptr);
        glBindVertexArray(0);
    }
}  // namespace mts