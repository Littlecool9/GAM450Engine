/**
 * @file TilemapRenderer.cpp
 * @author Jackey Cen
 * @brief // TODO: Add a brief description of this class
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#include "pch.h"
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <Engine/Resource/Texture.h>
#include <Engine/Resource/Shader.h>

#include "Chunk.h"
#include "TilemapRenderer.h"
#include "Tilemap.h"
#include "Engine/Core/Editor/DebugDraw.h"

namespace mts::Tilemap
{
    void TilemapRenderer::DebugDraw(const Chunk& chunk) const
    {
        ChunkPos pos = chunk.GetPos();
        glm::vec3 worldPos = mOwner->ChunkPosToWorldPos(pos);

        // worldPos is bottom-left corner, so add half chunk size to get center for debug box
        glm::vec3 debugCenter =
            worldPos + glm::vec3(Chunk::CHUNK_UNIT_WIDTH / 2, Chunk::CHUNK_UNIT_HEIGHT / 2, 0.0f);
        mts::debug::DebugDraw::Box(
            debugCenter,
            glm::vec2(Chunk::CHUNK_UNIT_WIDTH / 2, Chunk::CHUNK_UNIT_HEIGHT / 2),
            0.0f,
            {0.0f, 0.0f, 1.0f, 1.0f});
    }
}  // namespace mts::Tilemap