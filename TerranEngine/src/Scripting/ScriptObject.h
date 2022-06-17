#pragma once

#include "ScriptClass.h"

extern "C"
{
	typedef struct _MonoObject MonoObject;
}

namespace TerranEngine 
{
	class ScriptObject 
	{
	public:
		ScriptObject() = default;
		ScriptObject(MonoObject* monoObject);

		ScriptObject(const ScriptObject& other) = default;
		~ScriptObject() = default;

		inline MonoObject* GetMonoObject() const { return m_MonoObject; }
		
		static ScriptObject CreateInstace(const ScriptClass& klass);

		ScriptClass GetClass();
		
	private:
		MonoObject* m_MonoObject;
	};
}
