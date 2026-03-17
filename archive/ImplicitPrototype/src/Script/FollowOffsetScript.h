#pragma once
#include "Engine/Scripting/ScriptBase.h"
#include "Engine/EntitySystem/GameObject/GameObject.h"
#include <glm/glm.hpp>

class FollowOffsetScript : public mts::ScriptBase
{
   public:
    void OnCreate() override;
    void OnLateUpdate(float dt) override;

    void DeserializeFields(mts::JsonValue const& fields) override;
    void SerializeFields(mts::JsonDocument& out) const override;
    void GetInspectorFields(std::vector<mts::ScriptField>& fields) override;

    SCRIPT_CLASS(FollowOffsetScript)

   private:
    mts::Transform* mTransform = nullptr;

    mts::GameObject* mTarget = nullptr;              // runtime pointer, resolved from UUID
    mts::UUID mTargetId = mts::UUID::GenerateNil();  // persisted UUID

    glm::vec3 mOffset = {0.f, 1.f, 0.f};  // default: 1 unit above target
};
