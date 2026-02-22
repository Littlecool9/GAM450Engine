/**
 * @file JsonAbstraction.h
 * @author Dong Ta
 * @brief  Wrapper for JSON value access.
 * @version 0.1
 * @date 1/8/2026 7:06:27 AM
 *
 * @copyright Copyright (c) 2025 - RM Engine
 *
 */
#pragma once
#include <string_view>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

namespace mts
{
    class JsonValue
    {
       public:
        // ---- queries ----
        bool IsObject() const;
        bool IsArray() const;
        bool Has(std::string_view key) const;

        //---- object access ----
        JsonValue operator[](std::string_view key) const;

        //---- array access ----
        size_t Size() const;
        JsonValue operator[](size_t index) const;

        //---- value access ----
        float GetFloat(float def = 0.f) const;
        int GetInt(int def = 0) const;
        glm::vec3 GetVec3(glm::vec3 def = {});
        glm::vec2 GetVec2(glm::vec2 def = {});

        const char* GetString(const char* def = "") const;
        bool GetBool(bool def = false) const;

        std::string ToJsonString() const;

       private:
        friend class RapidJsonBackend;
        friend class JsonDocument;
        explicit JsonValue(const void* impl) : impl(impl) {}

        // opaque pointer to backend value to hide RapidJSON from header
        const void* impl = nullptr;
    };

    class JsonDocument
    {
       public:
        JsonDocument();
        ~JsonDocument();

        bool LoadFromFile(const std::string& filepath) const;

        bool LoadFromString(const std::string& json) const;
        JsonValue Root() const;  // view into the document root

        void BeginObject();
        void BeginObject(const char* key);
        void EndObject();

        void BeginArray(const char* key);
        void EndArray();

        void WriteFloat(const char* key, float value);
        void WriteInt(const char* key, int value);
        void WriteBool(const char* key, bool value);
        void WriteString(const char* key, const char* value);
        void WriteVec2(const char* key, const glm::vec2 value);
        void WriteVec3(const char* key, const glm::vec3 value);

        void WriteFloat(float value);         // array version
        void WriteInt(int value);             // array version
        void WriteString(const char* value);  // array version

        bool WriteJson(const char* key, const std::string& json);
        bool WriteRawJsonObject(const char* key, const std::string& jsonObject);

        bool SaveToFile(const std::string& path);

       private:
        struct Impl;                  // opaque (defined in .cpp)
        std::unique_ptr<Impl> impl_;  // owns RapidJSON document inside .cpp
    };
}  // namespace mts