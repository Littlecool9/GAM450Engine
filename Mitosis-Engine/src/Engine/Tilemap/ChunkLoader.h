/**
 * @file ChunkLoader.h
 * @author Jackey Cen
 * @brief // TODO: Add a brief description of this class
 *
 * @copyright Copyright (c) 2026 DigiPen (USA) Corporation
 *
 */

#pragma once

#include <thread>
#include <mutex>
#include <queue>
#include <future>
#include <vector>
#include <set>

#include "Chunk.h"
#include "Tile.h"

namespace mts::Tilemap
{
    struct ChunkLoadingTask
    {
        ChunkPos pos;
        std::shared_ptr<Chunk> chunkData;
        bool success = false;
    };

    struct ChunkUnloadingTask
    {
        ChunkPos pos;
        std::shared_ptr<Chunk> chunkToUnload;
        bool success = false;
    };

    class ChunkLoader
    {
       public:
        void Start(int threadCount = 1);

        // @brief: Adding a requested chunk position to a queue if it has not already been
        // requested, while ensuring thread safety through a mutex.
        void RequestChunk(ChunkPos pos, Tilemap* tilemap);
        void RequestUnloadChunk(ChunkPos pos, std::shared_ptr<Chunk> chunkToUnload);

        std::vector<ChunkLoadingTask> CollectResults();
        std::vector<ChunkUnloadingTask> CollectUnloadResults();

        // @brief: Signals all worker threads to stop and blocks until they finish.
        // Must be called before any data the workers reference (e.g. Tilesets) is freed.
        void Stop()
        {
            if (!m_running)
                return;
            m_running = false;
            m_cv.notify_all();
            for (auto& t : m_workers)
                t.join();
            m_workers.clear();
        }

        ~ChunkLoader()
        {
            Stop();
        }

       private:
        void WorkerThread()
        {
            while (m_running)
            {
                std::pair<ChunkPos, Tilemap*> loadPos;
                bool hasLoadTask = false;
                {
                    std::unique_lock<std::mutex> lock(m_queueMutex);
                    if (!m_requestQueue.empty())
                    {
                        loadPos = m_requestQueue.front();
                        m_requestQueue.pop();
                        hasLoadTask = true;
                    }
                }

                if (hasLoadTask)
                {
                    // Process loading task
                    ChunkLoadingTask result = LoadChunkData(loadPos.first, loadPos.second);
                    {
                        std::lock_guard<std::mutex> lock(m_resultMutex);
                        m_finishedTasks.push_back(std::move(result));
                    }
                    continue;
                }

                // Check for unloading tasks
                ChunkUnloadingTask unloadTask;
                bool hasUnloadTask = false;
                {
                    std::unique_lock<std::mutex> lock(m_unloadMutex);
                    if (!m_unloadQueue.empty())
                    {
                        unloadTask = m_unloadQueue.front();
                        m_unloadQueue.pop();
                        hasUnloadTask = true;
                    }
                }

                if (hasUnloadTask)
                {
                    // Process unloading task
                    UnloadChunkData(unloadTask);
                    {
                        std::lock_guard<std::mutex> lock(m_unloadResultMutex);
                        m_finishedUnloadTasks.push_back(std::move(unloadTask));
                    }
                    continue;
                }

                // Wait if no tasks available
                {
                    std::unique_lock<std::mutex> lock(m_queueMutex);
                    m_cv.wait(lock,
                              [this]
                              {
                                  return !m_requestQueue.empty() || !m_unloadQueue.empty() ||
                                         !m_running;
                              });
                }
            }
        }

        // @brief: Load data of a chunk at the given position
        // @brief: The data included:
        // @brief: 1. TileID data array, 2. Vertices and Indices for mesh
        ChunkLoadingTask LoadChunkData(ChunkPos pos, Tilemap* tilemap);

        // @brief: Risky to use this one now, need to ensure OpenGL context safety
        void UnloadChunkData(ChunkUnloadingTask& task);

        std::atomic<bool> m_running{false};

        // Loading queue
        std::queue<std::pair<ChunkPos, Tilemap*>> m_requestQueue;
        std::set<ChunkPos> m_pendingPositions;
        std::vector<ChunkLoadingTask> m_finishedTasks;

        // Unloading queue
        std::queue<ChunkUnloadingTask> m_unloadQueue;
        std::set<ChunkPos> m_pendingUnloads;
        std::vector<ChunkUnloadingTask> m_finishedUnloadTasks;

        // Mutexes for thread safety
        std::mutex m_queueMutex;
        std::mutex m_resultMutex;
        std::mutex m_unloadMutex;
        std::mutex m_unloadResultMutex;

        std::condition_variable m_cv;
        std::condition_variable m_unloadCv;
        std::vector<std::thread> m_workers;
    };
}  // namespace mts::Tilemap