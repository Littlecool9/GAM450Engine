/**
 * @file LogPanel.cpp
 * @author sumin.park
 * @brief  log ui
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#include "pch.h"
#include "LogPanel.h"

#include <imgui.h>
#include "Engine/Logging/Log.h"
#include "Engine/Core/Editor/EditorContext.h"

namespace mts
{
    void LogPanel::Draw(Scene* scene, EditorContext& ctx)
    {
        ImGui::Begin("Log");

        // Toolbar
        if (ImGui::Button("Clear"))
            Log::ClearEntries();
        ImGui::SameLine();
        ImGui::Checkbox("Auto-scroll", &mAutoScroll);

        ImGui::Separator();

        ImGui::BeginChild("##log_scroll",
                          ImVec2(0, 0),
                          false,
                          ImGuiWindowFlags_HorizontalScrollbar);

        const auto& entries = Log::GetEntries();
        for (const auto& entry : entries)
        {
            ImVec4 color;
            switch (entry.level)
            {
                case LogLevel::Warn:
                    color = ImVec4(1.0f, 0.85f, 0.0f, 1.0f);
                    break;
                case LogLevel::Error:
                    color = ImVec4(1.0f, 0.3f, 0.3f, 1.0f);
                    break;
                case LogLevel::Critical:
                    color = ImVec4(1.0f, 0.1f, 0.1f, 1.0f);
                    break;
                default:
                    color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
                    break;
            }
            ImGui::PushStyleColor(ImGuiCol_Text, color);
            ImGui::TextUnformatted(entry.text.c_str());
            ImGui::PopStyleColor();
        }

        if (mAutoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
            ImGui::SetScrollHereY(1.0f);

        ImGui::EndChild();
        ImGui::End();
    }
}  // namespace mts
