/**
 * @file SceneSystem.cpp
 * @author sumin.park
 * @brief  The scene manager
 * @version 0.1
 * @date 1/8/2026 7:40:23 AM
 *
 * @copyright Copyright (c) 2026 DigiPen (USA) Corporation
 *
 */

#include "pch.h"
#include "SceneSystem.h"
#include "Scene.h"
#include "Engine/Serialization/SerializationUtility.h"
#include "Engine/Serialization/JsonAbstraction.h"
#include "Engine/Serialization/FileManager.h"
#include <filesystem>

namespace mts
{
    std::vector<std::unique_ptr<Scene>> SceneSystem::mOwnedScenes;
    std::vector<std::string> SceneSystem::mRegisteredScenes;
    int SceneSystem::mPendingSceneIndex = -1;
    Scene* SceneSystem::currentScene = nullptr;
    bool SceneSystem::sInPlayMode = false;
    Delegate<> SceneSystem::onChangeSceneDelegate;

    void SceneSystem::Init(const std::string& path)
    {
        auto scene = std::make_unique<Scene>();
        SerializationUtility::DeserializeFromFile(scene.get(), path);
        scene->SetFilePath(path);
        currentScene = scene.get();
        mOwnedScenes.push_back(std::move(scene));
        currentScene->Init();
    }

    void SceneSystem::Awake()
    {
        MTS_ASSERT(currentScene != nullptr);
        currentScene->Awake();
    }

    void SceneSystem::Start()
    {
        MTS_ASSERT(currentScene != nullptr);
        currentScene->Start();
    }

    void SceneSystem::Update(float dt)
    {
        MTS_ASSERT(currentScene != nullptr);
        currentScene->Update(dt);
    }

    void SceneSystem::LateUpdate(float dt)
    {
        MTS_ASSERT(currentScene != nullptr);
        currentScene->LateUpdate(dt);
    }

    void SceneSystem::FixedUpdate(float dt)
    {
        MTS_ASSERT(currentScene != nullptr);
        currentScene->FixedUpdate(dt);
    }

    void SceneSystem::Render()
    {
        MTS_ASSERT(currentScene != nullptr);
        currentScene->Render();
    }

    void SceneSystem::DebugRender()
    {
        MTS_ASSERT(currentScene != nullptr);
        currentScene->DebugRender();
    }

    void SceneSystem::Shutdown()
    {
        mOwnedScenes.clear();
        currentScene = nullptr;
        mRegisteredScenes.clear();
        mPendingSceneIndex = -1;
    }

    void SceneSystem::SaveScene(Scene* scene, const std::string& fileName)
    {
        SerializationUtility::SerializeToFile(scene, fileName);
    }

    void SceneSystem::LoadScene(Scene* scene, const std::string& fileName)
    {
        SerializationUtility::DeserializeFromFile(scene, fileName);
        scene->SetFilePath(fileName);
    }

    void SceneSystem::OpenScene(const std::string& path)
    {
        LoadScene(currentScene, path);
        currentScene->Init();
    }

    bool SceneSystem::CreateScene(const std::string& path)
    {
        namespace fs = std::filesystem;
        fs::create_directories(fs::path(FileManager::Resolve(path)).parent_path());
        Scene empty;
        return SerializationUtility::SerializeToFile(&empty, path);
    }

    void SceneSystem::SceneCleanUp()
    {
        MTS_ASSERT(currentScene != nullptr);
        currentScene->DestroyGameObjects();
    }

    // --- Scene registry ---

    void SceneSystem::RegisterScene(const std::string& path)
    {
        mRegisteredScenes.push_back(path);
    }

    void SceneSystem::UnregisterScene(int index)
    {
        if (index < 0 || index >= static_cast<int>(mRegisteredScenes.size()))
            return;
        mRegisteredScenes.erase(mRegisteredScenes.begin() + index);
    }

    void SceneSystem::SwitchScene(int index)
    {
        if (index < 0 || index >= static_cast<int>(mRegisteredScenes.size()))
        {
            LOG_WARN("SceneSystem::SwitchScene — index {} out of range", index);
            return;
        }
        mPendingSceneIndex = index;
    }

    void SceneSystem::LoadRegistry(const std::string& path)
    {
        if (path.empty() || !FileManager::Exists(path))
            return;

        JsonDocument doc;
        if (!doc.LoadFromFile(FileManager::Resolve(path)))
        {
            LOG_WARN("SceneSystem::LoadRegistry — failed to load {}", path);
            return;
        }

        JsonValue root = doc.Root();
        if (!root.Has("scenes") || !root["scenes"].IsArray())
            return;

        JsonValue arr = root["scenes"];
        mRegisteredScenes.clear();
        for (size_t i = 0; i < arr.Size(); ++i)
            mRegisteredScenes.push_back(arr[i].GetString());
    }

    void SceneSystem::SaveRegistry(const std::string& path)
    {
        namespace fs = std::filesystem;
        fs::create_directories(fs::path(FileManager::Resolve(path)).parent_path());

        JsonDocument doc;
        doc.BeginArray("scenes");
        for (const auto& s : mRegisteredScenes)
            doc.WriteString(s.c_str());
        doc.EndArray();

        if (!doc.SaveToFile(FileManager::Resolve(path)))
            LOG_WARN("SceneSystem::SaveRegistry — failed to save {}", path);
    }

    void SceneSystem::ProcessPendingSceneSwitch()
    {
        if (!sInPlayMode || mPendingSceneIndex < 0)
            return;

        MTS_ASSERT(currentScene != nullptr);

        const std::string& targetPath = mRegisteredScenes[mPendingSceneIndex];
        mPendingSceneIndex = -1;

        currentScene->ClearGameObjects();
        sInPlayMode = false;  // prevent AddGameObject from firing lifecycle on partially-deserialized GOs
        SerializationUtility::DeserializeFromFile(currentScene, targetPath);
        sInPlayMode = true;
        currentScene->StartPlay();

        // PhysicsLayer subscribes to this delegate and calls PhysicsSystem2D::Reset(),
        // clearing stale prevContacts before the next FixedUpdate.
        onChangeSceneDelegate.Invoke();
    }

}  // namespace mts
