/**
 * @file ResourceManager.h
 * @author josh.siegel
 * @brief // Loads all texture, shader, and mesh resources and handles giving them to game objects and freeing them at the end of the program
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#pragma once

namespace mts
{
    // Forward Declarations
    class Texture;
    class Shader;
    class Mesh;

    class ResourceManager
    {
        MTS_CLASS_NEW_DELETE();
       public:
        static void Init();
        static void Shutdown();

        static Texture* GetTexture(const std::string& textureSourcePath);
        static Shader* GetShader(const std::string& shaderSourcePath);
        static Mesh* GetMesh(const std::string& meshSourcePath);

       private:
        static Texture* BuildTexture(const std::string& textureSourcePath);
        static Shader* BuildShader(const std::string& shaderSourcePath);
        static Mesh* BuildMesh(const std::string& meshSourcePath);

        static std::string LoadTextFile(const char* path);

        static std::unordered_map<std::string, Texture*> mTextureSources;
        static std::unordered_map<std::string, Shader*> mShaderSources;
        static std::unordered_map<std::string, Mesh*> mMeshSources;
    };

}  // namespace mts