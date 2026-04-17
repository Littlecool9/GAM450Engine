/**
 * @file JsonAbstraction.cpp
 * @author Dong Ta
 * @brief  Definitions for the Json wrapper.
 * @version 0.1
 * @date 1/8/2026 7:06:27 AM
 *
 * @copyright Copyright (c) 2026 DigiPen (USA) Corporation
 *
 */
#include "pch.h"
#include "JsonAbstraction.h"

#include <filesystem>
#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>
#include <rapidjson/writer.h>
#include <rapidjson/ostreamwrapper.h>
#include <rapidjson/prettywriter.h>
#include <vector>

#include "FileManager.h"

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
        std::string json;
        if (!FileManager::ReadAllText(filepath, json))
            return false;

        impl_->doc.Parse<pf::kParseCommentsFlag | pf::kParseTrailingCommasFlag |
                         pf::kParseEscapedApostropheFlag>(json.c_str());

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

    void JsonDocument::BeginArray()
    {
        auto* parent = impl_->stack.empty() ? nullptr : impl_->stack.back();
        auto& alloc = impl_->doc.GetAllocator();
        // Case A: first array in the document (root)
        if (!parent)
        {
            impl_->doc.SetArray();
            impl_->stack.push_back(&impl_->doc);
            return;
        }
        // Case B: we are inside an array -> append a new array element
        if (parent->IsArray())
        {
            parent->PushBack(RjValue(rapidjson::kArrayType), alloc);
            RjValue& child = (*parent)[parent->Size() - 1];
            impl_->stack.push_back(&child);
            return;
        }
        // Case C: parent is an object -> you must use BeginArray(key)
        assert(false && "BeginArray() called with object parent. Use BeginArray(key).");
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

    void JsonDocument::WriteBool(const char* key, bool value)
    {
        auto* obj = impl_->stack.back();
        auto& alloc = impl_->doc.GetAllocator();
        assert(obj && obj->IsObject());
        rapidjson::Value k;
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

    void JsonDocument::WriteVec2(const char* key, const glm::vec2 value)
    {
        BeginArray(key);
        WriteFloat(value.x);
        WriteFloat(value.y);
        EndArray();
    }

    void JsonDocument::WriteVec3(const char* key, const glm::vec3 value)
    {
        BeginArray(key);
        WriteFloat(value.x);
        WriteFloat(value.y);
        WriteFloat(value.z);
        EndArray();
    }

    void JsonDocument::WriteString(const char* value)
    {
        auto& alloc = impl_->doc.GetAllocator();
        RjValue v;
        v.SetString(value, alloc);
        impl_->stack.back()->PushBack(v.Move(), alloc);
    }

    bool JsonDocument::WriteJson(const char* key, const std::string& json)
    {
        auto* obj = impl_->stack.back();
        auto& alloc = impl_->doc.GetAllocator();
        assert(obj && obj->IsObject());

        // Parse into a temporary Document so RapidJSON owns its own memory
        rapidjson::Document tmp;
        tmp.Parse(json.c_str());
        if (tmp.HasParseError())
            return false;

        rapidjson::Value k;
        k.SetString(key, alloc);

        // Deep-copy parsed value into this document's allocator
        rapidjson::Value v;
        v.CopyFrom(tmp, alloc);

        obj->AddMember(k.Move(), v.Move(), alloc);
        return true;
    }

    bool JsonDocument::WriteRawJsonObject(const char* key, const std::string& jsonObject)
    {
        rapidjson::Document tmp;
        tmp.Parse(jsonObject.c_str());
        if (tmp.HasParseError() || !tmp.IsObject())
            return false;

        return WriteJson(key, jsonObject);
    }

    bool JsonDocument::SaveToFile(const std::string& path)
    {
        rapidjson::StringBuffer buffer;
        rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
        writer.SetIndent(' ', 4);
        writer.SetFormatOptions(rapidjson::kFormatDefault);

        impl_->doc.Accept(writer);

        return FileManager::WriteAllText(path, buffer.GetString(), true);
    }

    bool JsonDocument::SaveToString(std::string& outStr)
    {
        rapidjson::StringBuffer buffer;
        rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);

        if (!impl_->doc.Accept(writer))
            return false;

        outStr = buffer.GetString();
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
        MTS_ASSERT(index <= std::numeric_limits<rapidjson::SizeType>::max());
        return JsonValue(&(*v)[static_cast<rapidjson::SizeType>(index)]);
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

    glm::vec3 JsonValue::GetVec3(glm::vec3 def)
    {
        auto* v = static_cast<const RjValue*>(impl);
        if (!v || !v->IsArray() || v->Size() != 3)
        {
            return def;
        }

        const auto& x = (*v)[0];
        const auto& y = (*v)[1];
        const auto& z = (*v)[2];

        if (!x.IsNumber() || !y.IsNumber() || !z.IsNumber())
        {
            return def;
        }

        return {x.GetFloat(), y.GetFloat(), z.GetFloat()};
    }

    glm::vec2 JsonValue::GetVec2(glm::vec2 def)
    {
        auto* v = static_cast<const RjValue*>(impl);
        if (!v || !v->IsArray() || v->Size() != 2)
        {
            return def;
        }

        const auto& x = (*v)[0];
        const auto& y = (*v)[1];

        if (!x.IsNumber() || !y.IsNumber())
        {
            return def;
        }

        return {x.GetFloat(), y.GetFloat()};
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

    bool JsonValue::GetBool(bool def) const
    {
        auto* v = static_cast<const RjValue*>(impl);
        if (!v || !v->IsBool())
        {
            return def;
        }
        return v->GetBool();
    }

    std::string JsonValue::ToJsonString() const
    {
        if (!impl)
            return "{}";

        auto* v = static_cast<const rapidjson::Value*>(impl);

        rapidjson::StringBuffer buffer;
        rapidjson::Writer writer(buffer);
        v->Accept(writer);

        return buffer.GetString();
    }
}  // namespace mts