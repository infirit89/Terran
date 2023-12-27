#include "trpch.h"
#include "ManagedObject.h"
#include "ManagedClass.h"

#include <mono/metadata/object.h>
#include <mono/metadata/appdomain.h>

namespace TerranEngine 
{
	ManagedObject::ManagedObject(MonoObject* monoObject)
		: m_MonoObject(monoObject), m_Class(nullptr)
	{ }
	
	/*ManagedObject ManagedObject::CreateInstace(const ManagedClass& klass)
	{
		MonoObject* monoObject = mono_object_new(mono_domain_get(), klass.GetMonoClass());
		mono_runtime_object_init(monoObject);
		
		return monoObject;
	}*/

	/*ManagedClass ManagedObject::GetClass()
	{
		if(!m_MonoObject)
			return {};
			
		MonoClass* monoClass = mono_object_get_class(m_MonoObject);

		return { monoClass };
	}*/
}
