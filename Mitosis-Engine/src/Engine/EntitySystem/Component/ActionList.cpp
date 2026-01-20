#include "pch.h"
#include "ActionList.h"
#include "Actions/BaseAction.h"
#include "Actions/RotateAction.h"
#include "Actions/ScaleAction.h"
#include "Actions/TranslateAction.h"

namespace mts
{
    ActionList::ActionList(GameObject* owner) : Component(owner)
    {

    }
    void ActionList::Init()
    {
        //actions_.clear();
        Groups.reset();
    }
    void ActionList::LateUpdate(float dt)
    {
    }
    void ActionList::Render()
    {
    }
    void ActionList::Deserialize(JsonValue const& jsonObj)
    {
        
    }
    void ActionList::Serialize(JsonDocument& doc) const
    {
        doc.WriteString("Type", "ActionList");
        doc.EndObject();
    }

    bool ActionList::IsEmpty() const
    {
        return Actions.empty();
    }

    void ActionList::FastForward()
    {
        SpeedUp = true;
        SpeedDown = false;
    }

    void ActionList::SlowDown()
    {
        SpeedDown = true;
        SpeedUp = false;
    }

    void ActionList::ResetSpeed()
    {
        SpeedDown = false;
        SpeedUp = false;
    }

    void ActionList::AddTranslateAction(math::Vec3 Goal, GameObject* target, float TimeToComplete, int id, float delay,
                                        bool block, BaseAction::EaseType ease)
    {
        // create a new TranslateAction and add it to the list
        BaseAction* action =
            new TranslateAction(Goal, target, id, TimeToComplete, delay, block, ease);
        Actions.push_back(action);
    }
    void ActionList::AddRotateAction(math::Vec3 Goal, GameObject* target, float TimeToComplete, int id, float delay,
                                     bool block, BaseAction::EaseType ease)
    {
        // create a new RotateAction and add it to the list
        BaseAction* action =
            new RotateAction(Goal.x, target, id, TimeToComplete, delay, block, ease);
        Actions.push_back(action);
    }

    void ActionList::AddScaleAction(math::Vec3 Goal, GameObject* target, float TimeToComplete, int id, float delay, 
                                    bool block, BaseAction::EaseType ease)
    {
        // create a new ScaleAction and add it to the list
        BaseAction* action = new ScaleAction(Goal, target, id, TimeToComplete, delay, block, ease);
        Actions.push_back(action);
    }

    void ActionList::AddAction(BaseAction* action)
    {
        Actions.push_back(action);
    }

    void ActionList::Update(float dt)
    {
        if (SpeedDown)
        {
            SpeedModifier = Slow;
        }
        else if (SpeedUp)
        {
            SpeedModifier = FastForwardSpeed;
        }
        else
        {
            SpeedModifier = 1;
        }
        if (!Paused)  // nothing gets run if it's in pause
        {
            if (!IsEmpty())  // run if list not empty
            {
                bool block = false;
                for (int i = 0; i < Actions.size(); ++i)
                {
                    if (block == true)  // if there was a previous action that's blocking
                    {
                        if (Groups[Actions[i]->GetGroupId()] ==
                            true)  // check if this action belongs to the blocking action's group
                        {
                            continue;  // don't run if it does
                        }
                    }
                    if (Actions[i]->Update(dt * SpeedModifier) == BaseAction::ActionResult::Success ||
                        Actions[i]->Update(dt* SpeedModifier) ==
                            BaseAction::ActionResult::Failed)  // remove the action if it's done or failed
                    {
                        if (Actions[i]->IsBlocking())  // unblock all the other actions in the same group
                        {
                            block = false;
                            Groups[Actions[i]->GetGroupId()] = false;
                        }
                        BaseAction* obj = Actions[i];
                        Actions.erase(Actions.begin() + i);  // decrement i
                        delete obj;
                        --i;
                    }
                    else
                    {
                        if (Actions[i]->GetDelay() <= 0)  // if it is not in delay
                        {
                            Actions[i]->IncrementTime(dt * SpeedModifier);
                        }
                        if (Actions[i]->IsBlocking() == true)  // if this action is blocking, then just return
                                                    // so the rest dont run the rest of actions
                        {
                            block = true;
                            Groups[Actions[i]->GetGroupId()] = true;
                        }
                    }
                }
                Groups.reset();// reset the groups
            }
        }
    }
}  // namespace mts