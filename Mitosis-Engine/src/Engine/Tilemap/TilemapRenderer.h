/**
 * @file TilemapRenderer.h
 * @author Jackey Cen
 * @brief This is a renderer class for rendering tiles in tilemaps
 * @brief This class solve the problem of rendering irregular shape meshes of tilemap
 *
 * @copyright Copyright (c) 2026 DigiPen (USA) Corporation
 *
 */

#pragma once
#include <glad/glad.h>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

namespace mts
{
    class Shader;
    class Texture;
    class ISubmitRenderCommand;
    struct RenderCommand;

}  // namespace mts

namespace mts::Tilemap
{
    // Forward declaration
    class Chunk;
    class Tilemap;

    class TilemapRenderer
    {
       public:
        TilemapRenderer(Tilemap* owner) : mOwner(owner) {}

        void DebugDraw(const Chunk& chunk) const;

       private:
        Tilemap* mOwner;
    };

}  // namespace mts::Tilemap