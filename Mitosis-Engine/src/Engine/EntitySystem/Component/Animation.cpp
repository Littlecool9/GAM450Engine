/**
 * @file Animation.cpp
 * @author Dong Ta
 * @brief  Funcionality Animation component
 * @version 0.1
 * @date 2/7/2026 6:55:28 AM
 *
 * @copyright Copyright (c) 2025 - Mitosis Engine
 *
 */
#include "pch.h"
#include "Animation.h"

#include "SpriteRenderer.h"
#include "Engine/EntitySystem/GameObject/GameObject.h"

namespace mts
{
    Animation::Animation()
    {
        // no op comment
    }

    void Animation::Deserialize(JsonValue const& jsonObj)
    {
        if (jsonObj.Has("FrameIndex"))
        {
            mFrameIndex = jsonObj["FrameIndex"].GetInt();
        }
        if (jsonObj.Has("FrameCount"))
        {
            mFrameCount = jsonObj["FrameCount"].GetInt();
        }
        if (jsonObj.Has("FrameDelay"))
        {
            mFrameDelay = jsonObj["FrameDelay"].GetFloat();
        }
        if (jsonObj.Has("FrameDuration"))
        {
            mFrameDuration = jsonObj["FrameDuration"].GetFloat();
        }
        if (jsonObj.Has("IsRunning"))
        {
            mIsRunning = jsonObj["IsRunning"].GetInt();
        }
        if (jsonObj.Has("IsLooping"))
        {
            mIsLooping = jsonObj["IsLooping"].GetInt();
        }
    }
    void Animation::Serialize(JsonDocument& doc) const
    {
        doc.WriteString("Type", "Animation");
        doc.BeginObject("Data");

        doc.WriteInt("FrameIndex", mFrameIndex);
        doc.WriteInt("FrameCount", mFrameCount);

        doc.WriteFloat("FrameDelay", mFrameDelay);
        doc.WriteFloat("FrameDuration", mFrameDuration);

        doc.WriteInt("IsRunning", mIsRunning ? 1 : 0);
        doc.WriteInt("IsLooping", mIsLooping ? 1 : 0);

        doc.EndObject();
    }

    void Animation::LateUpdate(float dt)
    {
        // no op
    }

    void Animation::Init()
    {
        // no op
    }

    // Play a simple animation sequence (0 .. frameCount).
    // Params:
    //	 animation = Pointer to the animation object.
    //	 frameCount = The number of frames in the sequence.
    //	 frameDuration = The amount of time to wait between frames (in seconds).
    //	 isLooping = True if the animation loops, false otherwise.
    void Animation::AnimationPlay(int startFrame,
                                  int frameCountC,
                                  float frameDurationC,
                                  bool isLoopingC)
    {
        mFrameCount = frameCountC;
        mFrameDuration = frameDurationC;
        mIsLooping = isLoopingC;
        mIsRunning = true;
        mFrameIndex = startFrame;
        mStartFrame = startFrame;
        mFrameDelay = mFrameDuration;
        IsDone = false;
        GetGameObject()->GetComponent<SpriteRenderer>()->SetFrame(mFrameIndex);
    }

    void Animation::AnimationStop()
    {
        mIsRunning = false;
        IsDone = true;
        mFrameIndex = mFrameCount - 1;
    }

    void Animation::Update(float dt)
    {
        IsDone = false;
        if (mIsRunning)
        {
            mFrameDelay -= dt;
            if (mFrameDelay <= 0)  // if framedelay equal or less than 0, go to advance frame
            {                      // ADVANCE FRAME SECTION
                mFrameIndex += 1;  // increment frameIndex by 1

                // check if index greater than framecount
                if (mFrameIndex - mStartFrame >= mFrameCount)
                {
                    if (mIsLooping)  // check if loop is true
                    {
                        mFrameIndex = mStartFrame;
                        IsDone = true;
                    }
                    else
                    {
                        mFrameIndex = mFrameCount - 1;
                        mIsRunning = false;
                        IsDone = true;
                    }
                }
                if (mIsRunning)  // check if its running
                {
                    GetGameObject()->GetComponent<SpriteRenderer>()->SetFrame(mFrameIndex);
                    mFrameDelay += mFrameDuration;
                }
                else
                {
                    mFrameDelay = 0;
                }
            }
        }
    }

    bool Animation::AnimationIsDone() const
    {
        return IsDone;
    }
    Component* Animation::Clone(GameObject* newOwner) const
    {
        Animation* c = new Animation(*this);
        c->InitFromClone(newOwner);
        return c;
    }
}  // namespace mts