/**
 * @file PauseMenuNavigator.h
 * @author Jackey Cen
 * @brief Keyboard navigation, focus signifier, and return-to-main-menu
 *        confirmation dialog for the pause menu. Lives as a sibling script on
 *        the PauseMenuPanel GameObject and drives all focus state for the
 *        actionable items in the pause menu (volume stepper, resume, return).
 *
 * @copyright (c) 2026 DigiPen (USA) Corporation
 *
 */

#pragma once
#include <vector>

#include "Engine/Scripting/ScriptBase.h"
#include "Engine/Delegate/Delegate.h"
#include "Engine/UUID/UUID.h"

#include <glm/glm.hpp>

namespace mts
{
    class GameObject;
    class Button;
    class SpriteRenderer;
}  // namespace mts

class PauseMenuNavigator : public mts::ScriptBase
{
   public:
    SCRIPT_CLASS(PauseMenuNavigator)

    void Awake() override;
    void OnUpdate(float dt) override;
    void OnDisable() override;

    void DeserializeFields(mts::JsonValue const& fields) override;
    void SerializeFields(mts::JsonDocument& out) const override;
    void GetInspectorFields(std::vector<mts::ScriptField>& fields) override;

    // Called by PauseMenuPanel when the menu is shown/hidden so the cursor and
    // visual highlight can reset cleanly between sessions.
    void OnPanelShown();
    void OnPanelHidden();

    // Called by ReturnToMainMenuButton when the player clicks "Return to Main
    // Menu" so the navigator can present the confirmation dialog instead of
    // switching scenes immediately.
    void ShowReturnConfirm();
    void HideReturnConfirm();

   private:
    enum class Group
    {
        Main,
        Confirm
    };

    // A focus slot can group multiple buttons (the volume stepper has minus
    // and plus inside one slot). Sprites and original tints are kept in
    // parallel so the highlight can be applied/reverted cleanly.
    struct FocusItem
    {
        std::vector<mts::Button*> buttons;
        std::vector<mts::SpriteRenderer*> sprites;
        std::vector<glm::vec3> originalTints;
    };

    // Binding slot indices into the mBindingUUIDs / mBindingGOs arrays.
    // Order must match the JSON "bindings" array in the scene file.
    enum Slot
    {
        VolumeMinus = 0,
        VolumePlus,
        SFXMinus,
        SFXPlus,
        Resume,
        Return,
        ConfirmBg,
        ConfirmPrompt,
        ConfirmYes,
        ConfirmNo,
        SlotCount
    };

    void BuildItems();
    FocusItem MakeItem(mts::GameObject* go);

    void SubscribeHover(FocusItem const& item, Group group, int index);
    void ApplyHighlight();
    void ClearHighlight();
    void SetMainCursor(int index);
    void SetConfirmCursor(int index);
    void ActivateFocused();

    // Resolved focus slots.
    std::vector<FocusItem> mMainItems;     // [SFX-, SFX+, Vol-, Vol+, Resume, Return]
    std::vector<FocusItem> mConfirmItems;  // [Yes, No]

    // GameObjects that make up the confirmation dialog (toggled via SetActive).
    std::vector<mts::GameObject*> mConfirmMembers;

    // Subscription bookkeeping so OnDisable can cleanly unhook everything.
    struct Sub
    {
        mts::Button* button{nullptr};
        mts::Delegate<>::Id id{0};
    };
    std::vector<Sub> mHoverSubs;
    std::vector<Sub> mClickSubs;

    Group mActiveGroup{Group::Main};
    int mMainCursor{0};
    int mConfirmCursor{1};  // default to "No" — the safe choice
    bool mPanelVisible{false};
    bool mHighlightApplied{false};
    bool mGamePadLeftNeutral{true};
    bool mGamePadRightNeutral{true};

    // Highlight tint multiplier applied to the focused slot's sprites.
    glm::vec3 mHighlightTint{1.4f, 1.4f, 0.6f};

    // UUID-based GO bindings configured in the scene JSON "bindings" array.
    // Index order defined by the Slot enum above.
    mts::UUID mBindingUUIDs[SlotCount] = {
        mts::UUID::InvalidID,
        mts::UUID::InvalidID,
        mts::UUID::InvalidID,
        mts::UUID::InvalidID,
        mts::UUID::InvalidID,
        mts::UUID::InvalidID,
        mts::UUID::InvalidID,
        mts::UUID::InvalidID,
        mts::UUID::InvalidID,
        mts::UUID::InvalidID,
    };
    mts::GameObject* mBindingGOs[SlotCount]{};
};
