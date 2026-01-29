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
    };

    class Mesh : public Entity
    {
        MTS_CLASS_NEW_DELETE();
       public:
        void BuildQuad(int numCols, int numRows);
        void Draw() const;
       private:
        GLuint mVAO = 0, mVBO = 0, mIBO = 0;
        GLsizei mIndexCount = 0;
        
    };

}  // namespace mts