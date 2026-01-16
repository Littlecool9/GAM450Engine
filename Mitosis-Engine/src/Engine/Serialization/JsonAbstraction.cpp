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
#include <rapidjson/writer.h>
#include <rapidjson/ostreamwrapper.h>
#include <rapidjson/prettywriter.h>
#include <fstream>
#include <vector>
namespace mts
{

    using RjValue = rapidjson::Value;
    using RjDocument = rapidjson::Document;
    using RJIStreamWrapper = rapidjson::IStreamWrapper;
    using RJOStreamWrapper = rapidjson::OStreamWrapper;
    using pf = rapidjson::ParseFlag;
    using pw = rapidjson::PrettyWriter<RJOStreamWrapper>;

    struct JsonDocument::Impl
    {
        RjDocument doc;
        std::vector<RjValue*> stack;
    };

    JsonDocument::JsonDocument() : impl_(std::make_unique<Impl>())
    {
        impl_->doc.SetObject();
        impl_->stack.clear();
        impl_->stack.push_back(&impl_->doc);
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

        RJIStreamWrapper isw(ifs);

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

    void JsonDocument::BeginObject()
    {
        auto* parent = impl_->stack.empty() ? nullptr : impl_->stack.back();
        auto& alloc = impl_->doc.GetAllocator();

        // Case A: first object in the document (root)
        if (!parent)
        {
            impl_->doc.SetObject();
            impl_->stack.push_back(&impl_->doc);
            return;
        }

        // Case B: we are inside an array -> append a new object element
        if (parent->IsArray())
        {
            parent->PushBack(RjValue(rapidjson::kObjectType), alloc);
            RjValue& child = (*parent)[parent->Size() - 1];
            impl_->stack.push_back(&child);
            return;
        }

        // Case C: parent is an object -> you must use BeginObject(key)
        assert(false && "BeginObject() called with object parent. Use BeginObject(key).");
    }
    void JsonDocument::BeginObject(const char* key)
    {
        RjValue obj(rapidjson::kObjectType);
        RjValue k(key, impl_->doc.GetAllocator());

        impl_->stack.back()->AddMember(k, obj, impl_->doc.GetAllocator());
        impl_->stack.push_back(&(*impl_->stack.back())[key]);
    }
    void JsonDocument::EndObject()
    {
        impl_->stack.pop_back();
    }

    void JsonDocument::BeginArray(const char* key)
    {
        auto* obj = impl_->stack.back();
        auto& alloc = impl_->doc.GetAllocator();

        assert(obj && obj->IsObject());

        RjValue k;
        k.SetString(key, alloc);

        RjValue arr(rapidjson::kArrayType);

        // Move both key and value into the object
        obj->AddMember(k.Move(), arr.Move(), alloc);

        // Get a pointer to the inserted array
        auto it = obj->FindMember(key);
        assert(it != obj->MemberEnd() && it->value.IsArray());

        impl_->stack.push_back(&it->value);
    }

    void JsonDocument::EndArray()
    {
        impl_->stack.pop_back();
    }

    void JsonDocument::WriteFloat(const char* key, float value)
    {
        auto* obj = impl_->stack.back();
        auto& alloc = impl_->doc.GetAllocator();

        assert(obj && obj->IsObject());

        RjValue k;
        k.SetString(key, alloc);

        obj->AddMember(k.Move(), value, alloc);
    }

    void JsonDocument::WriteFloat(float value)
    {
        impl_->stack.back()->PushBack(value, impl_->doc.GetAllocator());
    }

    void JsonDocument::WriteInt(const char* key, int value)
    {
        auto* obj = impl_->stack.back();
        auto& alloc = impl_->doc.GetAllocator();
        assert(obj && obj->IsObject());
        RjValue k;
        k.SetString(key, alloc);
        obj->AddMember(k.Move(), value, alloc);
    }

    void JsonDocument::WriteInt(int value)
    {
        impl_->stack.back()->PushBack(value, impl_->doc.GetAllocator());
    }

    void JsonDocument::WriteString(const char* key, const char* value)
    {
        auto* obj = impl_->stack.back();
        auto& alloc = impl_->doc.GetAllocator();
        assert(obj && obj->IsObject());
        RjValue k;
        k.SetString(key, alloc);
        RjValue v;
        v.SetString(value, alloc);
        obj->AddMember(k.Move(), v.Move(), alloc);
    }

    void JsonDocument::WriteString(const char* value)
    {
        auto& alloc = impl_->doc.GetAllocator();
        RjValue v;
        v.SetString(value, alloc);
        impl_->stack.back()->PushBack(v.Move(), alloc);
    }

    bool JsonDocument::SaveToFile(const std::string& path)
    {
        std::ofstream ofs(path);
        if (!ofs.is_open())
        {
            return false;
        }

        RJOStreamWrapper osw(ofs);

        pw writer(osw);
        writer.SetIndent(' ', 4);
        writer.SetFormatOptions(rapidjson::kFormatDefault);
        impl_->doc.Accept(writer);

        return true;
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

}  // namespace mts