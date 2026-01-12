/**
 * @file Scene.cpp
 * @author sumin.park
 * @brief  Scene that manages game objects
 * @version 0.1
 * @date 1/8/2026 7:42:45 AM
 *
 * @copyright Copyright (c) 2025 - RM Engine
 *
 */

#include "pch.h"
#include "Scene.h"
#include "Engine/EntitySystem/GameObject/GameObject.h"

namespace rm
{
    Scene::~Scene() {
        for (auto g : gameObjects) {
            delete g;
        }
    }

    void Scene::Init() {
        OnInit();
        for (auto g : gameObjects) {
            g->Init();
        }
    }

    void Scene::Update(float dt) {
        for (auto g : gameObjects) {
            g->Update(dt);
        }
    }

    void Scene::LateUpdate(float dt) {
        for (auto g : gameObjects) {
            g->LateUpdate(dt);
        }
    }

    void Scene::Render() {
        for (auto g : gameObjects) {
            g->Render();
        }
    }

    GameObject* Scene::CreateGameObject() {
        GameObject* newGameObject = new GameObject();
        gameObjects.push_back(newGameObject);
        return newGameObject;
    }
} // rm namespace