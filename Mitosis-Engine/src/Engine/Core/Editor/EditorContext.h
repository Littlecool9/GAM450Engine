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

namespace mts
{
    struct EditorContext
    {
        UUID selected = UUID::InvalidID;
        bool showEditorUI = true;
    };
}  // namespace mts