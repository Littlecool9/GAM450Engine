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
        const char* GetString(const char* def = "") const;

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
        void WriteString(const char* key, const char* value);

        void WriteFloat(float value);  // array version
        void WriteInt(int value);      // array version
        void WriteString(const char* value);  // array version

        bool SaveToFile(const std::string& path);


       private:
        struct Impl;                  // opaque (defined in .cpp)
        std::unique_ptr<Impl> impl_;  // owns RapidJSON document inside .cpp
    };
}  // namespace rm