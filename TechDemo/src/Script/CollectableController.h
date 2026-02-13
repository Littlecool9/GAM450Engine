/**
 * @file CollectableController.h
 * @author josh.siegel
 * @brief // behavior script for collectable objects (ex. coins/gems)
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#pragma once
#include "Engine/Scripting/ScriptBase.h"

namespace mts
{
    class AudioEmitter;
}

class CollectableController : public mts::ScriptBase
{
   public:
    void OnCreate() override;
    void OnUpdate(float dt) override;
    void DeserializeFields(mts::JsonValue const& fields) override {}

    void OnCollisionEnter2D(const mts::GameObject*) override;
   private:
    bool collected = false;
    mts::AudioEmitter* emitter;
};
