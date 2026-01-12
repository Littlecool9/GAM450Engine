/**
 * @file Mesh.h
 * @author sumin.park
 * @brief  Mesh class which has geometric info
 * @version 0.1
 * @date 1/8/2026 12:48:36 PM
 *
 * @copyright Copyright (c) 2025 - RM Engine
 *
 */

#pragma once
#include <glad/glad.h>

#include "Engine/Math/Vec2.h"
namespace rm
{
	struct Vertex {
        math::Vec2 pos;
	};

	class Mesh {
	public:
        GLuint vao = 0, vbo = 0, ibo = 0;
        GLsizei indexCount = 0;
	};

} // rm namespace