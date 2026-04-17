/**
 * @file SceneSystem.h
 * @author sumin.park
 * @brief  The scene manager
 * @version 0.1
 * @date 1/8/2026 7:40:23 AM
 *
 * @copyright Copyright (c) 2026 DigiPen (USA) Corporation
 *
 */

#pragma once
#include <functional>
#include <memory>
#include "Engine/Delegate/Delegate.h"

namespace mts
{
    class Scene;

    class SceneSystem
    {
       public:
        static void Init(const std::string& path);
        static void Awake();  // play mode only — all Awakes before any Start
        static void Start();  // play mode only — fires once per GO on first activation
        static void Update(float dt);
        static void LateUpdate(float dt);
        static void FixedUpdate(float dt);
        static void Render();
        static void DebugRender();
        static void Shutdown();

        // Scene file operations (editor path)
        static void SaveScene(Scene* scene, const std::string& fileName);
        static void OpenScene(const std::string& path);    // LoadScene + Init — canonical edit-mode entry point
        static bool CreateScene(const std::string& path);  // creates empty scene file on disk

        static void SceneCleanUp();
        static Scene* GetCurrentScene() { return currentScene; }
        static bool IsInPlayMode() { return sInPlayMode; }
        static void SetInPlayMode(bool value) { sInPlayMode = value; }

        // Scene registry — pre-registered paths indexed by integer
        static void RegisterScene(const std::string& path);
        static void UnregisterScene(int index);
        static const std::vector<std::string>& GetRegisteredScenes() { return mRegisteredScenes; }

        // Runtime scene switch (deferred to end of frame — call from scripts)
        static void SwitchScene(int index);

        // Registry persistence
        static void LoadRegistry(const std::string& path);
        static void SaveRegistry(const std::string& path);

        // Called by SceneLayer::OnEndFrame — processes pending SwitchScene
        static void ProcessPendingSceneSwitch();

       public:
        static Delegate<> onChangeSceneDelegate;

       private:
        static void LoadScene(Scene* scene, const std::string& fileName);

        static std::vector<std::unique_ptr<Scene>> mOwnedScenes;
        static std::vector<std::string> mRegisteredScenes;
        static int mPendingSceneIndex;

        static Scene* currentScene;
        static bool sInPlayMode;
    };

}  // namespace mts