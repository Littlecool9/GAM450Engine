
#include "pch.h"
#include "Renderer.h"
#include "EntitySystem/GameObject/GameObject.h"
#include "EntitySystem/Component/Transform.h"
#include "EntitySystem/Component/SpriteRenderer.h"
#include "EntitySystem/Component/Particles/ParticleEmitter.h"

#include <algorithm>

namespace mts
{
    RenderCamera Renderer::ActiveCamera{};

    glm::vec2 Renderer::ScreenToWorld(glm::vec2 screenPos)
    {
        // 1. Screen -> NDC
        GLint viewport[4];
        glGetIntegerv(GL_VIEWPORT, viewport);
        int width = viewport[2];
        int height = viewport[3];
        float x = (2.0f * screenPos.x) / width - 1.0f;
        float y = 1.0f - (2.0f * (height - screenPos.y)) / height;

        glm::vec4 ndcPos(x, y, 0.0f, 1.0f);

        // 2. NDC -> World
        glm::mat4 invVP = glm::inverse(ActiveCamera.viewProj);
        glm::vec4 world = invVP * ndcPos;

        return glm::vec2(world.x, world.y);
    }

    glm::vec2 Renderer::ScreenToWorld(glm::vec3 screenPos)
    {
        return ScreenToWorld(glm::vec2(screenPos.x, screenPos.y));
    }

    glm::vec2 Renderer::WorldToScreen(glm::vec3 worldPos)
    {
        glm::vec2 screenPos = WorldToScreen(glm::vec2(worldPos.x, worldPos.y));
        return screenPos;
    }

    // Returns (x, y, width, height) of the viewport in pixels
    // The returned coordinates are in converted Screen Space, where (0,0) is the middle of the
    // screen
    glm::vec4 Renderer::GetViewportInfo()
    {
        GLint vp[4];
        glGetIntegerv(GL_VIEWPORT, vp);
        float pixelWidth = static_cast<float>(vp[2]);
        float pixelHeight = static_cast<float>(vp[3]);

        float logicWidth = pixelWidth;
        float logicHeight = pixelHeight;

        return glm::vec4(vp[0], -vp[1], logicWidth, logicHeight);
    }

    glm::vec2 Renderer::WorldToScreen(glm::vec2 worldPos)
    {
        // 1. World -> NDC
        glm::vec4 world(worldPos, 0.0f, 1.0f);
        glm::vec4 ndc = ActiveCamera.viewProj * world;
        // 2. NDC -> Screen
        GLint viewport[4];
        glGetIntegerv(GL_VIEWPORT, viewport);
        int width = viewport[2];
        int height = viewport[3];
        float x = (ndc.x / ndc.w + 1.0f) * 0.5f * width;
        float y = (1.0f - ndc.y / ndc.w) * 0.5f * height;  // flip y
        return glm::vec2(x, y);
    }

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

            // particle emitter check
            ParticleEmitter* particleEmitter = object->GetComponent<ParticleEmitter>();
            if (particleEmitter)
            {
                transparentObjects.push_back(object);
                continue;
            }

            // mesh renderer check
            SpriteRenderer* spriteRenderer = object->GetComponent<SpriteRenderer>();
            if (!spriteRenderer)
                continue;
            // transparency/opaque check
            if (!spriteRenderer->HasTextureAlpha())
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
            SpriteRenderer* spriteRenderer = object->GetComponent<SpriteRenderer>();
            spriteRenderer->DrawSprite();
        }

        // Second, draw transparent sprites back to front
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDisable(GL_DEPTH_TEST);
        glDepthMask(GL_FALSE);
        for (auto& object : transparentObjects)
        {
            ParticleEmitter* particleEmitter = object->GetComponent<ParticleEmitter>();
            if (particleEmitter)
                particleEmitter->DrawParticles();
            SpriteRenderer* spriteRenderer = object->GetComponent<SpriteRenderer>();
            if (spriteRenderer)
                spriteRenderer->DrawSprite();
        }

        // Reset GL states
        blendEnabled ? glEnable(GL_BLEND) : glDisable(GL_BLEND);
        depthTestEnabled ? glEnable(GL_DEPTH_TEST) : glDisable(GL_DEPTH_TEST);
        glDepthMask(depthMaskEnabled);
    }
}  // namespace mts