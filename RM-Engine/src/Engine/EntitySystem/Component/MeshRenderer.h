/**
 * @file MeshRenderer.h
 * @author sumin.park
 * @brief  Renderer Component
 * @version 0.1
 * @date 1/8/2026 11:30:10 AM
 *
 * @copyright Copyright (c) 2025 - RM Engine
 *
 */

#pragma once
#include "Component.h"
#include "Engine/Resource/Material.h"
#include "Engine/Resource/Mesh.h"
namespace rm {
    class MeshRenderer : public Component {
       public:
        MeshRenderer(GameObject* owner);
        ~MeshRenderer() override;

        void Init() override;
        void Update(float dt) override;
        void LateUpdate(float dt) override;
        void Render() override;

        void SetMesh(Mesh* newMesh) { mesh = newMesh; }
        void SetMaterial(Material* newMaterial) { material = newMaterial; }

       private:
        Mesh* mesh = nullptr;
        Material* material = nullptr;
    };

}  // namespace rm