
#include "pch.h"
#include <RMEngine.h>
/**
 * @file main.cpp
 * @author sumin.park
 * @brief Main sandbox area.
 *
 * @copyright Copyright (c) 2025 - RM Engine
 *
 */

class Sandbox : public rm::Application
{
public:
	Sandbox() = default;
	~Sandbox() = default;
};

rm::Application *rm::CreateApplication()
{
	return new Sandbox();
}
