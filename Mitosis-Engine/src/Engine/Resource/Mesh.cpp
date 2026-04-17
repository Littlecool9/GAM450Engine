/**
 * @file Mesh.cpp
 * @author sumin.park
 * @brief  Mesh class which has geometric info
 * @version 0.1
 * @date 1/8/2026 12:48:36 PM
 *
 * @copyright Copyright (c) 2026 DigiPen (USA) Corporation
 *
 */

#include "pch.h"
#include "Mesh.h"

namespace mts
{
    void Mesh::BuildQuad(int numCols, int numRows)
    {
        mNumCols = numCols;
        mNumRows = numRows;

        float uSize = 1.0f / static_cast<float>(numCols);
        float vSize = 1.0f / static_cast<float>(numRows);

        Vertex vertices[] = {
            {.pos = glm::vec2(-0.5f, -0.5f), .uv = glm::vec2(0.0f, vSize)},
            {.pos = glm::vec2(-0.5f, 0.5f), .uv = glm::vec2(0.0f, 0.0f)},
            {.pos = glm::vec2(0.5f, -0.5f), .uv = glm::vec2(uSize, vSize)},
            {.pos = glm::vec2(0.5f, 0.5f), .uv = glm::vec2(uSize, 0.0f)}
        };

        unsigned int indices[] = {
            1, 2, 0,
            1, 3, 2
        };

        mIndexCount = 6;

        glGenVertexArrays(1, &mVAO);
        glGenBuffers(1, &mVBO);
        glGenBuffers(1, &mIBO);
        
        glBindVertexArray(mVAO);
        glBindBuffer(GL_ARRAY_BUFFER, mVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) (0));

        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) offsetof(Vertex, uv));

        glBindVertexArray(0);
    }

    void Mesh::Draw() const
    {
        glBindVertexArray(mVAO);
        glDrawElements(GL_TRIANGLES, this->mIndexCount, GL_UNSIGNED_INT, nullptr);
        glBindVertexArray(0);
    }

}  // namespace mts