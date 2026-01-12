/**
 * @file Component.h
 * @author sumin.park
 * @brief  Basic Component class
 * @version 0.1
 * @date 1/8/2026 6:55:28 AM
 *
 * @copyright Copyright (c) 2025 - RM Engine
 *
 */

#pragma once

namespace rm
{
    class GameObject;

	class Component
	{
	public:
        Component(GameObject* owner);
        Component(const Component& other) = default;
        virtual ~Component() = default;

		Component& operator=(const Component& other) = default;

	public:
        virtual void Init() = 0;
        virtual void Update(float dt) = 0;
        virtual void LateUpdate(float dt) = 0;
        virtual void Render() = 0;

        GameObject* GetGameObject() { return owner; }
	private:
        GameObject* owner;
	};

} // rm namespace