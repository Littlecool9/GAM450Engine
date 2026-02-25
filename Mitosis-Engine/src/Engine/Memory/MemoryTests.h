/**
 * @file MemoryTests.h
 * @author sumin.park
 * @brief  Tests for MemoryManager
 *
 * @copyright Copyright (c) 2025 - RM Engine
 *
 */

#pragma once
#include "MemoryManager.h"
#include "Engine/Logging/Assert.h"
#include <cstdint>


// Test A : Basic alloc/free round trip
// Goal: prove the map inserts and erases correctly.
// Expected: count/bytes unchanged.
void Test_BasicAllocFree()
{
    mts::MemoryManager::Init();

    const auto startCount = mts::MemoryManager::AllocationCount();
    const auto startBytes = mts::MemoryManager::ActiveBytes();

    void* p = MTS_ALLOC_BYTES(128, 16);
    MTS_FREE(p);

    const auto endCount = mts::MemoryManager::AllocationCount();
    const auto endBytes = mts::MemoryManager::ActiveBytes();

    // EXPECT: no change
    MTS_ASSERT(endCount == startCount);
    MTS_ASSERT(endBytes == startBytes);

    mts::MemoryManager::Shutdown();
}
// Test B : Leak detection (intentional leak)
// Goal: prove leaks show up in your map.
// Expected: eak appears with file/function/line.
void Test_LeakDetection()
{
    mts::MemoryManager::Init();

    void* leak = MTS_ALLOC_BYTES(256, 32);
    (void) leak;

    MTS_ASSERT(mts::MemoryManager::AllocationCount() >= 1);
    
    mts::MemoryManager::Shutdown();
}

bool IsAligned(void* p, std::size_t align)
{
    return (reinterpret_cast<std::uintptr_t>(p) & (align - 1)) == 0;
}

// Test C : Alignment correctness
// Goal : confirm returned pointers are actually aligned.
// Expected : assertions pass.
void Test_Alignment()
{
    mts::MemoryManager::Init();

    void* p16 = MTS_ALLOC_BYTES(1, 16);
    void* p64 = MTS_ALLOC_BYTES(1, 64);

    MTS_ASSERT(IsAligned(p16, 16));
    MTS_ASSERT(IsAligned(p64, 64));

    MTS_FREE(p16);
    MTS_FREE(p64);

    mts::MemoryManager::Shutdown();
}

// Test D : ctor/dtor + tracking
// Goal : confirm placement new + destructor path + free path works.
// Expected : alive returns to 0 and map is empty.
struct Probe
{
    static inline int alive = 0;
    int x = 0;

    Probe(int v) : x(v) { ++alive; }
    ~Probe() { --alive; }
};

void Test_NewDelete()
{
    mts::MemoryManager::Init();

    MTS_ASSERT(Probe::alive == 0);

    Probe* p = MTS_NEW(Probe, 123);
    MTS_ASSERT(p->x == 123);
    MTS_ASSERT(Probe::alive == 1);

    MTS_DELETE(p);
    MTS_ASSERT(Probe::alive == 0);

    MTS_ASSERT(mts::MemoryManager::AllocationCount() == 0);

    mts::MemoryManager::Shutdown();
}

// Test E : Array ctor/dtor correctness
// Goal : confirm MTS_NEW_ARRAY calls constructors and MTS_DELETE_ARRAY calls destructors for each element.
// Expected : alive becomes N then returns to 0.
struct ProbeArr
{
    static inline int alive = 0;
    ProbeArr() { ++alive; }
    ~ProbeArr() { --alive; }
};

void Test_ArrayNewDelete()
{
    mts::MemoryManager::Init();

    constexpr std::size_t N = 10;

    MTS_ASSERT(ProbeArr::alive == 0);

    ProbeArr* arr = MTS_NEW_ARRAY(ProbeArr, N);
    MTS_ASSERT(ProbeArr::alive == (int) N);

    MTS_DELETE_ARRAY(arr, N);
    MTS_ASSERT(ProbeArr::alive == 0);

    MTS_ASSERT(mts::MemoryManager::AllocationCount() == 0);

    mts::MemoryManager::Shutdown();
}

// Test F : Double-free and invalid-free detection
// Goal : Double-free and invalid-free detection
// Expected : your system should scream loudly if you enable this test.
void Test_DoubleFree()
{
    mts::MemoryManager::Init();

    void* p = MTS_ALLOC_BYTES(64, 16);
    MTS_FREE(p);

    // Should assert/log (ptr not found)
    MTS_FREE(p);

    mts::MemoryManager::Shutdown();
}
