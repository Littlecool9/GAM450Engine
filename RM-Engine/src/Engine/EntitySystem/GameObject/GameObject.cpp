/**
 * @file GameObject.cpp
 * @author sumin.park
 * @brief  GameObject
 * @version 0.1
 * @date 1/8/2026 7:30:10 AM
 *
 * @copyright Copyright (c) 2025 - RM Engine
 *
 */

#include "pch.h"
#include "GameObject.h"
#include "../Component/Component.h"
#include "../Component/Transform.h"

namespace rm
{
    GameObject::GameObject() {
        AddComponent<Transform>();
    }

    GameObject::~GameObject() {
        for (auto c : components)
            delete c;
    }

    void GameObject::Init() {
        for (auto c : components)
            c->Init();
    }

    void GameObject::Update(float dt) {
        for (auto c : components)
            c->Update(dt);
    }

    void GameObject::LateUpdate(float dt) {
        for (auto c : components)
            c->LateUpdate(dt);
    }

    void GameObject::Render() {
        for (auto c : components)
            c->Render();
    }

} // rm namespace