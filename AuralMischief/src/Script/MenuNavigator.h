/**
 * @file MenuNavigator.h
 * @brief Reusable keyboard/gamepad/mouse focus + tint highlight script for any
 *        menu scene (main menu, win screen, lose screen, etc.). The focusable
 *        button list and highlight tint are configured entirely from JSON;
 *        navigation order matches the order of UUIDs in the "buttons" array.
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#pragma once
#include <vector>

#include "Engine/Delegate/Delegate.h"
#include "Engine/Scripting/ScriptBase.h"
#include "Engine/UUID/UUID.h"

#include <glm/glm.hpp>

namespace mts
{
    class Button;
    class GameObject;
    class SpriteRenderer;
}  // namespace mts

class MenuNavigator : public mts::ScriptBase
{
   public:
    SCRIPT_CLASS(MenuNavigator)

    void OnEnable() override;
    void OnDisable() override;
    void OnUpdate(float dt) override;

    void DeserializeFields(mts::JsonValue const& fields) override;
    void SerializeFields(mts::JsonDocument& out) const override;

   private:
    struct FocusItem
    {
        mts::Button* button{nullptr};
        mts::SpriteRenderer* sprite{nullptr};
        glm::vec3 originalTint{1.0f};
    };

    struct Sub
    {
        mts::Button* button{nullptr};
        mts::Delegate<>::Id id{0};
    };

    void EnsureResolved();
    void SubscribeAll();
    void UnsubscribeAll();
    void ApplyHighlight();
    void ClearHighlight();
    void SetCursor(int index);
    void StepCursor(int delta);
    void ActivateFocused();

    std::vector<mts::UUID> mButtonUUIDs;
    std::vector<FocusItem> mItems;
    std::vector<Sub> mHoverSubs;

    int mCursor{0};
    bool mResolved{false};
    bool mHighlightApplied{false};

    glm::vec3 mHighlightTint{1.4f, 1.4f, 0.6f};
};
