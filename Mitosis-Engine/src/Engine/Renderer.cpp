
#include "pch.h"
#include "Renderer.h"
#include "EntitySystem/GameObject/GameObject.h"
#include "EntitySystem/Component/Transform.h"
#include "EntitySystem/Component/MeshRenderer.h"

#include <algorithm>

namespace mts
{
    RenderCamera Renderer::ActiveCamera{};

    void Renderer::DrawSprites(Scene* scene)
    {
        // Create a vector of game objects with opaque sprites and one with transparent sprites
        std::vector<GameObject*>& gameObjects = scene->GetGameObjects();
        std::vector<GameObject*> opaqueObjects;
        std::vector<GameObject*> transparentObjects;
        for (auto& object : gameObjects)
        {
            // null check
            if (!object)
                continue;
            // transform check
            Transform* transform = object->GetComponent<Transform>();
            if (!transform)
                continue;
            // mesh renderer check
            MeshRenderer* meshRenderer = object->GetComponent<MeshRenderer>();
            if (!meshRenderer)
                continue;
            // transparency/opaque check
            if (!meshRenderer->HasTextureAlpha())
                opaqueObjects.push_back(object);
            else
                transparentObjects.push_back(object);
        }

        // Sort the sprites
        std::sort(opaqueObjects.begin(),
             opaqueObjects.end(),
             [](GameObject* a, GameObject* b)
             {
                 Transform* aTransform = a->GetComponent<Transform>();
                 Transform* bTransform = b->GetComponent<Transform>();
                 return aTransform->GetPosition().z > bTransform->GetPosition().z;
             });
        std::sort(transparentObjects.begin(),
             transparentObjects.end(),
             [](GameObject* a, GameObject* b)
             {
                 Transform* aTransform = a->GetComponent<Transform>();
                 Transform* bTransform = b->GetComponent<Transform>();
                 return aTransform->GetPosition().z < bTransform->GetPosition().z;
             });


        // Save current GL states
        GLboolean blendEnabled = glIsEnabled(GL_BLEND);
        GLboolean depthTestEnabled = glIsEnabled(GL_DEPTH_TEST);
        GLboolean depthMaskEnabled = GL_TRUE;
        glGetBooleanv(GL_DEPTH_WRITEMASK, &depthMaskEnabled);

        // First, draw opaque sprites front to back
        glDisable(GL_BLEND);
        glEnable(GL_DEPTH_TEST);
        glDepthMask(GL_TRUE);
        for (auto& object : opaqueObjects)
        {
            MeshRenderer* meshRenderer = object->GetComponent<MeshRenderer>();
            meshRenderer->DrawSprite();
        }

        // Second, draw transparent sprites back to front
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDisable(GL_DEPTH_TEST);
        glDepthMask(GL_FALSE);
        for (auto& object : transparentObjects)
        {
            MeshRenderer* meshRenderer = object->GetComponent<MeshRenderer>();
            meshRenderer->DrawSprite();
        }

        // Reset GL states
        blendEnabled ? glEnable(GL_BLEND) : glDisable(GL_BLEND);
        depthTestEnabled ? glEnable(GL_DEPTH_TEST) : glDisable(GL_DEPTH_TEST);
        glDepthMask(depthMaskEnabled);
    }
}