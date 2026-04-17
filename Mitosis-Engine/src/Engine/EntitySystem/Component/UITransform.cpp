/**
 * @file UITransform.cpp
 * @author sumin.park
 * @brief  transform component for screen space GameObjects
 *
 * @copyright Copyright (c) 2026 DigiPen (USA) Corporation
 *
 */

#include "pch.h"
#include "UITransform.h"

#include <algorithm>
#include <cstring>
#include <glm/ext/matrix_transform.hpp>

namespace mts
{
    // coordinate system is Y-down, (0,0) = top-left, (1,1) = bottom-right.
    glm::vec2 UITransform::AnchorToNorm(UIAnchor a)
    {
        switch (a)
        {
            case UIAnchor::TopLeft:
                return {0.0f, 0.0f};
            case UIAnchor::TopCenter:
                return {0.5f, 0.0f};
            case UIAnchor::TopRight:
                return {1.0f, 0.0f};
            case UIAnchor::MiddleLeft:
                return {0.0f, 0.5f};
            case UIAnchor::Center:
                return {0.5f, 0.5f};
            case UIAnchor::MiddleRight:
                return {1.0f, 0.5f};
            case UIAnchor::BottomLeft:
                return {0.0f, 1.0f};
            case UIAnchor::BottomCenter:
                return {0.5f, 1.0f};
            case UIAnchor::BottomRight:
                return {1.0f, 1.0f};
        }
        return {0.5f, 0.5f};
    }

    const char* UITransform::AnchorToString(UIAnchor a)
    {
        switch (a)
        {
            case UIAnchor::TopLeft:
                return "TopLeft";
            case UIAnchor::TopCenter:
                return "TopCenter";
            case UIAnchor::TopRight:
                return "TopRight";
            case UIAnchor::MiddleLeft:
                return "MiddleLeft";
            case UIAnchor::Center:
                return "Center";
            case UIAnchor::MiddleRight:
                return "MiddleRight";
            case UIAnchor::BottomLeft:
                return "BottomLeft";
            case UIAnchor::BottomCenter:
                return "BottomCenter";
            case UIAnchor::BottomRight:
                return "BottomRight";
        }
        return "Center";
    }

    UIAnchor UITransform::AnchorFromString(const char* s)
    {
        if (!s)
            return UIAnchor::Center;
        if (std::strcmp(s, "TopLeft") == 0)
            return UIAnchor::TopLeft;
        if (std::strcmp(s, "TopCenter") == 0)
            return UIAnchor::TopCenter;
        if (std::strcmp(s, "TopRight") == 0)
            return UIAnchor::TopRight;
        if (std::strcmp(s, "MiddleLeft") == 0)
            return UIAnchor::MiddleLeft;
        if (std::strcmp(s, "MiddleRight") == 0)
            return UIAnchor::MiddleRight;
        if (std::strcmp(s, "BottomLeft") == 0)
            return UIAnchor::BottomLeft;
        if (std::strcmp(s, "BottomCenter") == 0)
            return UIAnchor::BottomCenter;
        if (std::strcmp(s, "BottomRight") == 0)
            return UIAnchor::BottomRight;
        return UIAnchor::Center;
    }

    UIAnchor UITransform::GetAnchor() const
    {
        return mAnchor;
    }
    void UITransform::SetAnchor(UIAnchor a)
    {
        mAnchor = a;
    }
    const glm::vec2& UITransform::GetOffset() const
    {
        return mOffset;
    }
    void UITransform::SetOffset(const glm::vec2& off)
    {
        mOffset = off;
    }
    float UITransform::GetDepth() const
    {
        return mDepth;
    }
    void UITransform::SetDepth(float d)
    {
        mDepth = d;
    }
    const glm::vec3& UITransform::GetScale() const
    {
        return mScale;
    }
    void UITransform::SetScale(const glm::vec3& s)
    {
        mScale = s;
    }
    float UITransform::GetRotation() const
    {
        return mRotation;
    }
    void UITransform::SetRotation(float r)
    {
        mRotation = r;
    }

    glm::vec3 UITransform::GetEffectiveScale() const
    {
        glm::vec2 sf = Renderer::GetUIScaleFactor();
        float uf = std::min(sf.x, sf.y);
        return {mScale.x * uf, mScale.y * uf, mScale.z};
    }

