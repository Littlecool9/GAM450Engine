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
#include "Engine/String/Format.h"
#include <unordered_map>
#include <mutex>

namespace mts
{
    static bool initialized = false;
    static std::size_t allocatedBytes = 0;
    static std::size_t allocations = 0;
    static std::size_t maxAllocatedBytes = 0;
    static std::size_t maxAllocations = 0;
    static std::unordered_map<void*, MemoryInfo> allocationInfo;

    // we're not on multi-thread, but in case we ever do.
    static std::mutex memoryMutex;

    static void* PlatformAlloc(std::size_t sizeBytes, std::size_t alignBytes)
    {
        //todo: platform specific needed
        alignBytes = std::max<std::size_t>(alignBytes, alignof(std::max_align_t));
        return _aligned_malloc(sizeBytes, alignBytes);
    }

    static void PlatformFree(void* ptr)
    {
        //todo: platform specific needed
        
        _aligned_free(ptr);
    }

    void MemoryManager::Init()
    {
        std::scoped_lock lock(memoryMutex);

        initialized = true;
        allocationInfo = std::unordered_map<void*, MemoryInfo>();
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
        LOG_INFO("Allocations : {}, {} bytes", maxAllocations, maxAllocatedBytes);

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

        MemoryInfo info = {
            .sizeBytes = sizeBytes,
            .alignBytes = align,
            .fileName = string_format::ShortFile(location.file_name()),
            .functionName = string_format::ShortFunc(location.function_name()),
            .line = location.line()
        };
        
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
} // mts namespace