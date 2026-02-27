#pragma once
#include "Engine/Audio/AudioManager.h"
#include "Engine/Audio/AudioTypes.h"
#include "Engine/Conductor/ConductorSystem.h"
#include "StubAudioBackend.h"
#include <gtest/gtest.h>

class EngineTestFixture : public ::testing::Test
{
protected:
    mts::AudioManager    mAudio;
    mts::ConductorSystem mConductor;

    void SetUp() override
    {
        mts::AudioConfig cfg{};
        mAudio.Init(std::make_unique<mts::StubAudioBackend>(), cfg);
        mts::AudioManager::SetManager(&mAudio);
        mts::ConductorSystem::SetInstance(&mConductor);
        mConductor.Init();
    }

    void TearDown() override
    {
        mConductor.Shutdown();
        mAudio.Shutdown();
        mts::ConductorSystem::SetInstance(nullptr);
        mts::AudioManager::SetManager(nullptr);
    }
};
