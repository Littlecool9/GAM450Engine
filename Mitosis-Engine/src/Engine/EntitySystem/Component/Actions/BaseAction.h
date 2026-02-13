/**
 * @file BaseAction.h
 * @author Dong Ta
 * @brief  Base Action class for all actions
 * @version 0.1
 * @date 1/8/2026 7:06:27 AM
 *
 * @copyright Copyright (c) 2025 - Mitosis Engine
 *
 */
#pragma once
#include "pch.h"
#include "Engine/EntitySystem/GameObject/GameObject.h"
namespace mts
{
    class BaseAction
    {
        MTS_CLASS_NEW_DELETE();

       public:
        enum class EaseType
        {
            Linear,
            EaseIn,
            EaseOut,
            FastIn,
            FastOut,
            InAndOut,
            Bounce
        };

        enum class ActionResult
        {
            Invalid = -1,
            OnGoing,
            Blocking,
            Success,
            Failed
        };
        virtual ~BaseAction() = default;
        void IncrementTime(float dt);
        float Ease(float p, EaseType type);
        virtual ActionResult Update(float dt) = 0;
        void SetLifeSpan(float lifespan);
        float PercentDone();
        void SetDelay(float delay);
        void SetBlock(bool block);
        void SetTarget(GameObject* tgt);
        void SetGroupID(int id);
        void SetEaseType(EaseType type);
        bool IsBlocking() const;
        int GetGroupId();
        float GetDelay();

       protected:
        float Age = 0;  // how much time has passed

        bool FirstTime = true;

        float LifeSpan = 0;  // how long this thing should live for

        float Percent = 0;

        float Delay = 0;

        bool Block = false;  // if this is true, the action will block all subsequent actions on
                             // the list from running until it's done
        int GroupID;         // the group this action belongs to, used for blocking
        GameObject* Target = nullptr;  // target action will be performed on
        EaseType Easing = EaseType::Linear;
    };
}  // namespace mts
