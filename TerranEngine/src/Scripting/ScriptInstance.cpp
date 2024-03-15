#include "trpch.h"
#include "ScriptInstance.h"

#include "Utils/Debug/OptickProfiler.h"

#include <Coral/Type.hpp>
#include <Coral/ManagedArray.hpp>
#include <Coral/String.hpp>

namespace TerranEngine 
{
	ScriptInstance::ScriptInstance(Coral::Type& type, Entity entity)
	{
		m_Context = type.CreateInstance(entity.GetID()).GetHandle();

		m_OnInitMethodHandle = type.GetMethod("Init").GetHandle();
		m_OnUpdateMethodHandle = type.GetMethod<float>("Update").GetHandle();
	}

	ScriptInstance::~ScriptInstance()
	{
		Coral::ManagedObject object = m_Context;
		for (const auto& [fieldId, scriptArray] : m_FieldArrays)
		{
			Coral::ManagedArray array(scriptArray.Handle, scriptArray.Rank);
			array.Destroy();
		}

		object.Destroy();
		m_Context = nullptr;
	}

	void ScriptInstance::GetFieldValueInternal(int32_t fieldHandle, void* value) const
	{
		Coral::ManagedObject object = m_Context;
		object.GetFieldValueByHandleRaw(fieldHandle, value);
	}

#define GET_MANAGED_ARRAY()																		\
	Coral::ManagedArray array;																	\
	auto it = m_FieldArrays.find(fieldHandle);													\
	if (it == m_FieldArrays.end())																\
	{																							\
		array = object.GetFieldValueByHandle<Coral::ManagedArray>(fieldHandle);					\
		m_FieldArrays.emplace(fieldHandle, ScriptArray{ array.GetHandle(), array.GetRank() });	\
	}																							\
	else																						\
		array = Coral::ManagedArray(it->second.Handle, it->second.Rank)

	void ScriptInstance::SetFieldArrayStringValueInternal(const ScriptArray& array, const std::string& value, const int32_t* indices, size_t indicesSize) const
	{
		Coral::String string = Coral::String::New(value);
		SetFieldArrayValueInternal(array, &string, indices, indicesSize);
		Coral::String::Free(string);
	}

	std::string ScriptInstance::GetFieldArrayStringValueInternal(const ScriptArray& array, const int32_t* indices, size_t indicesSize) const
	{
		Coral::String string;
		GetFieldArrayValueInternal(array, &string, indices, indicesSize);
		std::string result = string;
		Coral::String::Free(string);
		return result;
	}

	void ScriptInstance::SetFieldArrayValueInternal(const ScriptArray& array, void* value, const int32_t* indices, size_t indicesSize) const
	{
		Coral::ManagedArray managedArray(array.Handle, array.Rank);
		managedArray.SetValueRaw(indices, indicesSize, value);
	}

	void ScriptInstance::GetFieldArrayValueInternal(const ScriptArray& array, void* value, const int32_t* indices, size_t indicesSize) const
	{
		Coral::ManagedArray managedArray(array.Handle, array.Rank);
		managedArray.GetValueRaw(indices, indicesSize, value);
	}

