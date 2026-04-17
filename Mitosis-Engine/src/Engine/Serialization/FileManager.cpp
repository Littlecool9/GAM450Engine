/**
 * @file FileManager.cpp
 * @author Dong Ta
 * @brief Definitions for file manager class
 * @version 0.1
 * @date 2/11/2026 7:06:27 AM
 *
 * @copyright Copyright (c) 2026 DigiPen (USA) Corporation
 *
 */


#include "pch.h"
#include "FileManager.h"


#include <filesystem>
#include <fstream>
#include <sstream>

namespace mts
{
    std::string FileManager::mRoot;

    void FileManager::SetRoot(std::string rootDir)
    {
        mRoot = std::move(rootDir);
    }

    std::string FileManager::Resolve(const std::string& path)
    {
        std::filesystem::path p(path);


        if (!mRoot.empty() && p.is_relative())
            p = std::filesystem::path(mRoot) / p;

        return p.lexically_normal().string();
    }

    bool FileManager::Exists(const std::string& path)
    {

        return std::filesystem::exists(Resolve(path));
    }

    bool FileManager::ReadAllText(const std::string& path, std::string& outText)
    {
        const std::string resolved = Resolve(path);

        std::ifstream file(resolved, std::ios::in | std::ios::binary);
        if (!file.is_open())
            return false;

        std::ostringstream ss;
        ss << file.rdbuf();
        outText = ss.str();
        return true;
    }

    bool FileManager::WriteAllText(const std::string& path, const std::string& text, bool createDirs)
    {
        const std::string resolved = Resolve(path);

        if (createDirs)
        {
            std::filesystem::path p(resolved);
            if (p.has_parent_path())
                std::filesystem::create_directories(p.parent_path());
        }

        std::ofstream file(resolved, std::ios::out | std::ios::binary | std::ios::trunc);
        if (!file.is_open())
            return false;

        file.write(text.data(), static_cast<std::streamsize>(text.size()));
        return true;
    }

    bool FileManager::ReadAllBytes(const std::string& path, std::vector<std::uint8_t>& outBytes)
    {
        const std::string resolved = Resolve(path);

        std::ifstream file(resolved, std::ios::in | std::ios::binary);
        if (!file.is_open())
            return false;

        file.seekg(0, std::ios::end);
        const std::streamsize size = file.tellg();
        file.seekg(0, std::ios::beg);

        if (size < 0)
            return false;

        outBytes.resize(static_cast<size_t>(size));
        file.read(reinterpret_cast<char*>(outBytes.data()), size);
        return true;
    }

    bool FileManager::WriteAllBytes(const std::string& path, const std::vector<std::uint8_t>& bytes, bool createDirs)
    {
        const std::string resolved = Resolve(path);

        if (createDirs)
        {
            std::filesystem::path p(resolved);
            if (p.has_parent_path())
                std::filesystem::create_directories(p.parent_path());
        }

        std::ofstream file(resolved, std::ios::out | std::ios::binary | std::ios::trunc);
        if (!file.is_open())
            return false;

        if (!bytes.empty())
            file.write(reinterpret_cast<const char*>(bytes.data()),
                       static_cast<std::streamsize>(bytes.size()));

        return true;
    }
}  // namespace mts