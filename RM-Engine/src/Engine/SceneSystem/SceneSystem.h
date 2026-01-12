/**
 * @file SceneSystem.h
 * @author sumin.park
 * @brief  The scene manager
 * @version 0.1
 * @date 1/8/2026 7:40:23 AM
 *
 * @copyright Copyright (c) 2025 - RM Engine
 *
 */

#pragma once

namespace rm
{
    class Scene;
	class SceneSystem
	{
	public:
        static void Init(Scene* initialScene);
        static void Update(float dt);
        static void LateUpdate(float dt);
        static void Render();
        static void Shutdown();
        static void ChangeCurrentScene(Scene* newScene);
	private:
        static Scene* currentScene;
	};

} // rm namespace