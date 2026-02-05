/**
 * @file AudioEmitter.cpp
 * @author Dong Ta
 * @brief Functionality for audio emitter
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */
#include "pch.h"
#include "AudioEmitter.h"

#include "Engine/Audio/AudioManager.h"
#include "Engine/EntitySystem/Component/Transform.h"
#include "Engine/EntitySystem/GameObject/GameObject.h"


namespace mts
{
    Transform* AudioEmitter::GetTransform() const
    {
        return GetGameObject()->GetComponent<Transform>();
    }

    AudioEmitter::AudioEmitter()
    {
        //no op comment 
    }

    AudioEmitter::AudioEmitter(GameObject* owner): Component(owner)
    {
        //no op comment 
    }

    AudioEmitter::~AudioEmitter()
    {
        if (handle)
        {
            // Let the sound fade out if you want; or Immediate then change stop mode
            AudioManager::Get().Stop(handle, StopMode::AllowFadeOut);
            AudioManager::Get().Release(handle);
            handle = {};
            isPlaying = false;
        }
    }

    void AudioEmitter::Render()
    {
        //no op comment 
    }

    void AudioEmitter::LateUpdate(float dt)
    {
        //no op comment 
    }

    void AudioEmitter::Deserialize(JsonValue const& jsonObj)
    {

        if (jsonObj.Has("EventId"))
        {
            eventId = jsonObj["EventId"].GetString();
        }

        if (jsonObj.Has("PositionOffset"))
        {
            positionOffset = jsonObj["PositionOffset"].GetVec2();
        }

        if (jsonObj.Has("StartVolume"))
        {
            startVolume = jsonObj["StartVolume"].GetFloat(startVolume);
        }

        if (jsonObj.Has("StartPitch"))
        {
            startPitch = jsonObj["StartPitch"].GetFloat(startPitch);
        }

        if (jsonObj.Has("MaxAudibleDistance"))
        {
            maxAudibleDistance = jsonObj["MaxAudibleDistance"].GetFloat(maxAudibleDistance);
        }
        if (jsonObj.Has("PlayOnStart"))
        {
            playOnStart = jsonObj["PlayOnStart"].GetInt();
        }
        if (jsonObj.Has("Spatialized2D"))
        {
            spatialized2D = jsonObj["Spatialized2D"].GetInt();
        }
        if (jsonObj.Has("IsOneShot"))
        {
            isOneShot = jsonObj["IsOneShot"].GetInt();
        }
    }

    void AudioEmitter::Serialize(JsonDocument& json) const
    {
        json.WriteString("Type", "AudioEmitter");
        json.BeginObject("Data");


        json.WriteString("EventId", eventId.c_str());

        json.BeginArray("PositionOffset");
        json.WriteFloat(positionOffset.x);
        json.WriteFloat(positionOffset.y);
        json.EndArray();

        json.WriteFloat("MaxAudibleDistance", maxAudibleDistance);
        json.WriteFloat("StartVolume", startVolume);
        json.WriteFloat("StartPitch", startPitch);
        json.WriteInt("PlayOnStart", playOnStart);
        json.WriteInt("Spatialized2D", spatialized2D);
        json.WriteInt("IsOneShot", isOneShot);
        json.EndObject();  // end Data
    }

    //TODO: Potentially change this to get world position depending on how we do things
    glm::vec2 AudioEmitter::Get2DPosition() const
    {
        if (auto* t = GetTransform())
        {
            //change from 3d to 2d
            glm::vec3 p3 = t->GetPosition();
            glm::vec2 p2{p3.x, p3.y};
            return p2 + positionOffset;
        }
        return positionOffset;
    }

    void AudioEmitter::Init()
    {
        if (playOnStart && !eventId.empty())
        {
            Play();
        }
        else
        {
            // Initialize prev pos so vel doesn’t spike on first update
            mPrevPos = Get2DPosition();
            mFirstFrame = true;
        }
    }
    void AudioEmitter::Update(float dt)
    {
        if (!spatialized2D || !isPlaying || !handle)
            return;

        glm::vec2 currentPos = Get2DPosition();
        glm::vec2 vel{0.f, 0.f};
        if (!mFirstFrame && dt > 0.f)
        {
            vel = (currentPos - mPrevPos) / dt;
        }

        glm::vec2 listenerPos = AudioManager::Get().GetListener2D();
        float dist = glm::length(currentPos - listenerPos);
        float t = dist / maxAudibleDistance;
        AudioManager::Get().SetInstance2D(handle, currentPos, vel);
        AudioManager::Get().SetParameter(handle, "Distance", t);
        LOG_INFO("Distance between listener and emitter: {}", dist);
        mPrevPos = currentPos;
        mFirstFrame = false;
    }

    void AudioEmitter::Play()
    {
        if (eventId.empty())
            return;

        if (spatialized2D)
        {
            glm::vec2 pos = Get2DPosition();
            handle = AudioManager::Get().Play2D(eventId, pos);
        }
        else
        {
            handle = AudioManager::Get().Play(eventId);
        }

        isPlaying = static_cast<bool>(handle);

        // Apply initial params
        if (isPlaying && handle)
        {
            AudioManager::Get().SetParameter(handle, "Volume", startVolume);  
        }

        // Reset motion state
        mPrevPos = Get2DPosition();
        mFirstFrame = true;

        //if oneshot, call play oneshot and dont bother updating position
        if (isOneShot)
        {
            AudioManager::Get().PlayOneShot2D(eventId, Get2DPosition());
        }
    }
    void AudioEmitter::Stop(StopMode mode)
    {
        if (!handle)
            return;

        AudioManager::Get().Stop(handle, mode);

        if (isOneShot)
        {
            AudioManager::Get().Release(handle);
            handle = {};
        }

        isPlaying = false;
    }
    void AudioEmitter::SetVolume(float value)
    {
        startVolume = value;
        if (isPlaying && handle)
        {
            AudioManager::Get().SetParameter(handle, "Volume", value);
        }
    }
    void AudioEmitter::SetPitch(float value)
    {
        startPitch = value;
        if (isPlaying && handle)
        {
            AudioManager::Get().SetParameter(handle, "Pitch", value);
        }
    }

}