/**
 * @file FileManager.h
 * @author Dong Ta
 * @brief  A single class to handle file I/Os
 * @version 0.1
 * @date 2/11/2026 7:06:27 AM
 *
 * @copyright Copyright (c) 2025 - RM Engine
 *
 */




#pragma once
#include <string>
#include <vector>
#include <cstdint>

namespace mts
{
    class FileManager
    {
       public:

        static void SetRoot(std::string rootDir);

        /// Checks if a file exists at the given path.
        static bool Exists(const std::string& path);
        
        //adding comments here for sanity

        /**
         * @brief Reads an entire file into a string.
         *
         * Intended for:
         *  - JSON
         *  - shaders
         *  - text configs
         *  - scripts
         *
         * @param path Relative or absolute file path
         * @param outText Receives the full file contents
         * @return false if open/read fails
         */
        static bool ReadAllText(const std::string& path, std::string& outText);

        /**
         * @brief Writes a full string to disk.
         *
         * Intended for:
         *  - saving JSON
         *  - logs
         *  - debug dumps
         *
         * @param path Destination file
         * @param text Text content to write
         * @param createDirs Automatically create parent directories if missing
         */
        static bool WriteAllText(const std::string& path,
                                 const std::string& text,
                                 bool createDirs = true);
        /**
         * @brief Reads an entire file as raw bytes.
         *
         * Intended for:
         *  - textures
         *  - audio banks
         *  - binary formats
         *
         * @param outBytes Receives binary data
         */
        static bool ReadAllBytes(const std::string& path, std::vector<std::uint8_t>& outBytes);
        /**
         * @brief Writes raw bytes to disk.
         *
         * Useful for:
         *  - binary saves
         *  - exported assets
         */
        static bool WriteAllBytes(const std::string& path,
                                  const std::vector<std::uint8_t>& bytes,
                                  bool createDirs = true);

        /**
         * @brief Converts a relative path into a fully resolved path.
         *
         * Handles:
         *  - root prefixing
         *  - normalization
         *  - platform-safe separators
         *
         * Example:
         *   root = "Data"
         *   Resolve("Player.json")
         *   -> ".../TechDemo/Data/Player.json"
         *
         * Mostly used internally, but helpful for logging/debugging.
         */
        static std::string Resolve(const std::string& path);

       private:
        static std::string mRoot;
    };
}  // namespace mts