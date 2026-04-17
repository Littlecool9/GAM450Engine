/**
 * @file AssetPanel.cpp
 * @author sumin.park
 * @brief  Asset browser panel — lists files on disk and supports drag-drop onto Inspector fields
 *
 * @copyright Copyright (c) 2026 DigiPen (USA) Corporation
 *
 */

#include "pch.h"
#include "AssetPanel.h"

#include <imgui.h>
#include <filesystem>
#include "Engine/Resource/ResourceManager.h"
#include "Engine/EntitySystem/GameObject/GameObjectFactory.h"
#include "Engine/SceneSystem/SceneSystem.h"
#include "Engine/SceneSystem/Scene.h"

namespace mts
{
    AssetPanel::AssetPanel()
    {
        RebuildDirs();
        Scan();
    }

    void AssetPanel::RebuildDirs()
    {
        mDirs = {
            {"Data/Scenes",    "Data/Scenes/",       {".scene"},                   {}},
            {"Data/Resources", "Data/Resources/",    {".tex", ".mesh", ".shader"}, {}},
            {"Data",           "Data/",              {".prefab"},                  {}},
            {"Assets",         "Assets/",            {".png", ".jpg"},             {}},
            {"Engine/Resources", "Engine/Resources/", {".tex", ".mesh", ".shader"}, {}},
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
                e.isScene = (ext == ".scene");
                if (e.isPrefab)
                    e.displayName = "[P] " + e.name;
                else if (e.isScene)
                    e.displayName = "[S] " + e.name;
                else
                    e.displayName = e.name;
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

        if (ImGui::Button("Refresh"))
            Scan();
        ImGui::SameLine();
        if (ImGui::Button("New Scene"))
            mOpenNewSceneModal = true;

        ImGui::Separator();

        for (auto& dir : mDirs)
        {
            if (ImGui::CollapsingHeader(dir.label.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
            {
                for (auto& entry : dir.entries)
                {
                    ImGui::PushID(entry.path.c_str());

                    if (entry.isScene)
                    {
                        if (ctx.playState != PlayState::Edit)
                        {
                            ImGui::PopID();
                            continue;
                        }
                        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.2f, 1.0f, 0.4f, 1.0f));
                        bool clicked = ImGui::Selectable(entry.displayName.c_str());
                        ImGui::PopStyleColor();
                        if (clicked && !ImGui::IsMouseDragging(ImGuiMouseButton_Left))
                            mPendingScenePath = entry.path;
                        if (ImGui::BeginDragDropSource())
                        {
                            ImGui::SetDragDropPayload("ASSET_PATH",
                                                      entry.path.c_str(),
                                                      entry.path.size() + 1);
                            ImGui::Text("Scene: %s", entry.name.c_str());
                            ImGui::EndDragDropSource();
                        }
                    }
                    else if (entry.isPrefab)
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

        if (mOpenNewSceneModal)
        {
            ImGui::OpenPopup("New Scene");
            mOpenNewSceneModal = false;
        }

        if (ImGui::BeginPopupModal("New Scene", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::TextUnformatted("Scene name:");
            ImGui::SetNextItemWidth(260.0f);
            ImGui::InputText("##sceneName", mNewSceneNameBuf, sizeof(mNewSceneNameBuf));
            ImGui::Spacing();
            bool nameEmpty = (mNewSceneNameBuf[0] == '\0');
            if (nameEmpty) ImGui::BeginDisabled();
            if (ImGui::Button("Create"))
            {
                std::string path = "Data/Scenes/";
                path += mNewSceneNameBuf;
                path += ".scene";
                SceneSystem::CreateScene(path);
                mNewSceneNameBuf[0] = '\0';
                Scan();
                ImGui::CloseCurrentPopup();
            }
            if (nameEmpty) ImGui::EndDisabled();
            ImGui::SameLine();
            if (ImGui::Button("Cancel"))
            {
                mNewSceneNameBuf[0] = '\0';
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }

        if (!mPendingScenePath.empty())
            ImGui::OpenPopup("Switch Scene?");

        if (ImGui::BeginPopupModal("Switch Scene?", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::Text("Switch scene? Any unsaved changes will be lost.");
            ImGui::Spacing();
            if (ImGui::Button("Switch"))
            {
                SceneSystem::OpenScene(mPendingScenePath);
                ctx.selected = UUID::InvalidID;
                mPendingScenePath.clear();
                ImGui::CloseCurrentPopup();
            }
            ImGui::SameLine();
            if (ImGui::Button("Cancel"))
            {
                mPendingScenePath.clear();
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }

        ImGui::End();
    }

}  // namespace mts
