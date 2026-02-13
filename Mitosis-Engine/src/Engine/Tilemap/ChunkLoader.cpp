/**
 * @file ChunkLoader.cpp
 * @author Jackey Cen
 * @brief // TODO: Add a brief description of this class
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#include "pch.h"
#include "ChunkLoader.h"

namespace mts::Tilemap
{
    void ChunkLoader::Start(int threadCount)
    {
        if (m_running)
            return;  // Already running
        m_running = true;
        for (int i = 0; i < threadCount; ++i)
        {
            m_workers.emplace_back(&ChunkLoader::WorkerThread, this);
        }
    }

	std::vector<ChunkLoadingTask> ChunkLoader::CollectResults()
    {
        std::lock_guard<std::mutex> lock(m_resultMutex);
        std::vector<ChunkLoadingTask> results = std::move(m_finishedTasks);
        m_finishedTasks.clear();
        return results;
    }

    std::vector<ChunkUnloadingTask> ChunkLoader::CollectUnloadResults()
    {
        std::lock_guard<std::mutex> lock(m_unloadResultMutex);
        std::vector<ChunkUnloadingTask> results = std::move(m_finishedUnloadTasks);
        m_finishedUnloadTasks.clear();
        return results;
    }

    void ChunkLoader::RequestChunk(ChunkPos pos, Tilemap* tilemap)
    {
        std::lock_guard<std::mutex> lock(m_queueMutex);
        if (m_pendingPositions.find(pos) == m_pendingPositions.end())
        {
            m_requestQueue.push(std::make_pair(pos, tilemap));
            m_pendingPositions.insert(pos);
            m_cv.notify_one();
        }
    }

    void ChunkLoader::RequestUnloadChunk(ChunkPos pos, std::shared_ptr<Chunk> chunkToUnload)
    {
        std::lock_guard<std::mutex> lock(m_unloadMutex);
        if (m_pendingUnloads.find(pos) == m_pendingUnloads.end())
        {
            ChunkUnloadingTask task;
            task.pos = pos;
            task.chunkToUnload = chunkToUnload;
            m_unloadQueue.push(task);
            m_pendingUnloads.insert(pos);
            m_cv.notify_one();
        }
    }

    ChunkLoadingTask ChunkLoader::LoadChunkData(ChunkPos pos, Tilemap* tilemap)
    {
        ChunkLoadingTask task;
        task.pos = pos;

        // Load data into chunkData here
        task.chunkData = std::make_shared<Chunk>(pos);
        task.chunkData->BindTilemap(tilemap);
        task.success = task.chunkData->Load();

        return task;
    }

    void ChunkLoader::UnloadChunkData(ChunkUnloadingTask& task)
    {
        // Unload chunk data here
        task.success = task.chunkToUnload->Unload();
    }

}  // namespace mts::Tilemap