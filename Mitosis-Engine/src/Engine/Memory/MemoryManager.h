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

        static std::size_t ActiveAllocationCount();
        static std::size_t ActiveBytes();
	};

    // For macros 
    template <class T, class... Args>
    T* NewImpl(std::source_location loc, Args&&... args)
    {
        void* mem = ::mts::MemoryManager::Alloc(sizeof(T), alignof(T), loc);
        if (!mem)
            throw std::bad_alloc{};
        return new (mem) T(std::forward<Args>(args)...);
    }

    template <class T>
    void DeleteImpl(T* ptr) noexcept
    {
        if (!ptr)
            return;
        ptr->~T();
        ::mts::MemoryManager::Free(ptr);
    }

    template <class T>
    T* NewArrayImpl(std::size_t count, std::source_location loc)
    {
        void* mem = ::mts::MemoryManager::Alloc(sizeof(T) * count, alignof(T), loc);
        if (!mem)
            throw std::bad_alloc{};
        T* arr = static_cast<T*>(mem);
        for (std::size_t i = 0; i < count; ++i)
            new (&arr[i]) T();
        return arr;
    }

    template <class T>
    void DeleteArrayImpl(T* ptr, std::size_t count) noexcept
    {
        if (!ptr)
            return;
        for (std::size_t i = 0; i < count; ++i)
            ptr[i].~T();
        ::mts::MemoryManager::Free(ptr);
    }
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

#define MTS_NEW(T, ...) ::mts::NewImpl<T>(std::source_location::current(), ##__VA_ARGS__)

#define MTS_DELETE(ptr) ::mts::DeleteImpl((ptr))

#define MTS_NEW_ARRAY(T, count) \
    ::mts::NewArrayImpl<T>((count), std::source_location::current())

#define MTS_DELETE_ARRAY(ptr, count) ::mts::DeleteArrayImpl((ptr), (count))