/**
 * @file Button.cpp
 * @author Dong Ta
 * @brief  Functionality for Button component
 * @version 0.1
 * @date 1/15/2026 6:55:28 AM
 *
 * @copyright Copyright (c) 2025 - Mitosis Engine
 *
 */
#include "pch.h"
#include "Button.h"

#include "Transform.h"
#include "Engine/Application.h"
#include "Engine/EntitySystem/GameObject/GameObject.h"
#include "Engine/Events/EventButton.h"
#include "Engine/Input/Input.h"
#include "Engine/Resource/Mesh.h"
#include "Engine/Events/InputEvent.h"
namespace mts
{
    void Button::TransformCheck()
    {
        if (GetGameObject()->GetComponent<Transform>() == nullptr)
        {
            assert(false && "Transform Component Missing");
        }
    }

    void Button::MeshCheck()
    {
        if (GetGameObject()->GetComponent<Mesh>() == nullptr)
        {
            assert(false && "Mesh Component Missing");
        }
    }

    void Button::Init()
    {
        TransformCheck();
        // TODO: ADD VISUAL FEEDBACK FOR BUTTON CLICK ONCE MESH CAN RETURN COLOR
        // MeshCheck();
        // ButtonColor = GetGameObject()->GetComponent<Mesh>()->GetColor();
    }

    void Button::Deserialize(JsonValue const& jsonObj)
    {
        if (jsonObj.Has("ButtonBoundary"))
        {
            auto bb = jsonObj["ButtonBoundary"];
            ButtonBoundary.x = bb[0].GetFloat(ButtonBoundary.x);
            ButtonBoundary.y = bb[1].GetFloat(ButtonBoundary.y);
        }
        if (jsonObj.Has("OnClickAction"))
        {
            OnClickEvent = jsonObj["OnClickAction"].GetString("");
        }
    }
    void Button::LateUpdate(float dt)
    {
    }
    void Button::Render()
    {
    }
    void Button::Serialize(JsonDocument& doc) const
    {
        doc.WriteString("Type", "Button");
        doc.BeginObject("Data");

        doc.BeginArray("ButtonBoundary");
        doc.WriteFloat(ButtonBoundary.x);
        doc.WriteFloat(ButtonBoundary.y);
        doc.EndArray();

        doc.WriteString("OnClickEvent", OnClickEvent.c_str());
        doc.EndObject();
    }

    void Button::Update(float dt)
    {
        glm::vec2 mousePos{Input::GetMouseX(), Input::GetMouseY()};
        glm::vec3 buttonPos = GetGameObject()->GetComponent<Transform>()->GetPosition();
        if ((mousePos.x >= buttonPos.x - ButtonBoundary.x ||
             mousePos.x <= buttonPos.x - ButtonBoundary.x) &&
            (mousePos.y >= buttonPos.y - ButtonBoundary.y ||
             mousePos.y <= buttonPos.y + buttonPos.y))
        {
            // Clicked
            // TODO: ADD VISUAL FEEDBACK FOR BUTTON CLICK ONCE MESH CAN RETURN COLOR
            if (Input::IsMouseDown(MouseButton::Left))
            {
                // Change Button Color on Click
            }
            if (Input::IsMouseReleased(MouseButton::Left))
            {
                // Execute OnClickAction
                ButtonClickedEvent event(OnClickEvent);
                //Input::OnEvent(event);
                LOG_INFO("Button Clicked! action={}", OnClickEvent.c_str());
            }
        }
    }

}  // namespace mts