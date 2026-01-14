/**
 * @file JsonAbstraction.cpp
 * @author Dong Ta
 * @brief  Definitions for the Json wrapper.
 * @version 0.1
 * @date 1/8/2026 7:06:27 AM
 *
 * @copyright Copyright (c) 2025 - RM Engine
 *
 */
#include "pch.h"
#include "JsonAbstraction.h"
#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>
#include <fstream>
namespace mts
{

    using RjValue = rapidjson::Value;

    struct JsonDocument::Impl
    {
        rapidjson::Document doc;
    };

    JsonDocument::JsonDocument() : impl_(std::make_unique<Impl>())
    {
    }
    JsonDocument::~JsonDocument() 
    {
    }

    bool JsonDocument::LoadFromFile(const std::string& filepath) const    
    {
        std::ifstream ifs(filepath, std::ios::in);
        if (!ifs.is_open())
        {
            return false;
        }

        rapidjson::IStreamWrapper isw(ifs);

        using pf = rapidjson::ParseFlag;
        impl_->doc.ParseStream<pf::kParseCommentsFlag | pf::kParseTrailingCommasFlag |
                               pf::kParseEscapedApostropheFlag>(isw);

        return !impl_->doc.HasParseError();
    }

    bool JsonDocument::LoadFromString(const std::string& json) const
    {
        impl_->doc.Parse(json.c_str());
        return !impl_->doc.HasParseError();
    }

    JsonValue JsonDocument::Root() const
    {
        return JsonValue(&impl_->doc);
    }


    bool JsonValue::IsObject() const
    {
        return impl && static_cast<const RjValue*>(impl)->IsObject();
    }

    bool JsonValue::IsArray() const
    {
        return impl && static_cast<const RjValue*>(impl)->IsArray();
    }

    bool JsonValue::Has(std::string_view key) const
    {
        auto* v = static_cast<const RjValue*>(impl);
        return v && v->IsObject() && v->HasMember(key.data());
    }

    JsonValue JsonValue::operator[](std::string_view key) const
    {
        auto* v = static_cast<const RjValue*>(impl);
        return JsonValue(&(*v)[key.data()]);
    }

    JsonValue JsonValue::operator[](size_t index) const
    {
        auto* v = static_cast<const RjValue*>(impl);
        return JsonValue(&(*v)[index]);
    }

    size_t JsonValue::Size() const
    {
        auto* v = static_cast<const RjValue*>(impl);
        if (!v || !v->IsArray())
        {
            return 0;
        }
        return v->Size();
    }

    float JsonValue::GetFloat(float def) const
    {
        auto* v = static_cast<const RjValue*>(impl);
        if (!v || !v->IsNumber())
        {
            return def;
        }

        return v->GetFloat();
    }

    int JsonValue::GetInt(int def) const
    {
        auto* v = static_cast<const RjValue*>(impl);
        if (!v || !v->IsInt())
        {
            return def;
        }

        return v->GetInt();
    }

    const char* JsonValue::GetString(const char* def) const
    {
        auto* v = static_cast<const RjValue*>(impl);
        if (!v || !v->IsString())
        {
            return def;
        }

        return v->GetString();
    }

}  // namespace rm