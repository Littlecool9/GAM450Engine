/**
 * @file MeshRenderer.h
 * @author sumin.park
 * @brief  Renderer Component
 * @version 0.1
 * @date 1/8/2026 11:30:10 AM
 *
 * @copyright Copyright (c) 2025 - Mitosis Engine
 *
 */

#pragma once
#include "Component.h"
#include "Engine/Resource/Mesh.h"
#include "Engine/Resource/Shader.h"
#include "Engine/Resource/Texture.h"

namespace mts
{
    class MeshRenderer : public Component
    {
       public:
        MeshRenderer(GameObject* owner);
        MeshRenderer() = default;
        ~MeshRenderer() override;

        void Init() override;
        void Update(float dt) override;
        void LateUpdate(float dt) override;
        void Render() override;
        void Deserialize(JsonValue const& jsonObj) override;
        void Serialize(JsonDocument& doc) const override;
        void SetMesh(Mesh* newMesh) { mesh = newMesh; }
        void SetShader(Shader* newShader) { shader = newShader; }
        void SetTexture(Texture* newTexture) { texture = newTexture; }

       private:
        Mesh* mesh = nullptr;
        Shader* shader = nullptr;
        Texture* texture = nullptr;
    };

}  // namespace mts