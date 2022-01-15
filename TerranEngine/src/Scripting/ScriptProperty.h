#pragma once

#include <mono/metadata/class.h>
#include <mono/metadata/object.h>

namespace TerranEngine 
{
	class ScriptProperty 
	{


	private:
		MonoProperty* m_MonoProperty;
		MonoObject* m_MonoObject;
	};
}