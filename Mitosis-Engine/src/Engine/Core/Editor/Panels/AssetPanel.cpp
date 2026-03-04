/**
 * @file AssetPanel.cpp
 * @author sumin.park
 * @brief  Asset browser panel — lists files on disk and supports drag-drop onto Inspector fields
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#include "pch.h"
#include "AssetPanel.h"

#include <imgui.h>
#include <filesystem>
#include <fstream>
#include "Engine/Resource/ResourceManager.h"
#include "Engine/EntitySystem/GameObject/GameObjectFactory.h"

static constexpr const char* kPrefsFile = "asset_panel.cfg";

namespace mts
{
    AssetPanel::AssetPanel()
    {
        if (std::ifstream f(kPrefsFile); f)
            std::getline(f, mProjectRoot);
        snprintf(mProjectRootBuf, sizeof(mProjectRootBuf), "%s", mProjectRoot.c_str());
        RebuildDirs();
        Scan();
    }

    void AssetPanel::RebuildDirs()
    {
        mDirs = {
            {"Data/Resources", mProjectRoot + "Data/Resources/", {".tex", ".mesh", ".shader"}, {}},
            {"Data", mProjectRoot + "Data/", {".prefab"}, {}},
            {"Assets", mProjectRoot + "Assets/", {".png", ".jpg"}, {}},
            {"Engine/Resources",
             "../Mitosis-Engine/src/Resources/",
             {".tex", ".mesh", ".shader"},
             {}},
        };
    }

    void AssetPanel::Scan()
    {
        namespace fs = std::filesystem;

        for (auto& dir : mDirs)
        {
            dir.entries.clear();

            fs::path dirPath(dir.scanPath);
            if (!fs::exists(dirPath))
                continue;

            for (auto& fsEntry : fs::directory_iterator(dirPath))
            {
                if (!fsEntry.is_regular_file())
                    continue;

                std::string ext = fsEntry.path().extension().string();
                bool match = false;
                for (const auto& allowed : dir.extensions)
                    if (ext == allowed)
                    {
                        match = true;
                        break;
                    }
                if (!match)
                    continue;

                Entry e;
                e.name = fsEntry.path().filename().string();
                e.path = fsEntry.path().string();
                e.isPrefab = (ext == ".prefab");
                e.displayName = e.isPrefab ? "[P] " + e.name : e.name;
                e.lastWriteTimeNs = fsEntry.last_write_time().time_since_epoch().count();

                auto it = mFileTimes.find(e.path);
                if (it != mFileTimes.end() && it->second != e.lastWriteTimeNs)
                {
                    LOG_INFO("Asset changed, evicting from cache: {}", e.path);
                    ResourceManager::ReloadResource(e.path);
                    if (e.isPrefab)
                        GameObjectFactory::RemoveArchetype(e.path);
                }
                mFileTimes[e.path] = e.lastWriteTimeNs;

                dir.entries.push_back(std::move(e));
            }
        }
    }

    void AssetPanel::Draw(Scene* scene, EditorContext& ctx)
    {
        ImGui::Begin("Assets");
        ImGui::SetWindowFontScale(mFontScale);

        // Project root input
        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - 60.0f);
        ImGui::InputText("##projroot", mProjectRootBuf, sizeof(mProjectRootBuf));
        ImGui::SameLine();
        if (ImGui::Button("Apply"))
        {
            mProjectRoot = mProjectRootBuf;
            if (std::ofstream f(kPrefsFile); f)
                f << mProjectRoot;
            RebuildDirs();
            Scan();
        }

        if (ImGui::Button("Refresh"))
            Scan();

        ImGui::Separator();

        for (auto& dir : mDirs)
        {
            if (ImGui::CollapsingHeader(dir.label.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
            {
                for (auto& entry : dir.entries)
                {
                    ImGui::PushID(entry.path.c_str());

                    if (entry.isPrefab)
                    {
                        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.4f, 0.85f, 1.0f, 1.0f));
                        ImGui::Selectable(entry.displayName.c_str());
                        ImGui::PopStyleColor();
                        if (ImGui::BeginDragDropSource())
                        {
                            ImGui::SetDragDropPayload("PREFAB_PATH",
                                                      entry.path.c_str(),
                                                      entry.path.size() + 1);
                            ImGui::Text("Prefab: %s", entry.name.c_str());
                            ImGui::EndDragDropSource();
                        }
                    }
                    else
                    {
                        ImGui::Selectable(entry.displayName.c_str());
                        if (ImGui::BeginDragDropSource())
                        {
                            ImGui::SetDragDropPayload("ASSET_PATH",
                                                      entry.path.c_str(),
                                                      entry.path.size() + 1);
                            ImGui::TextUnformatted(entry.name.c_str());
                            ImGui::EndDragDropSource();
                        }
                    }

                    ImGui::PopID();
                }
            }
        }

        ImGui::End();
    }

}  // namespace mts
