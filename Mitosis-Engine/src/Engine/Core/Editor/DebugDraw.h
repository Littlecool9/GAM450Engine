/**
 * @file DebugDraw.h
 * @author sumin.park
 * @brief  System for debug drawing
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#pragma once
#include <glm/vec4.hpp>
#include <glm/ext/matrix_float4x4.hpp>

namespace mts::debug
{
    class DebugDraw
    {
       public:
        static void Init();
        static void Shutdown();

        static void Line(glm::vec2 a, glm::vec2 b, glm::vec4 color);
        static void Box(glm::vec2 center,
                        glm::vec2 halfExtents,
                        float rotationRad,
                        glm::vec4 color);
        static void Circle(glm::vec2 center, float radius, int segments, glm::vec4 color);

        static void Draw(const glm::mat4& viewProj);

       private:
        static void CheckCapacity(size_t vertexCount);
    };

}  // namespace mts::debug