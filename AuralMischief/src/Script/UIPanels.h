

#pragma once
#include "Engine/Scripting/ScriptBase.h"
#include "Engine/Delegate/Delegate.h"
#include "Engine/Serialization/JsonAbstraction.h"

namespace mts
{
    class Button;
    class AudioManager;
    class JsonDocument;
    class JsonValue;
}  // namespace mts

class UIPanels : public mts::ScriptBase
{
   public:
    SCRIPT_CLASS(UIPanels);

    void Awake() override;
    void Start() override;
    void DeserializeFields(mts::JsonValue const& /*fields*/) override;
    void OnDisable() override;

    // Public so other UI scripts (e.g. the Resume button on the pause menu)
    // can route through the same pause/unpause code path as ESC and the
    // header pause icon.
    void SetPanelActive(bool active);


    private:
    mts::Button* mButton{nullptr};
    mts::Delegate<>::Id mClickId{0};
    std::vector<mts::UUID> kMemberUUIDs;
    std::vector<mts::GameObject*> mMembers;
    bool mPanelActive{false};
};
