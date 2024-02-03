#include "trpch.h"
#include "ScriptInstance.h"

#include <Coral/Type.hpp>

namespace TerranEngine 
{
	ScriptInstance::ScriptInstance(Coral::Type& type, Entity entity)
	{
		m_Context = type.CreateInstance(entity.GetID()).GetHandle();

		m_OnInitMethodHandle = type.GetMethod("Init").GetHandle();
		m_OnUpdateMethodHandle = type.GetMethod<float>("Update").GetHandle();
	}

	void ScriptInstance::GetFieldValueInternal(int32_t fieldHandle, void* value) const
	{
		Coral::ManagedObject object = m_Context;
		object.GetFieldValueByHandleRaw(fieldHandle, value);
	}

	void ScriptInstance::SetFieldValueInternal(int32_t fieldHandle, void* value) const
	{
		Coral::ManagedObject object = m_Context;
		object.SetFieldValueByHandleRaw(fieldHandle, value);
	}
}
