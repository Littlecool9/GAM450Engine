/**
 * @file ResourceManager.cpp
 * @author josh.siegel
 * @brief // Loads all texture, shader, and mesh resources and handles giving them to game objects and freeing them at the end of the program
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#include "pch.h"
#include "ResourceManager.h"
#include "Mesh.h"
#include "Shader.h"
#include "Texture.h"
#include "Engine/Serialization/JsonAbstraction.h"

#include <filesystem>
#include <fstream>
#include <sstream>
#include <ranges>

namespace mts
{
    const std::string defaultTextureSource = "../Mitosis-Engine/src/Resources/DefaultTextureResource.json";
    const std::string defaultShaderSource = "../Mitosis-Engine/src/Resources/DefaultShaderResource.json";
    const std::string defaultMeshSource = "../Mitosis-Engine/src/Resources/DefaultMeshResource.json";

    std::unordered_map<std::string, Texture*> ResourceManager::mTextureSources;
    std::unordered_map<std::string, Shader*> ResourceManager::mShaderSources;
    std::unordered_map<std::string, Mesh*> ResourceManager::mMeshSources;

    void ResourceManager::Init()
    {
        BuildTexture(defaultTextureSource);
        BuildShader(defaultShaderSource);
        BuildMesh(defaultMeshSource);
    }

    void ResourceManager::Shutdown()
    {
        for (const auto& texture : std::views::values(mTextureSources))
        {
            delete texture;
        }
        mTextureSources.clear();

        for (const auto& shader : std::views::values(mShaderSources))
        {
            delete shader;
        }
        mShaderSources.clear();

        for (const auto& mesh : std::views::values(mMeshSources))
        {
            delete mesh;
        }
        mMeshSources.clear();
    }

    Texture* ResourceManager::GetTexture(const std::string& textureSourcePath)
    {
        auto const search = mTextureSources.find(textureSourcePath);
        if (search != mTextureSources.end())
        {
            return search->second;
        }
        else
        {
            return BuildTexture(textureSourcePath);
        }
    }

    Shader* ResourceManager::GetShader(const std::string& shaderSourcePath)
    {
        auto const search = mShaderSources.find(shaderSourcePath);
        if (search != mShaderSources.end())
        {
            return search->second;
        }
        else
        {
            return BuildShader(shaderSourcePath);
        }
    }

    Mesh* ResourceManager::GetMesh(const std::string& meshSourcePath)
    {
        auto const search = mMeshSources.find(meshSourcePath);
        if (search != mMeshSources.end())
        {
            return search->second;
        }
        else
        {
            return BuildMesh(meshSourcePath);
        }
    }

    Texture* ResourceManager::BuildTexture(const std::string& textureSourcePath)
    {
        if (textureSourcePath.empty())
        {
            LOG_INFO("No Texture Source Specified");
            return GetTexture(defaultTextureSource);
        }

        // Load resource json file
        JsonDocument doc;
        if (!doc.LoadFromFile(textureSourcePath))
        {
            LOG_WARN("Error reading texture source: {}", textureSourcePath);
            return GetTexture(defaultTextureSource);
        }
        JsonValue const& jsonObj = doc.Root();

        // Build texture
        auto imageFile = "";
        if (jsonObj.Has("ImageFile"))
        {
            imageFile = jsonObj["ImageFile"].GetString(imageFile);
        }

        Texture* texture = new Texture();
        texture->GenerateTexture(imageFile);

        mTextureSources.insert(std::pair(textureSourcePath, texture));
        return texture;
    }

    Shader* ResourceManager::BuildShader(const std::string& shaderSourcePath)
    {
        if (shaderSourcePath.empty())
        {
            LOG_INFO("No Shader Source Specified");
            return GetShader(defaultShaderSource);
        }

        // Load resource json file
        JsonDocument doc;
        if (!doc.LoadFromFile(shaderSourcePath))
        {
            LOG_WARN("Error reading shader source: {}", shaderSourcePath);
            return GetShader(defaultShaderSource);
        }
        JsonValue const& jsonObj = doc.Root();

        // Read shaders from resource file
        auto vertFile = "";
        auto fragFile = "";

        if (jsonObj.Has("VertexShader"))
        {
            vertFile = jsonObj["VertexShader"].GetString("");
        }
        if (jsonObj.Has("FragmentShader"))
        {
            fragFile = jsonObj["FragmentShader"].GetString("");
        }

        // Build shader program
        Shader* shader = new Shader();
        auto vertSrc = LoadTextFile(vertFile);
        auto fragSrc = LoadTextFile(fragFile);
        shader->CreateProgram(vertSrc.c_str(), fragSrc.c_str());

        mShaderSources.insert(std::pair(shaderSourcePath, shader));
        return shader;
    }

    Mesh* ResourceManager::BuildMesh(const std::string& meshSourcePath)
    {
        if (meshSourcePath.empty())
        {
            LOG_INFO("No Mesh Source Specified");
            return GetMesh(defaultMeshSource);
        }

        // Load resource json file
        JsonDocument doc;
        if (!doc.LoadFromFile(meshSourcePath))
        {
            LOG_WARN("Error reading mesh source: {}", meshSourcePath);
            return GetMesh(defaultMeshSource);
        }
        JsonValue const& jsonObj = doc.Root();

        // Read in uv size
        int numRows = 1;
        int numCols = 1;
        if (jsonObj.Has("NumRows"))
        {
            numRows = jsonObj["NumRows"].GetInt(numRows);
        }
        if (jsonObj.Has("NumCols"))
        {
            numCols = jsonObj["NumCols"].GetInt(numCols);
        }

        // Build mesh
        Mesh* mesh = new Mesh();
        mesh->BuildQuad(numCols, numRows);

        mMeshSources.insert(std::pair(meshSourcePath, mesh));
        return mesh;
    }
    
    std::string ResourceManager::LoadTextFile(const char* path)
    {
        std::ifstream file(path);
        std::stringstream ss;
        ss << file.rdbuf();
        return ss.str();
    }

}  // namespace mts