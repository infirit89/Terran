#include "trpch.h"
#include "ScriptInstance.h"

#include <Coral/Type.hpp>
#include <Coral/ManagedArray.hpp>

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

	void ScriptInstance::SetFieldArrayValueInternal(int32_t fieldHandle, void* value, const int32_t* indices, size_t indicesSize) const
	{
		Coral::ManagedObject object = m_Context;
		GET_MANAGED_ARRAY();
		array.SetValueRaw(indices, indicesSize, value);
	}

	void ScriptInstance::GetFieldArrayValueInternal(int32_t fieldHandle, void* value, const int32_t* indices, size_t indicesSize) const
	{
		Coral::ManagedObject object = m_Context;
		GET_MANAGED_ARRAY();
		array.GetValueRaw(indices, indicesSize, value);
	}

	Utils::Variant ScriptInstance::GetFieldArrayValueInternal(int32_t fieldHandle, const int32_t* indices, size_t indicesSize) const
	{
		Coral::ManagedObject object = m_Context;
		GET_MANAGED_ARRAY();
		switch (m_Fields.at(fieldHandle).Type)
		{
		case ScriptType::Int8: break;
		case ScriptType::UInt8: break;
		case ScriptType::Int16: break;
		case ScriptType::UInt16: break;
		case ScriptType::Int32: break;
		case ScriptType::UInt32: break;
		case ScriptType::Int64: break;
		case ScriptType::UInt64: break;
		case ScriptType::Float: break;
		case ScriptType::Double: break;
		case ScriptType::Char: break;
		case ScriptType::Bool: break;
		case ScriptType::Vector2: break;
		case ScriptType::Vector3: break;
		case ScriptType::Color: break;
		case ScriptType::String: break;
		case ScriptType::Entity: break;
		}
	}

	void ScriptInstance::ResizeFieldArrayInternal(int32_t fieldHandle, int32_t length)
	{
		Coral::ManagedObject object = m_Context;
		GET_MANAGED_ARRAY();
		array.Resize(length);
		object.SetFieldValueByHandle<Coral::ManagedArray>(fieldHandle, array);
		m_FieldArrays.at(fieldHandle).Handle = array.GetHandle();
	}

	void ScriptInstance::ResizeFieldArrayInternal(int32_t fieldHandle, const int32_t* lengths, size_t lengthsSize)
	{
		Coral::ManagedObject object = m_Context;
		GET_MANAGED_ARRAY();
		array.Resize(lengths, lengthsSize);
		object.SetFieldValueByHandle<Coral::ManagedArray>(fieldHandle, array);
		m_FieldArrays.at(fieldHandle).Handle = array.GetHandle();
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

	void ScriptInstance::SetFieldValueInternal(int32_t fieldHandle, void* value) const
	{
		Coral::ManagedObject object = m_Context;
		object.SetFieldValueByHandleRaw(fieldHandle, value);
	}
}
