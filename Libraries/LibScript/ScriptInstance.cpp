#include "ScriptInstance.h"
#include "ScriptEngine.h"
#include "ScriptTypes.h"
#include "Variant.h"
#include "ScriptFieldType.h"

#include <LibCore/Log.h>
#include <LibCore/UUID.h>

// #include "Utils/Debug/OptickProfiler.h"

#include <Coral/Attribute.hpp>
#include <Coral/ManagedArray.hpp>
#include <Coral/String.hpp>
#include <Coral/Type.hpp>

#include <ranges>
#include <cstdint>
#include <cstddef>
#include <string>

namespace Terran::Script {

ScriptInstance::ScriptInstance(ScriptEngine* script_engine, Coral::Type const& type, Terran::Core::UUID const& id)
    : m_script_engine(script_engine)
{
    m_context = type.CreateInstance(id).GetHandle();

    m_oninit_method_handle = type.GetMethod("Init").GetHandle();
    m_onupdate_method_handle = type.GetMethod<float>("Update").GetHandle();
    // m_OnPhysicsUpdateMethodHandle = type.GetMethod("PhysicsUpdate").GetHandle();
    // m_OnCollisionBeginMethodHandle = type.GetMethod("Void OnCollisionBegin(Entity)").GetHandle();
    // m_OnCollisionEndMethodHandle = type.GetMethod("Void OnCollisionEnd(Entity)").GetHandle();
}

ScriptInstance::~ScriptInstance()
{
    Coral::ManagedObject object = m_context;
    TR_TRACE(SCRIPT_SYSTEM, "destroying script instance");
    for (auto const& [fieldHandle, value] : m_field_objects) {
        ScriptField const& current_field = field(fieldHandle);
        if (current_field.IsArray) {
            ScriptArray const& scriptArray = reinterpret_cast<ScriptArray const&>(value);
            Coral::ManagedArray array(scriptArray.Handle, scriptArray.Rank);
            array.Destroy();
        } else {
            Coral::ManagedObject object = value.Handle;
            object.Destroy();
        }
    }

    object.Destroy();
    m_context = nullptr;
}

void ScriptInstance::field_value_internal(int32_t fieldHandle, void* value) const
{
    Coral::ManagedObject object = m_context;
    object.GetFieldValueByHandleRaw(fieldHandle, value);
}

#define GET_MANAGED_ARRAY()                                                                     \
    Coral::ManagedArray array;                                                                  \
    auto it = m_FieldArrays.find(fieldHandle);                                                  \
    if (it == m_FieldArrays.end()) {                                                            \
        array = object.GetFieldValueByHandle<Coral::ManagedArray>(fieldHandle);                 \
        m_FieldArrays.emplace(fieldHandle, ScriptArray { array.GetHandle(), array.GetRank() }); \
    } else                                                                                      \
        array = Coral::ManagedArray(it->second.Handle, it->second.Rank)

void ScriptInstance::set_field_array_value_internal(const ScriptArray& array, void* value, const int32_t* indices, size_t indicesSize)
{
    Coral::ManagedArray managedArray(array.Handle, array.Rank);
    managedArray.SetValueRaw(indices, indicesSize, value);
}

void ScriptInstance::field_array_value_internal(ScriptArray const& array, void* value, int32_t const* indices, size_t indicesSize)
{
    Coral::ManagedArray managedArray(array.Handle, array.Rank);
    managedArray.GetValueRaw(indices, indicesSize, value);
}

template<>
void ScriptInstance::SetFieldValue<std::string>(int32_t fieldHandle, std::string const& value) const
{
    Coral::String string = Coral::String::New(value);
    set_field_value_internal(fieldHandle, &string);
    Coral::String::Free(string);
}

template<>
void ScriptInstance::SetFieldValue<Terran::Core::UUID>(int32_t fieldHandle, Terran::Core::UUID const& value) const
{
    Coral::ManagedObject object = m_context;
    Coral::ManagedObject entityObject = m_script_engine->create_entity_type_instance(value);
    auto it = m_field_objects.find(fieldHandle);
    if (it == m_field_objects.end())
        m_field_objects.emplace(fieldHandle, ScriptObject { entityObject.GetHandle() });
    else {
        ScriptObject& temp = m_field_objects.at(fieldHandle);
        Coral::ManagedObject tempObject = temp.Handle;
        tempObject.Destroy();
        temp.Handle = entityObject.GetHandle();
    }

    object.SetFieldValueByHandleRaw(fieldHandle, &entityObject);
}

template<>
void ScriptInstance::SetFieldValue<Utils::Variant>(int32_t fieldHandle, Utils::Variant const& value) const
{
    Coral::ManagedObject object = m_context;
    switch (value.GetType()) {
    case Utils::Variant::Type::Int8:
        object.SetFieldValueByHandle<int8_t>(fieldHandle, value);
        return;
    case Utils::Variant::Type::UInt8:
        object.SetFieldValueByHandle<uint8_t>(fieldHandle, value);
        return;
    case Utils::Variant::Type::Int16:
        object.SetFieldValueByHandle<int16_t>(fieldHandle, value);
        return;
    case Utils::Variant::Type::UInt16:
        object.SetFieldValueByHandle<uint16_t>(fieldHandle, value);
        return;
    case Utils::Variant::Type::Int32:
        object.SetFieldValueByHandle<int32_t>(fieldHandle, value);
        return;
    case Utils::Variant::Type::UInt32:
        object.SetFieldValueByHandle<uint32_t>(fieldHandle, value);
        return;
    case Utils::Variant::Type::Int64:
        object.SetFieldValueByHandle<int64_t>(fieldHandle, value);
        return;
    case Utils::Variant::Type::UInt64:
        object.SetFieldValueByHandle<uint64_t>(fieldHandle, value);
        return;
    case Utils::Variant::Type::Float:
        object.SetFieldValueByHandle<float>(fieldHandle, value);
        return;
    case Utils::Variant::Type::Double:
        object.SetFieldValueByHandle<double>(fieldHandle, value);
        return;
    case Utils::Variant::Type::Bool:
        object.SetFieldValueByHandle<bool>(fieldHandle, value);
        return;
    case Utils::Variant::Type::Char:
        object.SetFieldValueByHandle<char>(fieldHandle, value);
        return;
    case Utils::Variant::Type::Vector2:
        object.SetFieldValueByHandle<glm::vec2>(fieldHandle, value);
        return;
    case Utils::Variant::Type::Vector3:
        object.SetFieldValueByHandle<glm::vec3>(fieldHandle, value);
        return;
    case Utils::Variant::Type::Vector4:
        object.SetFieldValueByHandle<glm::vec4>(fieldHandle, value);
        return;
    case Utils::Variant::Type::String:
        SetFieldValue<std::string>(fieldHandle, value);
        return;
    case Utils::Variant::Type::UUID:
        SetFieldValue<Terran::Core::UUID>(fieldHandle, value);
        return;
    default:;
    }

    TR_ASSERT(false, "Unknown variant type");
}

template<>
std::string ScriptInstance::GetFieldValue<std::string>(int32_t fieldHandle) const
{
    // TR_PROFILE_FUNCTION();
    Coral::String string;
    field_value_internal(fieldHandle, &string);
    std::string result = string;
    Coral::String::Free(string);
    return result;
}

template<>
Terran::Core::UUID ScriptInstance::GetFieldValue<Terran::Core::UUID>(int32_t fieldHandle) const
{
    // TR_PROFILE_FUNCTION();
    Coral::ManagedObject object = m_context;
    auto it = m_field_objects.find(fieldHandle);
    Coral::ManagedObject entityObject;
    if (it == m_field_objects.end()) {
        entityObject = object.GetFieldValueByHandle<Coral::ManagedObject>(fieldHandle);

        if (entityObject.GetHandle() == nullptr)
            return {};

        m_field_objects.emplace(fieldHandle, ScriptObject { entityObject.GetHandle() });
    } else
        entityObject = it->second.Handle;

    Terran::Core::UUID id = Terran::Core::UUID::invalid();
    entityObject.GetFieldValueByHandleRaw(m_script_engine->entity_id_field_handle(), &id);
    return id;
}

template<>
Utils::Variant ScriptInstance::GetFieldValue<Utils::Variant>(int32_t fieldHandle) const
{
    Coral::ManagedObject object = m_context;
    switch (m_fields.at(fieldHandle).Type) {
    case ScriptFieldType::Int8:
        return object.GetFieldValueByHandle<int8_t>(fieldHandle);
    case ScriptFieldType::UInt8:
        return object.GetFieldValueByHandle<uint8_t>(fieldHandle);
    case ScriptFieldType::Int16:
        return object.GetFieldValueByHandle<int16_t>(fieldHandle);
    case ScriptFieldType::UInt16:
        return object.GetFieldValueByHandle<uint16_t>(fieldHandle);
    case ScriptFieldType::Int32:
        return object.GetFieldValueByHandle<int32_t>(fieldHandle);
    case ScriptFieldType::UInt32:
        return object.GetFieldValueByHandle<uint32_t>(fieldHandle);
    case ScriptFieldType::Int64:
        return object.GetFieldValueByHandle<int64_t>(fieldHandle);
    case ScriptFieldType::UInt64:
        return object.GetFieldValueByHandle<uint64_t>(fieldHandle);
    case ScriptFieldType::Float:
        return object.GetFieldValueByHandle<float>(fieldHandle);
    case ScriptFieldType::Double:
        return object.GetFieldValueByHandle<double>(fieldHandle);
    case ScriptFieldType::Bool:
        return object.GetFieldValueByHandle<bool>(fieldHandle);
    case ScriptFieldType::Char:
        return object.GetFieldValueByHandle<char>(fieldHandle);
    case ScriptFieldType::Vector2:
        return object.GetFieldValueByHandle<glm::vec2>(fieldHandle);
    case ScriptFieldType::Vector3:
        return object.GetFieldValueByHandle<glm::vec3>(fieldHandle);
    case ScriptFieldType::Color:
        return object.GetFieldValueByHandle<glm::vec4>(fieldHandle);
    case ScriptFieldType::String:
        return GetFieldValue<std::string>(fieldHandle);
    case ScriptFieldType::Entity:
        return GetFieldValue<Terran::Core::UUID>(fieldHandle);
    default:;
    }

    TR_ASSERT(false, "Unknown script field type");
    return {};
}

template<>
void ScriptInstance::SetFieldArrayValue<std::string>(ScriptArray const& array, std::string const& value, int32_t const* indices, size_t indicesSize)
{
    Coral::String string = Coral::String::New(value);
    set_field_array_value_internal(array, &string, indices, indicesSize);
    Coral::String::Free(string);
}

template<>
void ScriptInstance::SetFieldArrayValue(ScriptArray const& array, Terran::Core::UUID const& value, int32_t const* indices, size_t indicesSize)
{
    Coral::ManagedArray managedArray(array.Handle, array.Rank);
    Coral::ManagedObject entityObject = m_script_engine->create_entity_type_instance(value);
    managedArray.SetValueRaw(indices, indicesSize, &entityObject);
}

template<>
void ScriptInstance::SetFieldArrayValue<Utils::Variant>(ScriptArray const& array, Utils::Variant const& value, int32_t const* indices, size_t indicesSize)
{
    Coral::ManagedArray managedArray(array.Handle, array.Rank);
    switch (value.GetType()) {
    case Utils::Variant::Type::Int8:
        managedArray.SetValue<int8_t>(indices, static_cast<int32_t>(indicesSize), value);
        return;
    case Utils::Variant::Type::UInt8:
        managedArray.SetValue<uint8_t>(indices, static_cast<int32_t>(indicesSize), value);
        return;
    case Utils::Variant::Type::Int16:
        managedArray.SetValue<int16_t>(indices, static_cast<int32_t>(indicesSize), value);
        return;
    case Utils::Variant::Type::UInt16:
        managedArray.SetValue<uint16_t>(indices, static_cast<int32_t>(indicesSize), value);
        return;
    case Utils::Variant::Type::Int32:
        managedArray.SetValue<int32_t>(indices, static_cast<int32_t>(indicesSize), value);
        return;
    case Utils::Variant::Type::UInt32:
        managedArray.SetValue<uint32_t>(indices, static_cast<int32_t>(indicesSize), value);
        return;
    case Utils::Variant::Type::Int64:
        managedArray.SetValue<int64_t>(indices, static_cast<int32_t>(indicesSize), value);
        return;
    case Utils::Variant::Type::UInt64:
        managedArray.SetValue<uint64_t>(indices, static_cast<int32_t>(indicesSize), value);
        return;
    case Utils::Variant::Type::Float:
        managedArray.SetValue<float>(indices, static_cast<int32_t>(indicesSize), value);
        return;
    case Utils::Variant::Type::Double:
        managedArray.SetValue<double>(indices, static_cast<int32_t>(indicesSize), value);
        return;
    case Utils::Variant::Type::Bool:
        managedArray.SetValue<bool>(indices, static_cast<int32_t>(indicesSize), value);
        return;
    case Utils::Variant::Type::Char:
        managedArray.SetValue<char>(indices, static_cast<int32_t>(indicesSize), value);
        return;
    case Utils::Variant::Type::Vector2:
        managedArray.SetValue<glm::vec2>(indices, static_cast<int32_t>(indicesSize), value);
        return;
    case Utils::Variant::Type::Vector3:
        managedArray.SetValue<glm::vec3>(indices, static_cast<int32_t>(indicesSize), value);
        return;
    case Utils::Variant::Type::Vector4:
        managedArray.SetValue<glm::vec4>(indices, static_cast<int32_t>(indicesSize), value);
        return;
    case Utils::Variant::Type::String:
        SetFieldArrayValue<std::string>(array, value, indices, static_cast<int32_t>(indicesSize));
        return;
    case Utils::Variant::Type::UUID:
        SetFieldArrayValue<Terran::Core::UUID>(array, value, indices, static_cast<int32_t>(indicesSize));
        return;
    default:;
    }

    TR_ASSERT(false, "Unknown variant type");
}

template<>
std::string ScriptInstance::GetFieldArrayValue(ScriptArray const& array, int32_t const* indices, size_t indicesSize)
{
    Coral::String string;
    field_array_value_internal(array, &string, indices, indicesSize);
    std::string result = string;
    Coral::String::Free(string);
    return result;
}

template<>
Terran::Core::UUID ScriptInstance::GetFieldArrayValue(ScriptArray const& array, int32_t const* indices, size_t indicesSize)
{
    // TR_PROFILE_FUNCTION();
    Coral::ManagedArray managedArray(array.Handle, array.Rank);
    Coral::ManagedObject entityObject;
    managedArray.GetValueRaw(indices, indicesSize, &entityObject);

    if (entityObject.GetHandle() == nullptr)
        return {};

    Terran::Core::UUID id = Terran::Core::UUID::invalid();
    entityObject.GetFieldValueByHandleRaw(m_script_engine->entity_id_field_handle(), &id);
    entityObject.Destroy();
    return id;
}

template<>
Utils::Variant ScriptInstance::GetFieldArrayValue(ScriptArray const& array, int32_t const* indices, size_t indicesSize)
{
    Coral::ManagedArray managedArray(array.Handle, array.Rank);
    switch (m_fields.at(array.FieldHandle).Type) {
    case ScriptFieldType::Int8:
        return managedArray.GetValue<int8_t>(indices, static_cast<int32_t>(indicesSize));
    case ScriptFieldType::UInt8:
        return managedArray.GetValue<uint8_t>(indices, static_cast<int32_t>(indicesSize));
    case ScriptFieldType::Int16:
        return managedArray.GetValue<int16_t>(indices, static_cast<int32_t>(indicesSize));
    case ScriptFieldType::UInt16:
        return managedArray.GetValue<uint16_t>(indices, static_cast<int32_t>(indicesSize));
    case ScriptFieldType::Int32:
        return managedArray.GetValue<int32_t>(indices, static_cast<int32_t>(indicesSize));
    case ScriptFieldType::UInt32:
        return managedArray.GetValue<uint32_t>(indices, static_cast<int32_t>(indicesSize));
    case ScriptFieldType::Int64:
        return managedArray.GetValue<int64_t>(indices, static_cast<int32_t>(indicesSize));
    case ScriptFieldType::UInt64:
        return managedArray.GetValue<uint64_t>(indices, static_cast<int32_t>(indicesSize));
    case ScriptFieldType::Float:
        return managedArray.GetValue<float>(indices, static_cast<int32_t>(indicesSize));
    case ScriptFieldType::Double:
        return managedArray.GetValue<double>(indices, static_cast<int32_t>(indicesSize));
    case ScriptFieldType::Bool:
        return managedArray.GetValue<bool>(indices, static_cast<int32_t>(indicesSize));
    case ScriptFieldType::Char:
        return managedArray.GetValue<char>(indices, static_cast<int32_t>(indicesSize));
    case ScriptFieldType::Vector2:
        return managedArray.GetValue<glm::vec2>(indices, static_cast<int32_t>(indicesSize));
    case ScriptFieldType::Vector3:
        return managedArray.GetValue<glm::vec3>(indices, static_cast<int32_t>(indicesSize));
    case ScriptFieldType::Color:
        return managedArray.GetValue<glm::vec4>(indices, static_cast<int32_t>(indicesSize));
    case ScriptFieldType::String:
        return GetFieldArrayValue<std::string>(array, indices, static_cast<int32_t>(indicesSize));
    case ScriptFieldType::Entity:
        return GetFieldArrayValue<Terran::Core::UUID>(array, indices, static_cast<int32_t>(indicesSize));
    default:;
    }

    TR_ASSERT(false, "Unknown script field type");
    return {};
}

void ScriptInstance::resize_field_array_internal(ScriptArray& array, int32_t length)
{
    Coral::ManagedObject object = m_context;
    Coral::ManagedArray managedArray(array.Handle, array.Rank);
    managedArray.Resize(length);
    object.SetFieldValueByHandle<Coral::ManagedArray>(array.FieldHandle, managedArray);
    array.Handle = managedArray.GetHandle();
}

void ScriptInstance::resize_field_array_internal(ScriptArray& array, int32_t const* lengths, size_t lengthsSize)
{
    Coral::ManagedObject object = m_context;
    Coral::ManagedArray managedArray(array.Handle, array.Rank);
    managedArray.Resize(lengths, lengthsSize);
    object.SetFieldValueByHandle<Coral::ManagedArray>(array.FieldHandle, managedArray);
    array.Handle = managedArray.GetHandle();
}

int32_t ScriptInstance::GetFieldArrayLength(ScriptArray const& array, int dimension) const
{
    Coral::ManagedArray managedArray(array.Handle, array.Rank);
    return managedArray.GetLength(dimension);
}

// NOTE: maybe cache the field array?
ScriptArray ScriptInstance::script_array(int32_t fieldHandle)
{
    /*auto it = m_FieldObjects.find(fieldHandle);
    if (it == m_FieldObjects.end())
    {
            TR_TRACE(m_Context);
            Coral::ManagedObject object = m_Context;
            const Coral::Type& objectType = object.GetType();
            Coral::ScopedString typeName = objectType.GetFullName();
            TR_TRACE("Type before: {0}", (std::string)typeName);
            Coral::ManagedArray array = object.GetFieldValueByHandle<Coral::ManagedArray>(fieldHandle);
            m_FieldObjects.emplace(fieldHandle, ScriptObject{ array.GetHandle(), array.GetRank(), fieldHandle });
    }

    return (ScriptArray&)m_FieldObjects.at(fieldHandle);*/

    Coral::ManagedObject object = m_context;
    Coral::ManagedArray array = object.GetFieldValueByHandle<Coral::ManagedArray>(fieldHandle);
    return ScriptArray { array.GetHandle(), array.GetRank(), fieldHandle };
}

void ScriptInstance::invoke_init() const
{
    Coral::ManagedObject object = m_context;
    if (m_oninit_method_handle)
        object.InvokeMethodByMethodInfoWithUnwrappedExceptions(m_oninit_method_handle);
}

void ScriptInstance::invoke_update(float deltaTime) const
{
    Coral::ManagedObject object = m_context;
    if (m_onupdate_method_handle)
        object.InvokeMethodByMethodInfoWithUnwrappedExceptions(m_onupdate_method_handle, deltaTime);
}

// void ScriptInstance::InvokePhysicsUpdate() const
// {
//     Coral::ManagedObject object = m_Context;
//     if (m_OnPhysicsUpdateMethodHandle)
//         object.InvokeMethodByMethodInfoWithUnwrappedExceptions(m_OnPhysicsUpdateMethodHandle);
// }

// void ScriptInstance::InvokeCollisionBegin(Entity other) const
// {
//     Coral::ManagedObject object = m_Context;
//     if (m_OnCollisionBeginMethodHandle) {
//         Coral::ManagedObject entityObject = ScriptEngine::CreateEntityInstance(other.GetID());
//         object.InvokeMethodByMethodInfoWithUnwrappedExceptions(m_OnCollisionBeginMethodHandle, entityObject);
//         entityObject.Destroy();
//     }
// }

// void ScriptInstance::InvokeCollisionEnd(Entity other) const
// {
//     Coral::ManagedObject object = m_Context;
//     if (m_OnCollisionEndMethodHandle) {
//         Coral::ManagedObject entityObject = ScriptEngine::CreateEntityInstance(other.GetID());
//         object.InvokeMethodByMethodInfoWithUnwrappedExceptions(m_OnCollisionEndMethodHandle, entityObject);
//         entityObject.Destroy();
//     }
// }

void ScriptInstance::copy_field_from(int32_t fieldHandle, Terran::Core::Shared<ScriptInstance> const& source)
{
    ScriptField const& current_field = field(fieldHandle);

    if (current_field.IsArray) {
        ScriptArray sourceArray = source->script_array(fieldHandle);
        ScriptArray destArray = script_array(fieldHandle);
        if (sourceArray.Rank > 1 || destArray.Rank > 1)
            return;

        int32_t srcArrayLength = source->GetFieldArrayLength(sourceArray);
        int32_t dstArrayLength = GetFieldArrayLength(destArray);

        if (srcArrayLength != dstArrayLength) {
            ResizeFieldArray(destArray, srcArrayLength);
        }

        for (int32_t i = 0; i < srcArrayLength; i++) {
            Utils::Variant value = source->GetFieldArrayValue<Utils::Variant>(sourceArray, i);
            SetFieldArrayValue<Utils::Variant>(destArray, value, i);
        }

        return;
    }
    Utils::Variant value = source->GetFieldValue<Utils::Variant>(fieldHandle);
    SetFieldValue(fieldHandle, value);
}

void ScriptInstance::copy_all_fields_from(Core::Shared<ScriptInstance> const& source)
{
    for (auto const& fieldHandle : m_fields | std::views::keys)
        copy_field_from(fieldHandle, source);
}

void ScriptInstance::set_field_value_internal(int32_t fieldHandle, void* value) const
{
    Coral::ManagedObject object = m_context;
    object.SetFieldValueByHandleRaw(fieldHandle, value);
}

}
