/**
 * @file AnimationSystem.cpp
 * @author Dong Ta
 * @brief  Functionality for animation system
 * @version 0.1
 * @date 2/19/2026 6:55:28 AM
 *
 * @copyright Copyright (c) 2025 - Mitosis Engine
 *
 */
#include "pch.h"
#include "AnimationSystem.h"
#include "Animation.h"
#include "SpriteRenderer.h"
#include "Engine/EntitySystem/GameObject/GameObject.h"
#include "Engine/Resource/ResourceManager.h"

namespace mts
{
    void AnimationSystem::ChangeState(States NewState)
    {
        if (curStates == NewState)
        {
            return;
        }
        curStates = NewState;

        Info* temp = &dictionary[(curStates)];
        if (temp == nullptr)
        {
            return;
        }
        if (temp->mMesh != nullptr)
        {
            sPtr->SetMesh(temp->mMesh);
            sPtr->SetCols(temp->mMesh->GetNumCols());
            sPtr->SetRows(temp->mMesh->GetNumRows());
            sPtr->SetFrame(temp->frameIndex);
        }

        sPtr->SetTexture(temp->mTexture);

        sPtr->SetShader(temp->mShader);
        if (animPtr == nullptr)
        {
            LOG_INFO("Missing animation component");
            return;
        }
        if (temp->animation)
        {
            animPtr->mFrameDelay = temp->frameDelay;
            animPtr->mFrameIndex = temp->frameIndex;
            animPtr->mIsRunning = temp->isRunning;
            animPtr->AnimationPlay(temp->frameIndex,
                                   temp->frameCount,
                                   temp->frameDuration,
                                   temp->isLooping);
        }
        else
        {
            animPtr->AnimationStop();
        }
    }

    AnimationSystem::AnimationSystem()
    {
    }

    void AnimationSystem::Init()
    {
        animPtr = GetGameObject()->GetComponent<Animation>();
        sPtr = GetGameObject()->GetComponent<SpriteRenderer>();
    }

    void AnimationSystem::Update(float dt)
    {
    }

    void AnimationSystem::Deserialize(JsonValue const& jsonObj)
    {
        // "States" must exist and be an array
        if (!jsonObj.Has("States"))
            return;

        mts::JsonValue states = jsonObj["States"];
        if (!states.IsArray())
            return;

        dictionary.clear();
        /*dictionary.resize(states.Size());*/

        for (size_t i = 0; i < states.Size(); ++i)
        {
            JsonValue s = states[i];

            Info st{};
            if (s.Has("TextureResource"))
            {
                const char* texPath = s["TextureResource"].GetString("");
                st.textPath = texPath;
            }
            st.mTexture = ResourceManager::GetTexture(st.textPath);

            if (s.Has("MeshResource"))
            {
                const char* meshPath = s["MeshResource"].GetString("");
                st.meshPath = meshPath;
            }
            st.mMesh = ResourceManager::GetMesh(st.meshPath);

            if (s.Has("ShaderResource"))
            {
                const char* shaderPath = s["ShaderResource"].GetString("");
                st.shaderPath = shaderPath;
            }
            st.mShader = ResourceManager::GetShader(st.shaderPath);

            // --- int / float fields ---
            if (s.Has("HasAnimation"))
            {
                st.animation = s["HasAnimation"].GetInt(0);  // or GetBool(false) if you change JSON
            }

            if (s.Has("FrameIndex"))
            {
                st.frameIndex = s["FrameIndex"].GetInt(0);
            }

            if (s.Has("FrameCount"))
            {
                st.frameCount = s["FrameCount"].GetInt(0);
            }

            if (s.Has("FrameDelay"))
            {
                st.frameDelay = s["FrameDelay"].GetFloat(0.0f);
            }

            if (s.Has("FrameDuration"))
            {
                st.frameDuration = s["FrameDuration"].GetFloat(0.0f);
            }

            if (s.Has("IsRunning"))
            {
                st.isRunning = s["IsRunning"].GetInt(0) != 0;
            }

            if (s.Has("IsLooping"))
            {
                st.isLooping = s["IsLooping"].GetInt(0) != 0;
            }

            // --- state key / dictionary insert ---
            if (s.Has("State"))
            {
                int loc = s["State"].GetInt(0);
                dictionary[static_cast<States>(loc)] = st;
            }
        }
    }

    void AnimationSystem::Serialize(JsonDocument& doc) const
    {
        doc.WriteString("Type", "AnimationSystem");
        doc.BeginObject("Data");

        doc.BeginArray("States");

        for (auto& [state, info] : dictionary)
        {
            const Info& st = info;

            // One object per state in the array
            doc.BeginObject();  // object inside array -> no key

            doc.WriteInt("State", static_cast<int>(state));

            std::string texPath = st.textPath;
            std::string meshPath = st.meshPath;
            std::string shaderPath = st.shaderPath;

            doc.WriteString("TextureResource", texPath.c_str());
            doc.WriteString("MeshResource", meshPath.c_str());
            doc.WriteString("ShaderResource", shaderPath.c_str());

            doc.WriteInt("HasAnimation", st.animation);
            doc.WriteInt("FrameIndex", st.frameIndex);
            doc.WriteInt("FrameCount", st.frameCount);
            doc.WriteFloat("FrameDelay", st.frameDelay);
            doc.WriteFloat("FrameDuration", st.frameDuration);

            doc.WriteInt("IsRunning", st.isRunning ? 1 : 0);
            doc.WriteInt("IsLooping", st.isLooping ? 1 : 0);

            doc.EndObject();  // end state object
        }

        doc.EndArray();   // end "States"
        doc.EndObject();  // end "Data"
    }

    void AnimationSystem::LateUpdate(float dt)
    {
    }

    bool AnimationSystem::IsDone()
    {
        if (animPtr == nullptr)
        {
            LOG_INFO("Missing animation component");
            return true;
        }
        return animPtr->IsDone;
    }
}  // namespace mts