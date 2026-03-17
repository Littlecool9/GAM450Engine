#include "pch.h"
#include "FollowOffsetScript.h"

#include "Engine/EntitySystem/Component/Transform.h"
#include "Engine/Serialization/JsonAbstraction.h"
#include "Engine/SceneSystem/SceneSystem.h"
#include "Engine/SceneSystem/Scene.h"

void FollowOffsetScript::OnCreate()
{
    mTransform = GetGameObject()->GetComponent<mts::Transform>();

    if (mTargetId.IsValid())
        if (auto* scene = mts::SceneSystem::GetCurrentScene())
            mTarget = scene->FindGameObjectByUUID(mTargetId);
}

void FollowOffsetScript::OnLateUpdate(float /*dt*/)
{
    if (!mTransform || !mTarget)
        return;

    auto* targetTransform = mTarget->GetComponent<mts::Transform>();
    if (!targetTransform)
        return;

    mTransform->SetPosition(targetTransform->GetPosition() + mOffset);
}

void FollowOffsetScript::DeserializeFields(mts::JsonValue const& fields)
{
    if (fields.Has("TargetUUID"))
        mTargetId = mts::UUID::FromString(fields["TargetUUID"].GetString());
    if (fields.Has("OffsetX"))
        mOffset.x = fields["OffsetX"].GetFloat(mOffset.x);
    if (fields.Has("OffsetY"))
        mOffset.y = fields["OffsetY"].GetFloat(mOffset.y);
    if (fields.Has("OffsetZ"))
        mOffset.z = fields["OffsetZ"].GetFloat(mOffset.z);
}

void FollowOffsetScript::SerializeFields(mts::JsonDocument& out) const
{
    out.WriteString("TargetUUID", mTargetId.IsValid() ? mTargetId.ToString().c_str() : "");
    out.WriteFloat("OffsetX", mOffset.x);
    out.WriteFloat("OffsetY", mOffset.y);
    out.WriteFloat("OffsetZ", mOffset.z);
}

void FollowOffsetScript::GetInspectorFields(std::vector<mts::ScriptField>& fields)
{
    fields.push_back({.name = "Target",
                      .type = mts::ScriptField::Type::GameObject,
                      .ptr = {.go = &mTarget},
                      .uuid = &mTargetId});
    fields.push_back({.name = "Offset X",
                      .type = mts::ScriptField::Type::Float,
                      .ptr = {.f = &mOffset.x},
                      .step = 0.1f});
    fields.push_back({.name = "Offset Y",
                      .type = mts::ScriptField::Type::Float,
                      .ptr = {.f = &mOffset.y},
                      .step = 0.1f});
    fields.push_back({.name = "Offset Z",
                      .type = mts::ScriptField::Type::Float,
                      .ptr = {.f = &mOffset.z},
                      .step = 0.1f});
}
