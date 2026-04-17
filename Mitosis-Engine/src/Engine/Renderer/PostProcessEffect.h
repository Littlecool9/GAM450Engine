/**
 * @file PostProcessEffect.h
 * @author Jackey Cen
 * @brief Base class for post-processing effects and built-in effect types
 *
 * @copyright Copyright (c) 2026 DigiPen (USA) Corporation
 *
 */

#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

namespace mts
{
    class Shader;
    class JsonValue;
    class JsonDocument;

    // ---------------------------------------------------------------------------
    // Base class
    // ---------------------------------------------------------------------------
    class PostProcessEffect
    {
       public:
        virtual ~PostProcessEffect() = default;

        virtual const char* GetTypeName() const = 0;

        // Number of rendering passes this effect needs (e.g. GaussianBlur = 2).
        virtual int GetPassCount() const { return 1; }

        // Return the shader to use for the given pass index.
        virtual Shader* GetShader(int passIndex = 0) const = 0;

        // Set effect-specific uniforms. Called right before the fullscreen quad draw.
        virtual void SetUniforms(Shader* shader, int passIndex, int texWidth,
                                 int texHeight) const = 0;

        // If true the stack will bind the previous frame texture to GL_TEXTURE1.
        virtual bool NeedsPreviousFrame() const { return false; }

        virtual void Deserialize(JsonValue const& json) = 0;
        virtual void Serialize(JsonDocument& doc) const = 0;

        bool enabled = true;
    };

    // ---------------------------------------------------------------------------
    // Factory
    // ---------------------------------------------------------------------------
    std::unique_ptr<PostProcessEffect> CreatePostProcessEffect(const char* typeName);

    // ---------------------------------------------------------------------------
    // Built-in effects
    // ---------------------------------------------------------------------------

    class ColorTintEffect : public PostProcessEffect
    {
       public:
        glm::vec3 color{1.0f, 1.0f, 1.0f};

        const char* GetTypeName() const override { return "ColorTint"; }
        Shader* GetShader(int passIndex = 0) const override;
        void SetUniforms(Shader* shader, int passIndex, int texWidth,
                         int texHeight) const override;
        void Deserialize(JsonValue const& json) override;
        void Serialize(JsonDocument& doc) const override;
    };

    class BCSEffect : public PostProcessEffect
    {
       public:
        float brightness = 0.0f;
        float contrast = 1.0f;
        float saturation = 1.0f;

        const char* GetTypeName() const override { return "BrightnessContrastSaturation"; }
        Shader* GetShader(int passIndex = 0) const override;
        void SetUniforms(Shader* shader, int passIndex, int texWidth,
                         int texHeight) const override;
        void Deserialize(JsonValue const& json) override;
        void Serialize(JsonDocument& doc) const override;
    };

    class GaussianBlurEffect : public PostProcessEffect
    {
       public:
        int radius = 3;
        float sigma = 1.5f;

        const char* GetTypeName() const override { return "GaussianBlur"; }
        int GetPassCount() const override { return 2; }
        Shader* GetShader(int passIndex = 0) const override;
        void SetUniforms(Shader* shader, int passIndex, int texWidth,
                         int texHeight) const override;
        void Deserialize(JsonValue const& json) override;
        void Serialize(JsonDocument& doc) const override;

       private:
        void ComputeWeights(float* outWeights, int maxWeights) const;
    };

    class MotionBlurEffect : public PostProcessEffect
    {
       public:
        float strength = 0.3f;

        const char* GetTypeName() const override { return "MotionBlur"; }
        Shader* GetShader(int passIndex = 0) const override;
        void SetUniforms(Shader* shader, int passIndex, int texWidth,
                         int texHeight) const override;
        bool NeedsPreviousFrame() const override { return true; }
        void Deserialize(JsonValue const& json) override;
        void Serialize(JsonDocument& doc) const override;
    };

    class CustomShaderEffect : public PostProcessEffect
    {
       public:
        struct FloatParam
        {
            std::string name;
            float value = 0.0f;
        };
        struct Vec3Param
        {
            std::string name;
            glm::vec3 value{0.0f};
        };
        struct IntParam
        {
            std::string name;
            int value = 0;
        };

        std::string shaderResource;
        std::vector<FloatParam> floatParams;
        std::vector<Vec3Param> vec3Params;
        std::vector<IntParam> intParams;

        const char* GetTypeName() const override { return "CustomShader"; }
        Shader* GetShader(int passIndex = 0) const override;
        void SetUniforms(Shader* shader, int passIndex, int texWidth,
                         int texHeight) const override;
        void Deserialize(JsonValue const& json) override;
        void Serialize(JsonDocument& doc) const override;
    };

}  // namespace mts
