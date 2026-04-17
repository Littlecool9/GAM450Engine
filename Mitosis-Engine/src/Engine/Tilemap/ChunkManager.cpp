/**
 * @file ChunkManager.cpp
 * @author Jackey Cen
 * @brief // TODO: Add a brief description of this class
 *
 * @copyright Copyright (c) 2026 DigiPen (USA) Corporation
 *
 */

#include "pch.h"

#include "Engine/Renderer.h"
#include "ChunkLoader.h"
#include "ChunkManager.h"
#include "Tilemap.h"
#include <math.h>
#include "Engine/EntitySystem/GameObject/GameObject.h"
#include "Engine/Core/Editor/DebugDraw.h"

#define CHUNK_LOAD_ADDITIONAL_RADIUS 1    // in chunks
#define CHUNK_UNLOAD_ADDITIONAL_RADIUS 2  // in chunks

namespace mts::Tilemap
{
    ChunkManager::~ChunkManager()
    {
        // Pool GOs are owned by the scene. ClearGameObjects() deletes them.
        mColliderPools.clear();
    }

    void ChunkManager::UpdateChunks(const RenderCamera& cam)
    {
        // Rebuild visible set each frame — stale raw pointers from unloaded chunks
        // would otherwise accumulate here indefinitely.
        mVisibleChunks.clear();

        // Handle the async result of loaded chunks
        auto results = mLoader.CollectResults();
        for (const ChunkLoadingTask& task : results)
        {
            if (task.success)
            {
                task.chunkData->BuildMesh();
                // Inject a previously-saved collider pool so we reuse existing GameObjects
                // instead of allocating new ones on every reload.
                auto poolIt = mColliderPools.find(task.pos);
                if (poolIt != mColliderPools.end())
                {
                    task.chunkData->InjectColliderPool(std::move(poolIt->second));
                    mColliderPools.erase(poolIt);
                }
                task.chunkData->BuildColliders();
                mActiveChunks[task.pos] = task.chunkData;
            }
        }

        glm::vec4 viewport = Renderer::GetViewportInfo();
        int screenWidth = static_cast<int>(viewport.z);
        int screenHeight = static_cast<int>(viewport.w);

        glm::vec2 worldLB = Renderer::ScreenToWorld(glm::vec3(0.0f, 0.0f, 0.0f));
        glm::vec2 worldRT = Renderer::ScreenToWorld(glm::vec3(screenWidth, screenHeight, 0.0f));

        glm::vec2 tilemapWorldLB = mOwner->GetLeftBottomWorldPos();
        glm::vec2 tilemapWorldRT = mOwner->GetRightTopWorldPos();

        glm::vec2 overlapWorldLB = glm::max(worldLB, tilemapWorldLB);
        glm::vec2 overlapWorldRT = glm::min(worldRT, tilemapWorldRT);

        ChunkPos visibleChunkLB = mOwner->WorldToChunkPos(glm::vec3(overlapWorldLB, 0));
        ChunkPos visibleChunkRT = mOwner->WorldToChunkPos(glm::vec3(overlapWorldRT, 0));

        if (visibleChunkLB.x > visibleChunkRT.x)
            std::swap(visibleChunkLB.x, visibleChunkRT.x);
        if (visibleChunkLB.y > visibleChunkRT.y)
            std::swap(visibleChunkLB.y, visibleChunkRT.y);

        // 3. Calculate load/unload range
        int loadL = visibleChunkLB.x - CHUNK_LOAD_ADDITIONAL_RADIUS;
        int loadB = visibleChunkLB.y - CHUNK_LOAD_ADDITIONAL_RADIUS;
        int loadR = visibleChunkRT.x + CHUNK_LOAD_ADDITIONAL_RADIUS;
        int loadT = visibleChunkRT.y + CHUNK_LOAD_ADDITIONAL_RADIUS;

        int unloadL = visibleChunkLB.x - CHUNK_UNLOAD_ADDITIONAL_RADIUS;
        int unloadB = visibleChunkLB.y - CHUNK_UNLOAD_ADDITIONAL_RADIUS;
        int unloadR = visibleChunkRT.x + CHUNK_UNLOAD_ADDITIONAL_RADIUS;
        int unloadT = visibleChunkRT.y + CHUNK_UNLOAD_ADDITIONAL_RADIUS;

        // 4. Unload
        for (auto it = mActiveChunks.begin(); it != mActiveChunks.end();)
        {
            ChunkPos p = it->first;
            if (p.x < unloadL || p.x > unloadR || p.y < unloadB || p.y > unloadT)
            {
                // Steal the collider pool before the Chunk is destroyed so the GameObjects
                // stay alive (inactive) and can be reused when this position reloads.
                mColliderPools[p] = it->second->StealColliderPool();
                it->second->Unload();
                it = mActiveChunks.erase(it);
            }
            else
            {
                ++it;
            }
        }
        // iterate all chunks in the load range
        for (int y = loadB; y <= loadT; ++y)
        {
            for (int x = loadL; x <= loadR; ++x)
            {
                if (x == 3)
                {
                    int debug = 0;
                }
                ChunkPos p{x, y};

                if (!mOwner->IsValidChunkPos(p))
                    continue;

                bool isLoaded = mActiveChunks.count(p) > 0;

                // condition of being visible
                bool isVisible = (x >= visibleChunkLB.x && x <= visibleChunkRT.x &&
                                  y >= visibleChunkLB.y && y <= visibleChunkRT.y);

                if (isLoaded)
                {
                    if (isVisible)
                    {
                        // In load range and loaded, add to visible chunks
                        mVisibleChunks.insert(mActiveChunks[p].get());
                    }
                }
                else
                {
                    // In load range but not loaded yet, request loading
                    mLoader.RequestChunk(p, mOwner);
                }
            }
        }
    }

    void ChunkManager::DebugDraw()
    {
        // Use GetVisibleChunks() — it cross-references mActiveChunks so it never
        // returns a pointer to a chunk that has already been freed.
        for (const auto& chunk : GetVisibleChunks())
        {
            chunk->DebugDrawCollider();
        }
        glm::vec2 tilemapWorldLB = mOwner->GetLeftBottomWorldPos();
        glm::vec2 tilemapWorldRT = mOwner->GetRightTopWorldPos();

        mts::debug::DebugDraw::Box((tilemapWorldLB + tilemapWorldRT) * 0.5f,
                                   (tilemapWorldRT - tilemapWorldLB) * 0.5f,
                                   0.0f,
                                   glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
    }

}  // namespace mts::Tilemap