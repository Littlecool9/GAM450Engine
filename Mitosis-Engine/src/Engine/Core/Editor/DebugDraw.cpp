/**
 * @file DebugDraw.cpp
 * @author sumin.park
 * @brief  System for debug drawing
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#include "pch.h"
#include "DebugDraw.h"

#include <glm/ext/matrix_clip_space.hpp>
#include <glm/gtc/constants.hpp>

#include "Engine/Resource/Shader.h"
#include "Engine/Resource/ResourceManager.h"

namespace mts::debug
{
    struct DebugVertex
    {
        glm::vec2 pos;
        glm::vec4 color;
    };

    static bool initialized = false;

    static std::vector<DebugVertex> debugVerticies;
    static size_t vboMaxVerts = 0;

    static GLuint vao = 0;
    static GLuint vbo = 0;
    static Shader* shader = nullptr;

    void DebugDraw::Init()
    {
        initialized = true;

        shader =
            ResourceManager::GetShader("../Mitosis-Engine/src/Resources/DebugShaderResource.shader");

        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);

        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);

        vboMaxVerts = 2048;
        glBufferData(GL_ARRAY_BUFFER,
                     (GLsizeiptr) (vboMaxVerts * sizeof(DebugVertex)),
                     nullptr,
                     GL_DYNAMIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0,
                              2,
                              GL_FLOAT,
                              GL_FALSE,
                              sizeof(DebugVertex),
                              (void*) offsetof(DebugVertex, pos));

        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1,
                              4,
                              GL_FLOAT,
                              GL_FALSE,
                              sizeof(DebugVertex),
                              (void*) offsetof(DebugVertex, color));

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        debugVerticies.reserve(vboMaxVerts);

        initialized = true;
    }

    void DebugDraw::Shutdown()
    {
        if (!initialized)
            return;

        if (vbo)
        {
            glDeleteBuffers(1, &vbo);
            vbo = 0;
        }
        if (vao)
        {
            glDeleteVertexArrays(1, &vao);
            vao = 0;
        }

        debugVerticies.clear();

        shader = nullptr;

        initialized = false;
    }

    void DebugDraw::Draw(const glm::mat4& viewProj)
    {
        if (!initialized || debugVerticies.empty())
            return;

        CheckCapacity(debugVerticies.size());
        shader->UseProgram();

        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferSubData(GL_ARRAY_BUFFER,
                        0,
                        (GLsizeiptr) (debugVerticies.size() * sizeof(DebugVertex)),
                        debugVerticies.data());
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        shader->SetMatrix4("viewProj", viewProj);

        glBindVertexArray(vao);
        glDrawArrays(GL_LINES, 0, (GLsizei) debugVerticies.size());
        glBindVertexArray(0);

        debugVerticies.clear();
    }

    void DebugDraw::Line(glm::vec2 a, glm::vec2 b, glm::vec4 color)
    {
        debugVerticies.push_back(DebugVertex{a, color});
        debugVerticies.push_back(DebugVertex{b, color});
    }

    void DebugDraw::Box(glm::vec2 center, glm::vec2 halfExtents, float rotationRad, glm::vec4 color)
    {
        glm::vec2 p[4] = {
            {-halfExtents.x, -halfExtents.y},
            {halfExtents.x, -halfExtents.y},
            {halfExtents.x, halfExtents.y},
            {-halfExtents.x, halfExtents.y},
        };

        float s = sin(rotationRad), cs = cos(rotationRad);
        for (int i = 0; i < 4; i++)
        {
            glm::vec2 v = p[i];
            glm::vec2 r = {v.x * cs - v.y * s, v.x * s + v.y * cs};
            p[i] = r + center;
        }

        Line(p[0], p[1], color);
        Line(p[1], p[2], color);
        Line(p[2], p[3], color);
        Line(p[3], p[0], color);
    }

    void DebugDraw::Circle(glm::vec2 center, float radius, int segments, glm::vec4 color)
    {
        segments = std::max(segments, 8);
        float step = glm::two_pi<float>() / float(segments);

        glm::vec2 prev = center + glm::vec2(radius, 0);
        for (int i = 1; i <= segments; i++)
        {
            float a = step * i;
            glm::vec2 cur = center + glm::vec2(cos(a) * radius, sin(a) * radius);
            Line(prev, cur, color);
            prev = cur;
        }
    }

    void DebugDraw::CheckCapacity(size_t vertexCount)
    {
        if (vertexCount <= vboMaxVerts)
            return;
        while (vboMaxVerts < vertexCount)
            vboMaxVerts *= 2;

        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER,
                     (GLsizeiptr) (vboMaxVerts * sizeof(DebugVertex)),
                     nullptr,
                     GL_DYNAMIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        debugVerticies.reserve(vboMaxVerts);
    }
}  // namespace mts::debug