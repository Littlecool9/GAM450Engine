/**
 * @file MemoryManageLayer.cpp
 * @author sumin.park
 * @brief  layer for memory management
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#include "pch.h"
#include "MemoryManageLayer.h"

#include <imgui.h>

namespace mts
{
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

    MemoryManageLayer::MemoryManageLayer() : Layer("MemoryManageLayer")
    {
    }

    void MemoryManageLayer::OnBeginFrame()
    {
        MemoryManager::BeginFrame();
    }

    void MemoryManageLayer::OnImGuiRender(float dt)
    {
        // Update histories
        const float fps = (dt > 0.0f) ? (1.0f / dt) : 0.0f;
        s_fpsHist.Push(fps);

        auto allocations = MemoryManager::CurrentAllocationCount();
        auto allocatedBytes = MemoryManager::CurrentAllocatedBytes();

        auto maxAllocations = MemoryManager::MaxAllocationCount();
        auto maxAllocatedBytes = MemoryManager::MaxAllocatedBytes();

        auto frameFreeBytes = MemoryManager::FrameFreeBytes();
        auto frameFreeCount = MemoryManager::FrameFreeCount();

        s_memHistMB.Push(
            BytesToKB(allocatedBytes));  // change bytestokb to bytestomb to convert to mb

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
        ImGui::Text("Active: %.2f KB (%zu allocs)", BytesToKB(allocatedBytes), allocations);

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
}  // namespace mts