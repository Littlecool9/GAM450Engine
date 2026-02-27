/**
 * @file AssetPanel.h
 * @author sumin.park
 * @brief  Asset browser panel — lists files on disk and supports drag-drop onto Inspector fields
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#pragma once
#include <string>
#include <vector>
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

        struct Entry { std::string name; std::string path; std::string displayName; bool isPrefab = false; };
        struct Dir   { std::string label; std::string scanPath; std::vector<std::string> extensions; std::vector<Entry> entries; };
        std::vector<Dir> mDirs;
        float mFontScale = 1.3f;
    };

}  // namespace mts
