/**
 * @file MemoryManager.h
 * @author sumin.park
 * @brief  Class that control of allocation and deallocation
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#pragma once

#include <string_view>

namespace mts
{
    struct MemoryInfo
    {
        std::size_t sizeBytes;
        std::size_t alignBytes;
        std::string_view fileName;
        std::string_view functionName;
        std::uint_least32_t line;
    };

	class MemoryManager
	{
	public:
        static void Init();
        static void Shutdown();
        static void* Alloc(std::size_t sizeBytes,
                          std::size_t align,
                          std::source_location location = std::source_location::current());
        static void Free(void* ptr);
        static void MemorymaDrawProfilerWindow(float dt);
        static void BeginFrame();
        static std::size_t ActiveAllocationCount();
        static std::size_t ActiveBytes();
	};
} // mts namespace

#define MTS_ALLOC_BYTES(sizeBytes, alignBytes) \
    ::mts::MemoryManager::Alloc((sizeBytes), (alignBytes), std::source_location::current())

#define MTS_FREE(ptr) ::mts::MemoryManager::Free((ptr))

#define MTS_ALLOC_T(T) \
    static_cast<T*>(   \
        ::mts::MemoryManager::Alloc(sizeof(T), alignof(T), std::source_location::current()))

#define MTS_ALLOC_ARRAY(T, count)                                    \
    static_cast<T*>(::mts::MemoryManager::Alloc(sizeof(T) * (count), \
                                                alignof(T),          \
                                                std::source_location::current()))

// Opt-in per-class allocation (put in class definition)
#define MTS_CLASS_NEW_DELETE()                                                            \
   public:                                                                                \
    static void* operator new(std::size_t sz,                                             \
                              std::source_location loc = std::source_location::current()) \
    {                                                                                     \
        void* mem = ::mts::MemoryManager::Alloc(sz, alignof(std::max_align_t), loc);      \
        if (!mem)                                                                         \
            throw std::bad_alloc{};                                                       \
        return mem;                                                                       \
    }                                                                                     \
    static void operator delete(void* p) noexcept                                         \
    {                                                                                     \
        ::mts::MemoryManager::Free(p);                                                    \
    }                                                                                     \
    static void operator delete(void* p, std::source_location) noexcept                   \
    {                                                                                     \
        ::mts::MemoryManager::Free(p);                                                    \
    }                                                                                     \
    static void* operator new(std::size_t sz,                                             \
                              std::align_val_t al,                                        \
                              std::source_location loc = std::source_location::current()) \
    {                                                                                     \
        void* mem = ::mts::MemoryManager::Alloc(sz, static_cast<std::size_t>(al), loc);   \
        if (!mem)                                                                         \
            throw std::bad_alloc{};                                                       \
        return mem;                                                                       \
    }                                                                                     \
    static void operator delete(void* p, std::align_val_t) noexcept                       \
    {                                                                                     \
        ::mts::MemoryManager::Free(p);                                                    \
    }                                                                                     \
                                                                                          \
   private:                                                                               \
    /* making sure it's a complete declaration so we can add a semicolon at the end*/     \
    static_assert(true, "MTS_CLASS_NEW_DELETE") 
