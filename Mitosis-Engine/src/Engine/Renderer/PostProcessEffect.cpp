/**
 * @file PostProcessEffect.cpp
 * @author Jackey Cen
 * @brief Post-processing effect implementations
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#include "pch.h"
#include "PostProcessEffect.h"

#include <cmath>
#include "Engine/Resource/ResourceManager.h"
#include "Engine/Resource/Shader.h"
#include "Engine/Serialization/JsonAbstraction.h"

namespace mts
{
    // -----------------------------------------------------------------------
    // Shader resource paths (engine-side)
    // -----------------------------------------------------------------------
    static const char* kColorTintShader = "Engine/Resources/PP_ColorTint.shader";
    static const char* kBCSShader = "Engine/Resources/PP_BCS.shader";
    static const char* kGaussianBlurShader = "Engine/Resources/PP_GaussianBlur.shader";
    static const char* kMotionBlurShader = "Engine/Resources/PP_MotionBlur.shader";

    // -----------------------------------------------------------------------
    // Factory
    // -----------------------------------------------------------------------
    std::unique_ptr<PostProcessEffect> CreatePostProcessEffect(const char* typeName)
    {
        std::string t(typeName);
        if (t == "ColorTint")
            return std::make_unique<ColorTintEffect>();
        if (t == "BrightnessContrastSaturation")
            return std::make_unique<BCSEffect>();
        if (t == "GaussianBlur")
            return std::make_unique<GaussianBlurEffect>();
        if (t == "MotionBlur")
            return std::make_unique<MotionBlurEffect>();
        if (t == "CustomShader")
            return std::make_unique<CustomShaderEffect>();

        LOG_WARN("Unknown post-process effect type: {}", typeName);
        return nullptr;
    }

    // =======================================================================
    // ColorTintEffect
    // =======================================================================
    Shader* ColorTintEffect::GetShader(int) const
    {
        return ResourceManager::GetShader(kColorTintShader);
    }

    void ColorTintEffect::SetUniforms(Shader* shader, int, int, int) const
    {
        shader->SetInt("screenTexture", 0);
        shader->SetVector3f("tintColor", color);
    }

    void ColorTintEffect::Deserialize(JsonValue const& json)
    {
        if (json.Has("Color"))
            color = json["Color"].GetVec3(color);
        if (json.Has("Enabled"))
            enabled = json["Enabled"].GetBool(enabled);
    }

    void ColorTintEffect::Serialize(JsonDocument& doc) const
    {
        doc.WriteString("Effect", "ColorTint");
        doc.WriteVec3("Color", color);
        doc.WriteBool("Enabled", enabled);
    }

    // =======================================================================
    // BCSEffect
    // =======================================================================
    Shader* BCSEffect::GetShader(int) const
    {
        return ResourceManager::GetShader(kBCSShader);
    }

    void BCSEffect::SetUniforms(Shader* shader, int, int, int) const
    {
        shader->SetInt("screenTexture", 0);
        shader->SetFloat("brightness", brightness);
        shader->SetFloat("contrast", contrast);
        shader->SetFloat("saturation", saturation);
    }

    void BCSEffect::Deserialize(JsonValue const& json)
    {
        if (json.Has("Brightness"))
            brightness = json["Brightness"].GetFloat(brightness);
        if (json.Has("Contrast"))
            contrast = json["Contrast"].GetFloat(contrast);
        if (json.Has("Saturation"))
            saturation = json["Saturation"].GetFloat(saturation);
        if (json.Has("Enabled"))
            enabled = json["Enabled"].GetBool(enabled);
    }

    void BCSEffect::Serialize(JsonDocument& doc) const
    {
        doc.WriteString("Effect", "BrightnessContrastSaturation");
        doc.WriteFloat("Brightness", brightness);
        doc.WriteFloat("Contrast", contrast);
        doc.WriteFloat("Saturation", saturation);
        doc.WriteBool("Enabled", enabled);
    }

    // =======================================================================
    // GaussianBlurEffect
    // =======================================================================
    Shader* GaussianBlurEffect::GetShader(int) const
    {
        return ResourceManager::GetShader(kGaussianBlurShader);
    }

    void GaussianBlurEffect::ComputeWeights(float* outWeights, int maxWeights) const
    {
        int r = (radius < 1) ? 1 : (radius > maxWeights - 1 ? maxWeights - 1 : radius);
        float s = (sigma > 0.0f) ? sigma : 1.0f;

        float sum = 0.0f;
        for (int i = 0; i <= r; ++i)
        {
            float w =
                std::exp(-static_cast<float>(i * i) / (2.0f * s * s));
            outWeights[i] = w;
            sum += (i == 0) ? w : 2.0f * w;
        }
        // Normalise
        for (int i = 0; i <= r; ++i)
            outWeights[i] /= sum;
        // Zero out the rest
        for (int i = r + 1; i < maxWeights; ++i)
            outWeights[i] = 0.0f;
    }

    void GaussianBlurEffect::SetUniforms(Shader* shader, int passIndex, int texWidth,
                                          int texHeight) const
    {
        shader->SetInt("screenTexture", 0);

        // Pass 0 = horizontal, pass 1 = vertical
        glm::vec2 dir = (passIndex == 0) ? glm::vec2(1.0f, 0.0f) : glm::vec2(0.0f, 1.0f);
        shader->SetVector2f("direction", dir);

        int r = (radius < 1) ? 1 : (radius > 15 ? 15 : radius);
        shader->SetInt("radius", r);

        float weights[16]{};
        ComputeWeights(weights, 16);
        GLuint loc = glGetUniformLocation(shader->GetID(), "weights");
        glUniform1fv(loc, 16, weights);

        glm::vec2 texelSize(1.0f / static_cast<float>(texWidth),
                            1.0f / static_cast<float>(texHeight));
        shader->SetVector2f("texelSize", texelSize);
    }

    void GaussianBlurEffect::Deserialize(JsonValue const& json)
    {
        if (json.Has("Radius"))
            radius = json["Radius"].GetInt(radius);
        if (json.Has("Sigma"))
            sigma = json["Sigma"].GetFloat(sigma);
        if (json.Has("Enabled"))
            enabled = json["Enabled"].GetBool(enabled);
    }

    void GaussianBlurEffect::Serialize(JsonDocument& doc) const
    {
        doc.WriteString("Effect", "GaussianBlur");
        doc.WriteInt("Radius", radius);
        doc.WriteFloat("Sigma", sigma);
        doc.WriteBool("Enabled", enabled);
    }

    // =======================================================================
    // MotionBlurEffect
    // =======================================================================
    Shader* MotionBlurEffect::GetShader(int) const
    {
        return ResourceManager::GetShader(kMotionBlurShader);
    }

    void MotionBlurEffect::SetUniforms(Shader* shader, int, int, int) const
    {
        shader->SetInt("screenTexture", 0);
        shader->SetInt("prevFrameTexture", 1);
        shader->SetFloat("strength", strength);
    }

    void MotionBlurEffect::Deserialize(JsonValue const& json)
    {
        if (json.Has("Strength"))
            strength = json["Strength"].GetFloat(strength);
        if (json.Has("Enabled"))
            enabled = json["Enabled"].GetBool(enabled);
    }

    void MotionBlurEffect::Serialize(JsonDocument& doc) const
    {
        doc.WriteString("Effect", "MotionBlur");
        doc.WriteFloat("Strength", strength);
        doc.WriteBool("Enabled", enabled);
    }

    // =======================================================================
    // CustomShaderEffect
    // =======================================================================
    Shader* CustomShaderEffect::GetShader(int) const
    {
        if (shaderResource.empty())
            return nullptr;
        return ResourceManager::GetShader(shaderResource);
    }

    void CustomShaderEffect::SetUniforms(Shader* shader, int, int, int) const
    {
        shader->SetInt("screenTexture", 0);

        for (auto const& p : floatParams)
            shader->SetFloat(p.name.c_str(), p.value);
        for (auto const& p : vec3Params)
            shader->SetVector3f(p.name.c_str(), p.value);
        for (auto const& p : intParams)
            shader->SetInt(p.name.c_str(), p.value);
    }

    void CustomShaderEffect::Deserialize(JsonValue const& json)
    {
        if (json.Has("ShaderResource"))
            shaderResource = json["ShaderResource"].GetString("");
        if (json.Has("Enabled"))
            enabled = json["Enabled"].GetBool(enabled);

        // Params are stored as arrays: [ { "Name": "x", "Value": 1.0 }, ... ]
        if (json.Has("FloatParams") && json["FloatParams"].IsArray())
        {
            JsonValue arr = json["FloatParams"];
            for (size_t i = 0; i < arr.Size(); ++i)
            {
                JsonValue item = arr[i];
                FloatParam p;
                p.name = item["Name"].GetString("");
                p.value = item["Value"].GetFloat(0.0f);
                if (!p.name.empty())
                    floatParams.push_back(p);
            }
        }
        if (json.Has("Vec3Params") && json["Vec3Params"].IsArray())
        {
            JsonValue arr = json["Vec3Params"];
            for (size_t i = 0; i < arr.Size(); ++i)
            {
                JsonValue item = arr[i];
                Vec3Param p;
                p.name = item["Name"].GetString("");
                p.value = item["Value"].GetVec3({});
                if (!p.name.empty())
                    vec3Params.push_back(p);
            }
        }
        if (json.Has("IntParams") && json["IntParams"].IsArray())
        {
            JsonValue arr = json["IntParams"];
            for (size_t i = 0; i < arr.Size(); ++i)
            {
                JsonValue item = arr[i];
                IntParam p;
                p.name = item["Name"].GetString("");
                p.value = item["Value"].GetInt(0);
                if (!p.name.empty())
                    intParams.push_back(p);
            }
        }
    }

    void CustomShaderEffect::Serialize(JsonDocument& doc) const
    {
        doc.WriteString("Effect", "CustomShader");
        doc.WriteString("ShaderResource", shaderResource.c_str());
        doc.WriteBool("Enabled", enabled);

        if (!floatParams.empty())
        {
            doc.BeginArray("FloatParams");
            for (auto const& p : floatParams)
            {
                doc.BeginObject();
                doc.WriteString("Name", p.name.c_str());
                doc.WriteFloat("Value", p.value);
                doc.EndObject();
            }
            doc.EndArray();
        }
        if (!vec3Params.empty())
        {
            doc.BeginArray("Vec3Params");
            for (auto const& p : vec3Params)
            {
                doc.BeginObject();
                doc.WriteString("Name", p.name.c_str());
                doc.WriteVec3("Value", p.value);
                doc.EndObject();
            }
            doc.EndArray();
        }
        if (!intParams.empty())
        {
            doc.BeginArray("IntParams");
            for (auto const& p : intParams)
            {
                doc.BeginObject();
                doc.WriteString("Name", p.name.c_str());
                doc.WriteInt("Value", p.value);
                doc.EndObject();
            }
            doc.EndArray();
        }
    }

}  // namespace mts