	void ScriptInstance::SetFieldArrayVariantValueInternal(const ScriptArray& array, const Utils::Variant& value, const int32_t* indices, size_t indicesSize) const
	{
		Coral::ManagedArray managedArray(array.Handle, array.Rank);
		switch (value.GetType())
		{
		case Utils::Variant::Type::Int8: managedArray.SetValue<int8_t>(indices, indicesSize, value); return;
		case Utils::Variant::Type::UInt8: managedArray.SetValue<uint8_t>(indices, indicesSize, value); return;
		case Utils::Variant::Type::Int16: managedArray.SetValue<int16_t>(indices, indicesSize, value); return;
		case Utils::Variant::Type::UInt16: managedArray.SetValue<uint16_t>(indices, indicesSize, value); return;
		case Utils::Variant::Type::Int32: managedArray.SetValue<int32_t>(indices, indicesSize, value); return;
		case Utils::Variant::Type::UInt32: managedArray.SetValue<uint32_t>(indices, indicesSize, value); return;
		case Utils::Variant::Type::Int64: managedArray.SetValue<int64_t>(indices, indicesSize, value); return;
		case Utils::Variant::Type::UInt64: managedArray.SetValue<uint64_t>(indices, indicesSize, value); return;
		case Utils::Variant::Type::Float: managedArray.SetValue<float>(indices, indicesSize, value); return;
		case Utils::Variant::Type::Double: managedArray.SetValue<double>(indices, indicesSize, value); return;
		case Utils::Variant::Type::Bool: managedArray.SetValue<bool>(indices, indicesSize, value); return;
		case Utils::Variant::Type::Char: managedArray.SetValue<char>(indices, indicesSize, value); return;
		case Utils::Variant::Type::Vector2: managedArray.SetValue<glm::vec2>(indices, indicesSize, value); return;
		case Utils::Variant::Type::Vector3: managedArray.SetValue<glm::vec3>(indices, indicesSize, value); return;
		case Utils::Variant::Type::Vector4: managedArray.SetValue<glm::vec4>(indices, indicesSize, value); return;
		case Utils::Variant::Type::String: SetFieldArrayStringValueInternal(array, value, indices, indicesSize); return;
		case Utils::Variant::Type::UUID: return; // TODO
		}

		TR_ASSERT(false, "Unknown variant type");
	}

	Utils::Variant ScriptInstance::GetFieldArrayVariantValueInternal(const ScriptArray& array, const int32_t* indices, size_t indicesSize) const
	{
		Coral::ManagedArray managedArray(array.Handle, array.Rank);
		switch (m_Fields.at(array.FieldHandle).Type)
		{
		case ScriptType::Int8: return managedArray.GetValue<int8_t>(indices, indicesSize);
		case ScriptType::UInt8: return managedArray.GetValue<uint8_t>(indices, indicesSize);
		case ScriptType::Int16: return managedArray.GetValue<int16_t>(indices, indicesSize);
		case ScriptType::UInt16: return managedArray.GetValue<uint16_t>(indices, indicesSize);
		case ScriptType::Int32: return managedArray.GetValue<int32_t>(indices, indicesSize);
		case ScriptType::UInt32: return managedArray.GetValue<uint32_t>(indices, indicesSize);
		case ScriptType::Int64: return managedArray.GetValue<int64_t>(indices, indicesSize);
		case ScriptType::UInt64: return managedArray.GetValue<uint64_t>(indices, indicesSize);
		case ScriptType::Float: return managedArray.GetValue<float>(indices, indicesSize);
		case ScriptType::Double: return managedArray.GetValue<double>(indices, indicesSize); 
		case ScriptType::Bool: return managedArray.GetValue<bool>(indices, indicesSize); 
		case ScriptType::Char: return managedArray.GetValue<char>(indices, indicesSize); 
		case ScriptType::Vector2: return managedArray.GetValue<glm::vec2>(indices, indicesSize); 
		case ScriptType::Vector3: return managedArray.GetValue<glm::vec3>(indices, indicesSize); 
		case ScriptType::Color: return managedArray.GetValue<glm::vec4>(indices, indicesSize); 
		case ScriptType::String: return GetFieldArrayStringValueInternal(array, indices, indicesSize); 
		case ScriptType::Entity: return {}; //TODO
		}

		TR_ASSERT(false, "Unknown script field type");
		return { };
	}

	template<>
	void ScriptInstance::SetFieldValue<std::string>(int32_t fieldHandle, const std::string& value) const
	{
		Coral::String string = Coral::String::New(value);
		SetFieldValueInternal(fieldHandle, &string);
		Coral::String::Free(string);
	}

