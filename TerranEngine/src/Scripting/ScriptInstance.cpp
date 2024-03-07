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
		object.Destroy();
		m_Context = nullptr;
	}

	void ScriptInstance::GetFieldValueInternal(int32_t fieldHandle, void* value) const
	{
		Coral::ManagedObject object = m_Context;
		object.GetFieldValueByHandleRaw(fieldHandle, value);
	}

	void ScriptInstance::SetFieldArrayValueInternal(int32_t fieldHandle, void* value, const int32_t* indices, size_t indicesSize) const
	{
		Coral::ManagedObject object = m_Context;
		Coral::ManagedArray array = object.GetFieldValueByHandle<Coral::ManagedArray>(fieldHandle);
		array.SetValueRaw(indices, indicesSize, value);
	}

	void ScriptInstance::GetFieldArrayValueInternal(int32_t fieldHandle, void* value, const int32_t* indices, size_t indicesSize) const
	{
		Coral::ManagedObject object = m_Context;
		Coral::ManagedArray array = object.GetFieldValueByHandle<Coral::ManagedArray>(fieldHandle);
		array.GetValueRaw(indices, indicesSize, value);
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
