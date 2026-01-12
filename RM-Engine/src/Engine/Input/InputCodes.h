/**
 * @file InputCodes.h
 * @author rahul
 * @brief // Engine-defined, platform-agnostic key and mouse codes used by the input system.
 *
 * @copyright Copyright (c) 2025 - RM Engine
 *
 */

#pragma once

namespace rm
{
	enum class Key : uint16_t
	{
		Unknown = 0,

		// Letters
		A,
		B,
		C,
		D,
		E,
		F,
		G,
		H,
		I,
		J,
		K,
		L,
		M,
		N,
		O,
		P,
		Q,
		R,
		S,
		T,
		U,
		V,
		W,
		X,
		Y,
		Z,

		// Numbers
		D0,
		D1,
		D2,
		D3,
		D4,
		D5,
		D6,
		D7,
		D8,
		D9,

		// Controls
		Space,
		Enter,
		Escape,
		Tab,
		Backspace,

		// Arrows
		Left,
		Right,
		Up,
		Down,

		// Modifiers
		LeftShift,
		RightShift,
		LeftCtrl,
		RightCtrl,
		LeftAlt,
		RightAlt,
	};

	enum class MouseButton : uint8_t
	{
		Left = 0,
		Right = 1,
		Middle = 2,
		Button4 = 3,
		Button5 = 4,
		Unknown = 255
	};

} // rm namespace