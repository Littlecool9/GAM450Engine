/**
 * @file ChunkManager.h
 * @author Jackey Cen
 * @brief This class manages the loading and unloading of chunks in the tilemap
 *
 * @copyright Copyright (c) 2026 DigiPen (USA) Corporation
 *
 */

#pragma once
#include <vector>
#include "Chunk.h"
#include <unordered_set>

namespace mts
{
    struct RenderCamera;
}

namespace mts::Tilemap
{
    class Tilemap;
    class ChunkLoader;

    class ChunkManager
    {
       public:
        ChunkManager(Tilemap* owner, ChunkLoader& loader) : mOwner(owner), mLoader(loader)
        {}
        ~ChunkManager();


        // @brief: Update loaded chunks based on camera position
        // @brief: If some chunks are out of view, unload them
        // @brief: If some chunks are in view but not loaded, load them from the tilemap data
        void UpdateChunks(const RenderCamera& cam);
        // Returns the set of chunks that are both loaded and visible this frame.
        // Invariant: mVisibleChunks is cleared at the start of UpdateChunks() and only populated
        // from entries that currently exist in mActiveChunks, so every pointer here is valid.
        const std::unordered_set<Chunk*>& GetVisibleChunks() const
        {
#ifdef MTS_DEBUG
            for (Chunk* c : mVisibleChunks)
            {
                bool found = false;
                for (const auto& pair : mActiveChunks)
                    if (pair.second.get() == c) { found = true; break; }
                MTS_ASSERT(found && "mVisibleChunks contains a pointer not in mActiveChunks");
            }
#endif
            return mVisibleChunks;
        }

        void DebugDraw();

       private:
        Tilemap* mOwner;  // Owning tilemap
        ChunkLoader& mLoader;

        // The chunks currently loaded in memory
        std::unordered_map<ChunkPos, std::shared_ptr<Chunk>> mActiveChunks;

        // The chunks sent to renderer
        std::unordered_set<Chunk*> mVisibleChunks;

        // Collider GameObjects are expensive to create. When a chunk unloads, its pool is moved
        // here so it can be injected back on next load instead of creating new objects.
        std::unordered_map<ChunkPos, std::vector<GameObject*>> mColliderPools;
    };

}  // namespace mts::Tilemap    