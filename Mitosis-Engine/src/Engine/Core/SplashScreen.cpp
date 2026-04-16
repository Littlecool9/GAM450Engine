/**
 * @file SplashScreen.cpp
 * @author sumin.park
 * @brief  Splash screen sequence runner and JSON loader.
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#include "pch.h"
#include "SplashScreen.h"

#include "Engine/Audio/AudioManager.h"
#include "Engine/Input/Input.h"
#include "Engine/Renderer.h"
#include "Engine/Resource/ResourceManager.h"
#include "Engine/Serialization/JsonAbstraction.h"
#include "Engine/Time/Time.h"
#include "Engine/Windows/Window.h"
#include <glm/gtc/matrix_transform.hpp>

namespace mts
{
    void SplashScreen::Run(const std::vector<SplashScreenConfig>& configs,
                           Window& window,
                           bool& isRunning)
    {
        if (configs.empty())
            return;

        Mesh* mesh = ResourceManager::GetMesh("Engine/Resources/DefaultMeshResource.mesh");
        Shader* shader =
            ResourceManager::GetShader("Engine/Resources/DefaultShaderResource.shader");

        if (!mesh || !shader)
        {
            LOG_WARN("SplashScreen: failed to load default mesh or shader, skipping splash.");
            return;
        }

        // Start splashscreen music for the entire sequence
        AudioHandle musicHandle{};
        AudioManager& audio = AudioManager::Get();
        if (audio.isInitialized())
        {
            audio.SetCategoryVolume(AudioCategory::UI, 1.0f);
            musicHandle = audio.Play("event:/splashscreen");
        }

        bool skipFlag = false;

        for (const auto& config : configs)
        {
            if (!isRunning)
                break;
            if (skipFlag)
                break;

            Texture* texture = ResourceManager::GetTexture(config.texturePath);
            if (!texture)
            {
                LOG_WARN("SplashScreen: failed to load texture '{}', skipping entry.",
                         config.texturePath);
                continue;
            }

            const float totalTime =
                config.fadeInDuration + config.holdDuration + config.fadeOutDuration;
            float elapsed = 0.0f;
            bool done = false;

            while (isRunning && !done && !skipFlag)
            {
                // --- Timing ---
                Time::BeginFrame();
                const float dt = static_cast<float>(Time::FrameDt());
                elapsed += dt;

                if (elapsed >= totalTime)
                    done = true;

                // --- Input: skip on any key press or mouse click ---
                Input::BeginFrame();
                window.PollEvents();

                if (mts::Input::IsKeyPressed(mts::Key::Space) ||
                    mts::Input::IsKeyPressed(mts::Key::Escape) ||
                    mts::Input::IsKeyPressed(mts::Key::Enter) ||
                    mts::Input::IsMousePressed(mts::MouseButton::Left) ||
                    mts::Input::IsMousePressed(mts::MouseButton::Right) ||
                    mts::Input::IsGamepadButtonPressed(mts::GamepadButton::A) ||
                    mts::Input::IsGamepadButtonPressed(mts::GamepadButton::Start) ||
                    mts::Input::IsGamepadButtonPressed(mts::GamepadButton::B) ||
                    mts::Input::IsGamepadButtonPressed(mts::GamepadButton::X) ||
                    mts::Input::IsGamepadButtonPressed(mts::GamepadButton::Y))
                {
                    skipFlag = true;
                }

                // --- Compute alpha ---
                float alpha = 1.0f;
                if (elapsed < config.fadeInDuration)
                    alpha = elapsed / config.fadeInDuration;
                else if (elapsed >= config.fadeInDuration + config.holdDuration)
                    alpha = 1.0f - (elapsed - config.fadeInDuration - config.holdDuration) /
                                       config.fadeOutDuration;
                alpha = std::clamp(alpha, 0.0f, 1.0f);

                // --- Full-screen model matrix (screen-space units) ---
                glm::vec4 vp = Renderer::GetViewportInfo();
                float w = vp.z / PixelsPerUnit;
                float h = vp.w / PixelsPerUnit;

                glm::mat4 model =
                    glm::translate(glm::mat4(1.0f), glm::vec3(w * 0.5f, h * 0.5f, 0.0f));
                model = glm::scale(model, glm::vec3(w, h, 1.0f));

                // --- Render ---
                RenderCommand cmd;
                cmd.shader = shader;
                cmd.texture = texture;
                cmd.mesh = mesh;
                cmd.model = model;
                cmd.tintColor = {1.0f, 1.0f, 1.0f};
                cmd.alpha = alpha;
                cmd.hasAlpha = true;
                cmd.isUI = true;
                cmd.sortKey = 0.0f;

                if (audio.isInitialized())
                    audio.Update(dt);

                window.ClearScreen();
                Renderer::BeginFrame();
                Renderer::Submit(cmd);
                Renderer::Flush();
                Renderer::EndFrame();
                window.SwapBuffer();
                Time::EndFrame();
            }
        }

        // Stop splashscreen music when sequence ends
        if (musicHandle)
        {
            audio.Stop(musicHandle, StopMode::AllowFadeOut);
            audio.Release(musicHandle);
        }
    }

    std::vector<SplashScreenConfig> SplashScreen::LoadFromJson(const std::string& path)
    {
        std::vector<SplashScreenConfig> configs;

        JsonDocument doc;
        if (!doc.LoadFromFile(path))
        {
            LOG_WARN("SplashScreen::LoadFromJson: failed to load '{}'.", path);
            return configs;
        }

        JsonValue root = doc.Root();
        if (!root.Has("splashScreens"))
        {
            LOG_WARN("SplashScreen::LoadFromJson: no 'splashScreens' array in '{}'.", path);
            return configs;
        }

        JsonValue arr = root["splashScreens"];
        for (size_t i = 0; i < arr.Size(); ++i)
        {
            JsonValue entry = arr[i];
            SplashScreenConfig cfg;
            cfg.texturePath = entry["texturePath"].GetString();
            cfg.fadeInDuration = entry["fadeInDuration"].GetFloat(0.5f);
            cfg.holdDuration = entry["holdDuration"].GetFloat(1.5f);
            cfg.fadeOutDuration = entry["fadeOutDuration"].GetFloat(0.5f);
            configs.push_back(cfg);
        }

        return configs;
    }

}  // namespace mts