	template<>
	void ScriptInstance::SetFieldValue<Utils::Variant>(int32_t fieldHandle, const Utils::Variant& value) const
	{
		Coral::ManagedObject object = m_Context;
		switch (value.GetType())
		{
		case Utils::Variant::Type::Int8: object.SetFieldValueByHandle<int8_t>(fieldHandle, value); return;
		case Utils::Variant::Type::UInt8: object.SetFieldValueByHandle<uint8_t>(fieldHandle, value); return;
		case Utils::Variant::Type::Int16: object.SetFieldValueByHandle<int16_t>(fieldHandle, value); return;
		case Utils::Variant::Type::UInt16: object.SetFieldValueByHandle<uint16_t>(fieldHandle, value); return;
		case Utils::Variant::Type::Int32: object.SetFieldValueByHandle<int32_t>(fieldHandle, value); return;
		case Utils::Variant::Type::UInt32: object.SetFieldValueByHandle<uint32_t>(fieldHandle, value); return;
		case Utils::Variant::Type::Int64: object.SetFieldValueByHandle<int64_t>(fieldHandle, value); return;
		case Utils::Variant::Type::UInt64: object.SetFieldValueByHandle<uint64_t>(fieldHandle, value); return;
		case Utils::Variant::Type::Float: object.SetFieldValueByHandle<float>(fieldHandle, value); return;
		case Utils::Variant::Type::Double: object.SetFieldValueByHandle<double>(fieldHandle, value); return;
		case Utils::Variant::Type::Bool: object.SetFieldValueByHandle<bool>(fieldHandle, value); return;
		case Utils::Variant::Type::Char: object.SetFieldValueByHandle<char>(fieldHandle, value); return;
		case Utils::Variant::Type::Vector2: object.SetFieldValueByHandle<glm::vec2>(fieldHandle, value); return;
		case Utils::Variant::Type::Vector3: object.SetFieldValueByHandle<glm::vec3>(fieldHandle, value); return;
		case Utils::Variant::Type::Vector4: object.SetFieldValueByHandle<glm::vec4>(fieldHandle, value); return;
		case Utils::Variant::Type::String: SetFieldValue<std::string>(fieldHandle, value); return;
		case Utils::Variant::Type::UUID: return; // TODO
		}

		TR_ASSERT(false, "Unknown variant type");
	}

	template<>
	std::string ScriptInstance::GetFieldValue<std::string>(int32_t fieldHandle) const 
	{
		Coral::String string;
		GetFieldValueInternal(fieldHandle, &string);
		std::string result = string;
		Coral::String::Free(string);
		return result;
	}

	template<>
	Utils::Variant ScriptInstance::GetFieldValue<Utils::Variant>(int32_t fieldHandle) const 
	{
		Coral::ManagedObject object = m_Context;
		switch (m_Fields.at(fieldHandle).Type)
		{
		case ScriptType::Int8: return object.GetFieldValueByHandle<int8_t>(fieldHandle);
		case ScriptType::UInt8: return object.GetFieldValueByHandle<uint8_t>(fieldHandle);
		case ScriptType::Int16: return object.GetFieldValueByHandle<int16_t>(fieldHandle);
		case ScriptType::UInt16: return object.GetFieldValueByHandle<uint16_t>(fieldHandle);
		case ScriptType::Int32: return object.GetFieldValueByHandle<int32_t>(fieldHandle);
		case ScriptType::UInt32: return object.GetFieldValueByHandle<uint32_t>(fieldHandle);
		case ScriptType::Int64: return object.GetFieldValueByHandle<int64_t>(fieldHandle);
		case ScriptType::UInt64: return object.GetFieldValueByHandle<uint64_t>(fieldHandle);
		case ScriptType::Float: return object.GetFieldValueByHandle<float>(fieldHandle);
		case ScriptType::Double: return object.GetFieldValueByHandle<double>(fieldHandle);
		case ScriptType::Bool: return object.GetFieldValueByHandle<bool>(fieldHandle);
		case ScriptType::Char: return object.GetFieldValueByHandle<char>(fieldHandle);
		case ScriptType::Vector2: return object.GetFieldValueByHandle<glm::vec2>(fieldHandle);
		case ScriptType::Vector3: return object.GetFieldValueByHandle<glm::vec3>(fieldHandle);
		case ScriptType::Color: return object.GetFieldValueByHandle<glm::vec4>(fieldHandle);
		case ScriptType::String: return GetFieldValue<std::string>(fieldHandle);
		case ScriptType::Entity: return {}; //TODO
		}

		TR_ASSERT(false, "Unknown script field type");
		return { };
	}

