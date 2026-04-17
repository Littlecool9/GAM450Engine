/**
 * @file AssetPanel.h
 * @author sumin.park
 * @brief  Asset browser panel — lists files on disk and supports drag-drop onto Inspector fields
 *
 * @copyright Copyright (c) 2026 DigiPen (USA) Corporation
 *
 */

#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include "Engine/Core/Editor/EditorContext.h"

namespace mts
{
    class Scene;

    class AssetPanel
    {
       public:
        AssetPanel();
        void Draw(Scene* scene, EditorContext& ctx);

       private:
        void Scan();
        void RebuildDirs();

        struct Entry { std::string name; std::string path; std::string displayName; bool isPrefab = false; bool isScene = false; int64_t lastWriteTimeNs = 0; };
        struct Dir   { std::string label; std::string scanPath; std::vector<std::string> extensions; std::vector<Entry> entries; };
        std::vector<Dir> mDirs;
        std::unordered_map<std::string, int64_t> mFileTimes;
        std::string mPendingScenePath;
        char mNewSceneNameBuf[128] = {};
        bool mOpenNewSceneModal = false;
        float mFontScale = 1.3f;
    };

}  // namespace mts
