/**
 * @file EnemyAudioController.cpp
 * @author dream
 * @brief  Shared service script that plays type-appropriate SFX for all enemy types
 *
 * @copyright (c) 2026 DigiPen (USA) Corporation
 *
 */

#include "pch.h"
#include "EnemyAudioController.h"
#include "Engine/Audio/AudioManager.h"
#include "Engine/Helpers/Random.h"

void EnemyAudioController::Init()
{
    mHitSounds[static_cast<int>(mts::EnemyType::FlyEnemy)] = {"event:/drone_hit_1",
                                                              "event:/drone_hit_2",
                                                              "event:/drone_hit_3"};
    mHitSounds[static_cast<int>(mts::EnemyType::SentryEnemy)] = {"event:/sentry_hit_1",
                                                                 "event:/sentry_hit_2",
                                                                 "event:/sentry_hit_3"};
    mHitSounds[static_cast<int>(mts::EnemyType::CrawlEnemy)] = {"event:/slug_hit_1",
                                                                "event:/slug_hit_2",
                                                                "event:/slug_hit_3"};

    mAlertSounds[static_cast<int>(mts::EnemyType::SentryEnemy)] = {"event:/sentry_alert_1",
                                                                   "event:/sentry_alert_2",
                                                                   "event:/sentry_alert_3"};
    mAlertSounds[static_cast<int>(mts::EnemyType::CrawlEnemy)] = {"event:/slug_alert_1",
                                                                  "event:/slug_alert_2",
                                                                  "event:/slug_alert_3"};
}

void EnemyAudioController::PlayHitSound(mts::EnemyType type)
{
    auto it = mHitSounds.find(static_cast<int>(type));
    if (it == mHitSounds.end())
        return;
    mts::AudioManager::Get().Play(it->second[rand() % 3]);
}

mts::AudioHandle EnemyAudioController::PlaySlugBeep()
{
    return mts::AudioManager::Get().Play("event:/fuse");
}

void EnemyAudioController::PlaySlugExplode(mts::AudioHandle fuseHandle)
{
    mts::AudioManager::Get().Stop(fuseHandle);
    mts::AudioManager::Get().Play("event:/explosion");
}

void EnemyAudioController::PlayFlyExplode()
{
    mts::AudioManager::Get().Play("event:/explosion");
}

mts::AudioHandle EnemyAudioController::PlayBossMoan(mts::AudioHandle bossHandle)
{
    int randomIndex = mts::Random::Range(1, 3);
    if (mts::AudioManager::Get().IsPlaying(bossHandle))
    {
        return bossHandle;  // already playing, don't interrupt        
    }
    mts::AudioManager::Get().Stop(bossHandle);
    switch (randomIndex)
    {
        case 1:
            return mts::AudioManager::Get().Play("event:/boss_moan_01");
        case 2:
            return mts::AudioManager::Get().Play("event:/boss_moan_02");
        case 3:
            return mts::AudioManager::Get().Play("event:/boss_moan_03");
        default:
            return mts::AudioManager::Get().Play("event:/boss_moan_01");
    }
}

void EnemyAudioController::PlayAlertSound(mts::EnemyType type)
{
    auto it = mAlertSounds.find(static_cast<int>(type));
    if (it == mAlertSounds.end())
        return;
    mts::AudioManager::Get().Play(it->second[rand() % 3]);
}
