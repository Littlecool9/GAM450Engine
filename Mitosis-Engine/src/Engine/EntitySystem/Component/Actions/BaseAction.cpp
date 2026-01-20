/**
 * @file BaseAction.h
 * @author Dong Ta
 * @brief  Basic Action class for all actions
 * @version 0.1
 * @date 1/8/2026 7:06:27 AM
 *
 * @copyright Copyright (c) 2025 - Mitosis Engine
 *
 */

#include "pch.h"
#include "BaseAction.h"

namespace mts
{
    float BaseAction::Ease(float p, EaseType type)
    {
        if (p <= 0)
        {
            return 0;
        }
        if (p >= 1)
        {
            return 1;
        }
        switch (type)
        {
            // todo Refine more easing functions and maybe find some solution to magic numbers
            case EaseType::Linear:
                return p;
            case EaseType::EaseIn:
                return sqrtf(p);
            case EaseType::EaseOut:
                return powf(p, 2.0f);
            case EaseType::FastIn:
                return sqrtf(std::sqrt(p));
            case EaseType::FastOut:
                return powf(p, 4.0f);
            case EaseType::InAndOut:
                if (p < 0.5f)
                {
                    return powf(p * 2.0f, 2.0f) * 0.5f;
                }
                else
                {
                    return sqrtf((p - 0.5f) * 2.0f) * 0.5f + 0.5f;
                }
            case EaseType::Bounce:
                float n = 7.5625f;
                float d = 2.75f;
                if (p < 1.0f / d)
                {
                    return n * p * p;
                }
                else if (p < 2.0f / d)
                {
                    return n * (p -= 1.5f / d) * p + 0.75f;
                }
                else if (p < 2.5f / d)
                {
                    return n * (p -= 2.25f / d) * p + 0.9375f;
                }
                else
                {
                    return n * (p -= 2.625f / d) * p + 0.984375f;
                }
        }
        return p;
    }
    int BaseAction::GetGroupId()
    {
        return GroupID;
    }
    float BaseAction::GetDelay()
    {
        return Delay;
    }
    void BaseAction::SetTarget(GameObject* tgt)
    {
        Target = tgt;
    }
    void BaseAction::SetGroupID(int id)
    {
        GroupID = id;
    }
    void BaseAction::SetEaseType(EaseType type)
    {
        Easing = type;
    }
    void BaseAction::SetLifeSpan(float lifespan)
    {
        LifeSpan = lifespan;
    }
    float BaseAction::PercentDone()
    {
        return Percent;
    }
    void BaseAction::SetDelay(float delay)
    {
        Delay = delay;
    }
    void BaseAction::SetBlock(bool block)
    {
        Block = block;
    }
    bool BaseAction::IsBlocking() const
    {
        return Block;
    }
    void BaseAction::IncrementTime(float dt)
    {
        Age += dt;
        if (Age >= LifeSpan)  // if pass 1.f, set to 1
        {
            Age = LifeSpan;
            Percent = 1.f;
        }
        else
        {
            Percent = Age / LifeSpan;
        }
        Percent = Ease(Percent, Easing);
    }

}