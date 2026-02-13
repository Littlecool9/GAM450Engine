/**
 * @file MemoryManager.cpp
 * @author sumin.park
 * @brief  Class that control of allocation and deallocation
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#include "pch.h"
#include "MemoryManager.h"
#include "imgui.h"
#include "Engine/Helpers/Format.h"
#include <unordered_map>
#include <mutex>

namespace mts
{
    static bool initialized = false;
    static std::size_t allocatedBytes = 0;
    static std::size_t allocations = 0;
    static std::size_t maxAllocatedBytes = 0;
    static std::size_t maxAllocations = 0;
    // variables added for ImGUI Graph
    static std::size_t frameFreeBytes = 0;
    static std::size_t frameFreeCount = 0;
    // end
    static std::unordered_map<void*, MemoryInfo> allocationInfo;
    static std::vector<MemoryInfo> allAllocationsInfo;

    // we're not on multi-thread, but in case we ever do.
    static std::mutex memoryMutex;

    static void* PlatformAlloc(std::size_t sizeBytes, std::size_t alignBytes)
    {
        // todo: platform specific needed
        alignBytes = std::max<std::size_t>(alignBytes, alignof(std::max_align_t));
        return _aligned_malloc(sizeBytes, alignBytes);
    }

    static void PlatformFree(void* ptr)
    {
        // todo: platform specific needed

        _aligned_free(ptr);
    }

    // TODO: Decide whether this code stays in memory manager or not
    namespace  // Putting this in empty namespace here so it's not viewable outside of this file
    {
        struct RingBuffer
        {
            std::vector<float> data;
            int head = 0;
            bool filled = false;

            explicit RingBuffer(int size) : data(size, 0.0f) {}

            void Push(float v)
            {
                data[head] = v;
                head = (head + 1) % (int) data.size();
                if (head == 0)
                    filled = true;
            }

            void CopyChronological(std::vector<float>& out) const
            {
                out.clear();
                out.reserve(data.size());

                if (!filled)
                {
                    out.insert(out.end(), data.begin(), data.begin() + head);
                    return;
                }
                out.insert(out.end(), data.begin() + head, data.end());
                out.insert(out.end(), data.begin(), data.begin() + head);
            }
        };

        static RingBuffer s_fpsHist(240);
        static RingBuffer s_memHistMB(240);
        static std::vector<float> s_tmp;

        static float BytesToMB(std::size_t b)
        {
            return (float) b / (1024.0f * 1024.0f);
        }
        static float BytesToKB(std::size_t b)
        {
            return (float) b / 1024.0f;
        }
    }  // namespace
    // TODO: Decide where this function goes, application or stay here or separate class
    void MemoryManager::MemorymaDrawProfilerWindow(float dt)
    {
        // Update histories
        const float fps = (dt > 0.0f) ? (1.0f / dt) : 0.0f;
        s_fpsHist.Push(fps);

        s_memHistMB.Push(BytesToKB(
            mts::MemoryManager::ActiveBytes()));  // change bytestokb to bytestomb to convert to mb

        // UI
        ImGui::Begin("Profiler");

        // FPS
        s_fpsHist.CopyChronological(s_tmp);
        if (!s_tmp.empty())
        {
            ImGui::Text("FPS: %.1f", s_tmp.back());
            ImGui::PlotLines("FPS over time",
                             s_tmp.data(),
                             (int) s_tmp.size(),
                             0,
                             nullptr,
                             0.0f,
                             240.0f,
                             ImVec2(0, 80));
        }

        ImGui::Separator();

        // Memory summary
        ImGui::Text("Active: %.2f KB (%zu allocs)",
                    BytesToKB(mts::MemoryManager::ActiveBytes()),
                    mts::MemoryManager::ActiveAllocationCount());

        ImGui::Text("Peak:   %.2f KB (%zu allocs)", BytesToKB(maxAllocatedBytes), maxAllocations);

        ImGui::Text("This frame: +%.2f KB (%zu allocs), -%.2f MB (%zu frees)",
                    BytesToKB(allocatedBytes),
                    allocations,
                    BytesToKB(frameFreeBytes),
                    frameFreeCount);

        // Memory graph
        s_memHistMB.CopyChronological(s_tmp);
        if (!s_tmp.empty())
            ImGui::PlotLines("Active KB over time",
                             s_tmp.data(),
                             (int) s_tmp.size(),
                             0,
                             nullptr,
                             0.0f,
                             500.0f,  // 500 kilobytes, change to for max value of graph
                             ImVec2(0, 80));

        ImGui::End();
    }
    //
    void MemoryManager::Init()
    {
        std::scoped_lock lock(memoryMutex);

        initialized = true;
        allocationInfo = std::unordered_map<void*, MemoryInfo>();
    }

    void MemoryManager::BeginFrame()
    {
        std::scoped_lock lock(memoryMutex);
        frameFreeBytes = 0;
        frameFreeCount = 0;
    }

    void MemoryManager::Shutdown()
    {
        if (!initialized)
            LOG_CRITICAL("Shutdown called before init");

        if (allocations > 0)
        {
            LOG_CRITICAL("=== Leak Report ===");
            for (auto& [ptr, info] : allocationInfo)
            {
                LOG_CRITICAL("[{}: line {}]", info.fileName, info.line);
                PlatformFree(ptr);
            }
            LOG_CRITICAL("==================");
        }
        else
        {
            LOG_INFO("=== No Leak ===");
        }
        for (auto& info : allAllocationsInfo)
        {
            LOG_INFO("[{} bytes : {} line {}]", info.sizeBytes, info.fileName, info.line);
        }
        LOG_INFO("{} allocations, {} bytes", maxAllocations, maxAllocatedBytes);

        allocationInfo.clear();
        allocatedBytes = 0;
        allocations = 0;
        initialized = false;
    }

    void* MemoryManager::Alloc(std::size_t sizeBytes,
                               std::size_t align,
                               std::source_location location)
    {
        if (!initialized)
        {
            LOG_CRITICAL("MemoryManager::Alloc called before MemoryManager::Init()");
            return nullptr;
        }

        void* ptr = PlatformAlloc(sizeBytes, align);

        MemoryInfo info = {.sizeBytes = sizeBytes,
                           .alignBytes = align,
                           .fileName = string_format::ShortFile(location.file_name()),
                           .functionName = string_format::ShortFunc(location.function_name()),
                           .line = location.line()};

        if (ptr == nullptr)
        {
            LOG_CRITICAL("Memory allocation failure at [{}: line {}][{}]",
                         info.fileName,
                         info.line,
                         info.functionName);
            return nullptr;
        }
        {
            std::scoped_lock lock(memoryMutex);
            allocatedBytes += sizeBytes;
            allocations += 1;
            maxAllocatedBytes += sizeBytes;
            maxAllocations += 1;
            allocationInfo[ptr] = info;
            allAllocationsInfo.push_back(info);
        }

        return ptr;
    }

    void MemoryManager::Free(void* ptr)
    {
        {
            std::scoped_lock lock(memoryMutex);
            auto it = allocationInfo.find(ptr);

            MTS_ASSERT(it != allocationInfo.end());

            auto& info = it->second;

            MTS_ASSERT(allocatedBytes >= info.sizeBytes);
            allocatedBytes -= info.sizeBytes;

            MTS_ASSERT(allocations > 0);
            allocations -= 1;
            frameFreeBytes += info.sizeBytes;
            frameFreeCount += 1;
            allocationInfo.erase(it);
        }
        PlatformFree(ptr);
    }

    std::size_t MemoryManager::ActiveAllocationCount()
    {
        return allocations;
    }

    std::size_t MemoryManager::ActiveBytes()
    {
        return allocatedBytes;
    }
}  // namespace mts