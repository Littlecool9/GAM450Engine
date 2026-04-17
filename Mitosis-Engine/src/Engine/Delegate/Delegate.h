/**
 * @file Delegate.h
 * @author Jackey Cen
 * @brief: This is a tool class for C# style delegate implementation.
 * implementation.
 *
 * @copyright Copyright (c) 2026 DigiPen (USA) Corporation
 *
 */

#pragma once

#include <functional>

namespace mts
{
    template <typename... Args>
    class Delegate
    {
       public:
        using Callback = std::function<void(Args...)>;
        using Id = uint32_t;

        Id Subscribe(Callback cb)
        {
            const Id id = nextId++;
            callbacks.emplace(id, std::move(cb));
            return id;
        }

        void Unsubscribe(Id id) { callbacks.erase(id); }

        void Invoke(Args... args)
        {
            // Make a copy to allow safe modification during iteration
            auto copy = callbacks;
            for (auto& [id, cb] : copy)
            {
                cb(args...);
            }
        }

        void Clear() { callbacks.clear(); }

        bool Empty() const { return callbacks.empty(); }

       private:
        Id nextId = 1;
        std::unordered_map<Id, Callback> callbacks;
    };


}  // namespace mts