    glm::vec2 UITransform::GetNormalizedCenter() const
    {
        Viewport gv = Renderer::GetGameViewport();
        float vpW =
            (gv.valid ? static_cast<float>(gv.w) : static_cast<float>(Renderer::GetWindowWidth())) /
            PixelsPerUnit;
        float vpH = (gv.valid ? static_cast<float>(gv.h)
                              : static_cast<float>(Renderer::GetWindowHeight())) /
                    PixelsPerUnit;

        glm::vec2 sf = Renderer::GetUIScaleFactor();
        float uf = std::min(sf.x, sf.y);

        glm::vec2 an = AnchorToNorm(mAnchor);
        float ax = an.x * vpW;
        float ay = an.y * vpH;
        float ox = (mOffset.x / PixelsPerUnit) * uf;
        float oy = (mOffset.y / PixelsPerUnit) * uf;

        return {(ax + ox) / vpW, (ay + oy) / vpH};
    }
    glm::mat4 UITransform::GetLocalMatrix()
    {
        Viewport gv = Renderer::GetGameViewport();
        float vpW =
            (gv.valid ? static_cast<float>(gv.w) : static_cast<float>(Renderer::GetWindowWidth())) /
            PixelsPerUnit;
        float vpH = (gv.valid ? static_cast<float>(gv.h)
                              : static_cast<float>(Renderer::GetWindowHeight())) /
                    PixelsPerUnit;

        glm::vec2 sf = Renderer::GetUIScaleFactor();
        float uf = std::min(sf.x, sf.y);

        glm::vec2 an = AnchorToNorm(mAnchor);
        float ax = an.x * vpW;
        float ay = an.y * vpH;

        float ox = (mOffset.x / PixelsPerUnit) * uf;
        float oy = (mOffset.y / PixelsPerUnit) * uf;

        glm::vec3 uiPos(ax + ox, ay + oy, mDepth);
        glm::vec3 scaledS = {mScale.x * uf, mScale.y * uf, mScale.z};

        glm::mat4 S = glm::scale(glm::mat4(1.0f), scaledS);
        glm::mat4 R = glm::rotate(glm::mat4(1.0f), mRotation, glm::vec3(0.0f, 0.0f, 1.0f));
        glm::mat4 T = glm::translate(glm::mat4(1.0f), uiPos);
        mLocalMatrix = T * R * S;
        return mLocalMatrix;
    }

    void UITransform::Serialize(JsonDocument& doc) const
    {
        doc.WriteString("Type", "UITransform");
        doc.BeginObject("Data");

        doc.WriteString("Anchor", AnchorToString(mAnchor));

        doc.BeginArray("Offset");
        doc.WriteFloat(mOffset.x);
        doc.WriteFloat(mOffset.y);
        doc.EndArray();

        doc.WriteFloat("Depth", mDepth);

        doc.BeginArray("Scale");
        doc.WriteFloat(mScale.x);
        doc.WriteFloat(mScale.y);
        doc.WriteFloat(mScale.z);
        doc.EndArray();

        doc.WriteFloat("Rotation", mRotation);

        doc.EndObject();
    }

    void UITransform::Deserialize(JsonValue const& jsonObj)
    {
        if (jsonObj.Has("Anchor"))
        {
            // New format
            mAnchor = AnchorFromString(jsonObj["Anchor"].GetString());

            if (jsonObj.Has("Offset"))
                mOffset = jsonObj["Offset"].GetVec2();

            if (jsonObj.Has("Depth"))
                mDepth = jsonObj["Depth"].GetFloat(mDepth);
        }
        else if (jsonObj.Has("Position"))
        {
            // Legacy: normalized [0,1] Y-down position → TopLeft anchor + pixel offset.
            // Visually identical at any resolution; re-save in editor to upgrade format.
            glm::vec3 pos = jsonObj["Position"].GetVec3();
            mAnchor = UIAnchor::TopLeft;
            mOffset = {pos.x * 1600.0f, pos.y * 900.0f};
            mDepth = pos.z;
        }

        if (jsonObj.Has("Scale"))
            mScale = jsonObj["Scale"].GetVec3();

        if (jsonObj.Has("Rotation"))
            mRotation = jsonObj["Rotation"].GetFloat(mRotation);
    }

    Component* UITransform::Clone(GameObject* newOwner) const
    {
        auto* c = new UITransform(*this);
        c->InitFromClone(newOwner);
        return c;
    }

}  // namespace mts
