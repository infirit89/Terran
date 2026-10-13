#pragma once

#include "ScriptFieldType.h"

#include <LibScene/Entity.h>

#include <LibCore/Base.h>
#include <LibCore/UUID.h>
#include "Variant.h"

#include <cstddef>
#include <cstdint>
#include <string>
#include <unordered_map>

namespace Coral {

class Type;
class FieldInfo;

}

namespace Terran::Script {

struct ScriptField final {
    ScriptFieldType Type = ScriptFieldType::None;
    std::string Name;
    bool IsArray = false;
};

struct ScriptObject final {
    void const* Handle;
    int32_t Padding[2];
};

struct ScriptArray final {
    void const* Handle;
    int32_t Rank;
    int32_t FieldHandle;
};

template<typename TArg, size_t TIndex>
void AddToIndexArrayI(int32_t* indicesArray, TArg value)
{
    indicesArray[TIndex] = value;
}

template<typename... TArgs, size_t... TIndices>
void AddToIndexArray(int32_t* indicesArray, TArgs&&... args, std::index_sequence<TIndices...> const&)
{
    (AddToIndexArrayI<TArgs, TIndices>(indicesArray, std::forward<TArgs>(args)), ...);
}

template<typename... TIndices>
concept indices_concept = sizeof...(TIndices) > 0;

class ScriptEngine;

class ScriptInstance final {
public:
    ScriptInstance(ScriptEngine* script_engine, Coral::Type const& type, Core::UUID const& id);
    ~ScriptInstance();

    template<typename TValue>
    void SetFieldValue(int32_t fieldHandle, TValue const& value) const
    {
        set_field_value_internal(fieldHandle, (void*)(&value));
    }

    template<>
    void SetFieldValue<std::string>(int32_t fieldHandle, std::string const& value) const;

    template<>
    void SetFieldValue<Core::UUID>(int32_t fieldHandle, Terran::Core::UUID const& value) const;

    template<>
    void SetFieldValue<Utils::Variant>(int32_t fieldHandle, Utils::Variant const& value) const;

    template<typename TReturn>
    TReturn GetFieldValue(int32_t fieldHandle) const
    {
        TReturn value;
        field_value_internal(fieldHandle, &value);
        return value;
    }

    template<>
    std::string GetFieldValue<std::string>(int32_t fieldHandle) const;

    template<>
    Core::UUID GetFieldValue<Core::UUID>(int32_t fieldHandle) const;

    template<>
    Utils::Variant GetFieldValue<Utils::Variant>(int32_t fieldHandle) const;

    template<typename TValue, std::same_as<int32_t>... TIndices>
    requires indices_concept<TIndices...>
    void SetFieldArrayValue(ScriptArray const& array, TValue const& value, TIndices... indices)
    {
        constexpr size_t indicesSize = sizeof...(indices);
        int32_t indicesArray[indicesSize] {};
        AddToIndexArray<TIndices...>(indicesArray, std::forward<TIndices>(indices)..., std::make_index_sequence<indicesSize> {});
        SetFieldArrayValue(array, value, indicesArray, indicesSize);
    }

    template<typename TValue>
    void SetFieldArrayValue(ScriptArray const& array, TValue const& value, int32_t const* indices, size_t indicesSize)
    {
        set_field_array_value_internal(array, (void*)(&value), indices, indicesSize);
    }

    template<>
    void SetFieldArrayValue<std::string>(ScriptArray const& array, std::string const& value, int32_t const* indices, size_t indicesSize);

    template<>
    void SetFieldArrayValue<Core::UUID>(ScriptArray const& array, Core::UUID const& value, int32_t const* indices, size_t indicesSize);

    template<>
    void SetFieldArrayValue<Utils::Variant>(ScriptArray const& array, Utils::Variant const& value, int32_t const* indices, size_t indicesSize);

    template<typename TReturn, std::same_as<int32_t>... TIndices>
    requires indices_concept<TIndices...>
    TReturn GetFieldArrayValue(ScriptArray const& array, TIndices... indices)
    {
        constexpr size_t indicesSize = sizeof...(indices);
        int32_t indicesArray[indicesSize] {};
        AddToIndexArray<TIndices...>(indicesArray, std::forward<TIndices>(indices)..., std::make_index_sequence<indicesSize> {});

        return GetFieldArrayValue<TReturn>(array, indicesArray, indicesSize);
    }

    template<typename TReturn>
    TReturn GetFieldArrayValue(ScriptArray const& array, int32_t const* indices, size_t indicesSize)
    {
        TReturn value;
        field_array_value_internal(array, &value, indices, indicesSize);
        return value;
    }

    template<>
    std::string GetFieldArrayValue(ScriptArray const& array, int32_t const* indices, size_t indicesSize);

    template<>
    Core::UUID GetFieldArrayValue(ScriptArray const& array, int32_t const* indices, size_t indicesSize);

    template<>
    Utils::Variant GetFieldArrayValue(ScriptArray const& array, int32_t const* indices, size_t indicesSize);

    template<std::same_as<int32_t>... TLengths>
    requires indices_concept<TLengths...>
    void ResizeFieldArray(ScriptArray& array, TLengths... lengths)
    {
        constexpr size_t lengthsSize = sizeof...(lengths);
        int32_t lengthsArray[lengthsSize] {};
        AddToIndexArray<TLengths...>(lengthsArray, std::forward<TLengths>(lengths)..., std::make_index_sequence<lengthsSize> {});
        if constexpr (lengthsSize > 1)
            resize_field_array_internal(array, lengthsArray, lengthsSize);
        else
            resize_field_array_internal(array, lengthsArray[0]);
    }

    int32_t GetFieldArrayLength(ScriptArray const& array, int dimension = 0) const;

    ScriptField const& field(int32_t fieldHandle) const { return m_fields.at(fieldHandle); }
    ScriptArray script_array(int32_t fieldHandle);

    void invoke_init() const;
    void invoke_update(float deltaTime) const;
    // void InvokePhysicsUpdate() const;
    // void InvokeCollisionBegin(Entity other) const;
    // void InvokeCollisionEnd(Entity other) const;

    void copy_field_from(int32_t fieldHandle, Core::Shared<ScriptInstance> const& source);
    void copy_all_fields_from(Core::Shared<ScriptInstance> const& source);

    void const* handle() const { return m_context; }

private:
    void set_field_value_internal(int32_t fieldHandle, void* value) const;
    void field_value_internal(int32_t fieldHandle, void* value) const;
    static void set_field_array_value_internal(ScriptArray const& array, void* value, int32_t const* indices, size_t indicesSize);
    static void field_array_value_internal(ScriptArray const& array, void* value, int32_t const* indices, size_t indicesSize);
    void resize_field_array_internal(ScriptArray& array, int32_t length);
    void resize_field_array_internal(ScriptArray& array, int32_t const* lengths, size_t lengthsSize);

private:
    void const* m_context;
    int32_t m_oninit_method_handle;
    int32_t m_onupdate_method_handle;
    // int32_t m_OnPhysicsUpdateMethodHandle;
    // int32_t m_OnCollisionBeginMethodHandle;
    // int32_t m_OnCollisionEndMethodHandle;
    std::unordered_map<int32_t, ScriptField> m_fields;
    mutable std::unordered_map<int32_t, ScriptObject> m_field_objects;
    ScriptEngine* m_script_engine;

    friend class ScriptEngine;
};

}
