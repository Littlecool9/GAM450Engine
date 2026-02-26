/**
 * @file JsonAbstraction.h
 * @author Dong Ta
 * @brief  The interface to implement for deserilzation/serialization support.
 * @version 0.1
 * @date 1/8/2026 7:06:27 AM
 *
 * @copyright Copyright (c) 2025 - RM Engine
 *
 */

#pragma once
// Includes
//#include "rapidjson/document.h"
#include "JsonAbstraction.h"
namespace mts
{
    /*!
     * \ingroup Serialization
     * \class Deserializable
     * \brief
     *    The interface class to inherent and implement for letting your class deserializable.
     *    See pure virtual function Deserialize for more info.
     */
    class Deserializable
    {
       public:  

        virtual void Deserialize(JsonValue const& jsonObj) = 0;

        virtual void Serialize(JsonDocument& doc) const = 0;
    };
}  // namespace mts