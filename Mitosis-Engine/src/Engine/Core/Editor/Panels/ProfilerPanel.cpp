/**
 * @file ProfilerPanel.cpp
 * @author sumin.park
 * @brief  Editor panel for FPS and memory profiling
 *
 * @copyright Copyright (c) 2026 DigiPen (USA) Corporation
 *
 */

#include "pch.h"
#include "ProfilerPanel.h"

#include <imgui.h>
#include "Engine/Memory/MemoryManager.h"

namespace mts
{
    // ---- RingBuffer ----

    ProfilerPanel::RingBuffer::RingBuffer(int size) : data(size, 0.0f) {}

    void ProfilerPanel::RingBuffer::Push(float v)
    {
        data[head] = v;
        head = (head + 1) % (int) data.size();
        if (head == 0)
            filled = true;
    }

    void ProfilerPanel::RingBuffer::CopyChronological(std::vector<float>& out) const
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

    // ---- ProfilerPanel ----

    ProfilerPanel::ProfilerPanel() : mFpsHist(240), mMemHistKB(240) {}

    float ProfilerPanel::BytesToKB(std::size_t b)
    {
        return (float) b / 1024.0f;
    }

    float ProfilerPanel::BytesToMB(std::size_t b)
    {
        return (float) b / (1024.0f * 1024.0f);
    }

    void ProfilerPanel::Draw(float dt)
    {
        // Update histories
        const float fps = (dt > 0.0f) ? (1.0f / dt) : 0.0f;
        mFpsHist.Push(fps);

        auto allocations     = MemoryManager::CurrentAllocationCount();
        auto allocatedBytes  = MemoryManager::CurrentAllocatedBytes();

        auto maxAllocations  = MemoryManager::MaxAllocationCount();
        auto maxAllocatedBytes = MemoryManager::MaxAllocatedBytes();

        auto frameFreeBytes  = MemoryManager::FrameFreeBytes();
        auto frameFreeCount  = MemoryManager::FrameFreeCount();

        mMemHistKB.Push(BytesToKB(allocatedBytes));

        // UI
        ImGui::Begin("Profiler");

        // FPS
        mFpsHist.CopyChronological(mTmp);
        if (!mTmp.empty())
        {
            ImGui::Text("FPS: %.1f", mTmp.back());
            ImGui::PlotLines("FPS over time",
                             mTmp.data(),
                             (int) mTmp.size(),
                             0,
                             nullptr,
                             0.0f,
                             240.0f,
                             ImVec2(0, 80));
        }

        ImGui::Separator();

        // Memory summary
        ImGui::Text("Active: %.2f KB (%zu allocs)", BytesToKB(allocatedBytes), allocations);

        ImGui::Text("Peak:   %.2f KB (%zu allocs)", BytesToKB(maxAllocatedBytes), maxAllocations);

        ImGui::Text("This frame: +%.2f KB (%zu allocs), -%.2f MB (%zu frees)",
                    BytesToKB(allocatedBytes),
                    allocations,
                    BytesToKB(frameFreeBytes),
                    frameFreeCount);

        // Memory graph
        mMemHistKB.CopyChronological(mTmp);
        if (!mTmp.empty())
            ImGui::PlotLines("Active KB over time",
                             mTmp.data(),
                             (int) mTmp.size(),
                             0,
                             nullptr,
                             0.0f,
                             500.0f,
                             ImVec2(0, 80));

        ImGui::End();
    }
}  // namespace mts
