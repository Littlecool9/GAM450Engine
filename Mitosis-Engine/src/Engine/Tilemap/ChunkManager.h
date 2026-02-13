/**
 * @file ChunkManager.h
 * @author Jackey Cen
 * @brief This class manages the loading and unloading of chunks in the tilemap
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
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

        // @brief: Update loaded chunks based on camera position
        // @brief: If some chunks are out of view, unload them
        // @brief: If some chunks are in view but not loaded, load them from the tilemap data
        void UpdateChunks(const RenderCamera& cam);
        // Replace the GetVisibleChunks method with the following implementation
        const std::unordered_set<Chunk*> GetVisibleChunks() const
        {
            std::unordered_set<Chunk*> result;
            for (const auto& pair : mActiveChunks)
            {
                if (pair.second && mVisibleChunks.find(pair.second.get()) != mVisibleChunks.end())
                {
                    result.insert(pair.second.get());
                }
            }
            return result;
        }

        void DebugDraw();

       private:
        Tilemap* mOwner;  // Owning tilemap
        ChunkLoader& mLoader;

        // The chunks currently loaded in memory
        std::unordered_map<ChunkPos, std::shared_ptr<Chunk>> mActiveChunks;

        // The chunks sent to renderer
        //std::vector<std::shared_ptr<Chunk>> mVisibleChunks;
        std::unordered_set<Chunk*> mVisibleChunks;
    };

}  // namespace mts::Tilemap    