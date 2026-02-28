/**
 * @file EditorContext.h
 * @author sumin.park
 * @brief  context for editor
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#pragma once
#include "Engine/UUID/UUID.h"
#include <string>
#include <imgui.h>
#include <glm/glm.hpp>

namespace mts
{
    enum class PlayState
    {
        Edit,
        Play
    };

    struct EditorCameraState
    {
        glm::vec2 position = {0.0f, 0.0f};
        float zoom = 1.0f;  // multiplier on PixelsPerUnit (80.0f)
    };

    struct EditorContext
    {
        UUID selected = UUID::InvalidID;
        bool showEditorUI = true;
        PlayState playState = PlayState::Edit;
        bool requestPlay = false;
        bool requestStop = false;

        // Editor camera & tool state
        EditorCameraState editorCamera;
        ImVec2 viewportPos = {0, 0};
        ImVec2 viewportSize = {0, 0};
        bool viewportHovered = false;
    };
}  // namespace mts