	void ScriptInstance::ResizeFieldArrayInternal(ScriptArray& array, int32_t length)
	{
		Coral::ManagedObject object = m_Context;
		Coral::ManagedArray managedArray(array.Handle, array.Rank);
		managedArray.Resize(length);
		object.SetFieldValueByHandle<Coral::ManagedArray>(array.FieldHandle, managedArray);
		m_FieldArrays.at(array.FieldHandle).Handle =
			array.Handle =
			managedArray.GetHandle();
	}

	void ScriptInstance::ResizeFieldArrayInternal(ScriptArray& array, const int32_t* lengths, size_t lengthsSize)
	{
		Coral::ManagedObject object = m_Context;
		Coral::ManagedArray managedArray(array.Handle, array.Rank);
		managedArray.Resize(lengths, lengthsSize);
		object.SetFieldValueByHandle<Coral::ManagedArray>(array.FieldHandle, managedArray);
		m_FieldArrays.at(array.FieldHandle).Handle =
			array.Handle =
			managedArray.GetHandle();
	}

	int32_t ScriptInstance::GetFieldArrayLength(const ScriptArray& array, int dimension) const
	{
		Coral::ManagedArray managedArray(array.Handle, array.Rank);
		return managedArray.GetLength(dimension);
	}

	ScriptArray& ScriptInstance::GetScriptArray(int32_t fieldHandle)
	{
		auto it = m_FieldArrays.find(fieldHandle);
		if (it == m_FieldArrays.end())
		{
			Coral::ManagedObject object = m_Context;
			Coral::ManagedArray array = object.GetFieldValueByHandle<Coral::ManagedArray>(fieldHandle);
			m_FieldArrays.emplace(fieldHandle, ScriptArray{ array.GetHandle(), array.GetRank(), fieldHandle });
		}

		return m_FieldArrays.at(fieldHandle);
	}

	void ScriptInstance::InvokeInit()
	{
		Coral::ManagedObject object = m_Context;
		if(m_OnInitMethodHandle > 0)
			object.InvokeMethodByMethodInfo(m_OnInitMethodHandle);
	}

	void ScriptInstance::InvokeUpdate(float deltaTime)
	{
		Coral::ManagedObject object = m_Context;

		if(m_OnUpdateMethodHandle > 0)
			object.InvokeMethodByMethodInfo(m_OnUpdateMethodHandle, deltaTime);
	}

	void ScriptInstance::CopyFieldFrom(int32_t fieldHandle, Shared<ScriptInstance> source)
	{
		const ScriptField& field = GetScriptField(fieldHandle);

		if (field.IsArray) 
		{
			return;
		}

		Utils::Variant value = source->GetFieldValue<Utils::Variant>(fieldHandle);
		SetFieldValue(fieldHandle, value);
	}

	void ScriptInstance::CopyAllFieldsFrom(Shared<ScriptInstance> source)
	{
		for (const auto& [fieldHandle, field] : m_Fields)
			CopyFieldFrom(fieldHandle, source);
	}

	void ScriptInstance::SetFieldValueInternal(int32_t fieldHandle, void* value) const
	{
		Coral::ManagedObject object = m_Context;
		object.SetFieldValueByHandleRaw(fieldHandle, value);
	}
}