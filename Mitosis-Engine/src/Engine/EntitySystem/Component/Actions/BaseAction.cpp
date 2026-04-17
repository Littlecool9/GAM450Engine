/**
 * @file BaseAction.h
 * @author Dong Ta
 * @brief  Basic Action class for all actions
 * @version 0.1
 * @date 1/8/2026 7:06:27 AM
 *
 * @copyright Copyright (c) 2026 DigiPen (USA) Corporation
 *
 */

#include "pch.h"
#include "BaseAction.h"

namespace mts
{

    constexpr float kHalf              = 0.5f;
    constexpr float kTwo               = 2.0f;
    constexpr float kFour              = 4.0f;


    constexpr float kBounceN           = 7.5625f;
    constexpr float kBounceD           = 2.75f;


    constexpr float kBounceThresh1Mul  = 1.0f;
    constexpr float kBounceThresh2Mul  = 2.0f;
    constexpr float kBounceThresh3Mul  = 2.5f;


    constexpr float kBounceShift1      = 1.5f;
    constexpr float kBounceShift2      = 2.25f;
    constexpr float kBounceShift3      = 2.625f;

    constexpr float kBounceAdd1        = 0.75f;
    constexpr float kBounceAdd2        = 0.9375f;
    constexpr float kBounceAdd3        = 0.984375f;
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
            case EaseType::Linear:
                return p;
            case EaseType::EaseIn:
                return sqrtf(p);
            case EaseType::EaseOut:
                return powf(p, kTwo);
            case EaseType::FastIn:
                return sqrtf(sqrtf(p));
            case EaseType::FastOut:
                return powf(p, kFour);
            case EaseType::InAndOut:
                if (p < kHalf)
                {
                    // powf((p * 2.0f), 2.0f) * 0.5f;
                    return powf(p * kTwo, kTwo) * kHalf;
                }
                else
                {
                    // sqrtf((p - 0.5f) * 2.0f) * 0.5f + 0.5f;
                    return sqrtf((p - kHalf) * kTwo) * kHalf + kHalf;
                }
            case EaseType::Bounce:
            {
                const float n = kBounceN;
                const float d = kBounceD;

                const float threshold1 = kBounceThresh1Mul / d; // 1.0f / d
                const float threshold2 = kBounceThresh2Mul / d; // 2.0f / d
                const float threshold3 = kBounceThresh3Mul / d; // 2.5f / d

                if (p < threshold1)
                {
                    // n * p * p;
                    return n * p * p;
                }
                else if (p < threshold2)
                {
                    // n * (p -= 1.5f / d) * p + 0.75f;
                    const float t = p - (kBounceShift1 / d);
                    return n * t * t + kBounceAdd1;
                }
                else if (p < threshold3)
                {
                    // n * (p -= 2.25f / d) * p + 0.9375f;
                    const float t = p - (kBounceShift2 / d);
                    return n * t * t + kBounceAdd2;
                }
                else
                {
                    // n * (p -= 2.625f / d) * p + 0.984375f;
                    const float t = p - (kBounceShift3 / d);
                    return n * t * t + kBounceAdd3;
                }
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