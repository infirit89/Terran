#pragma once

#include <mono/metadata/debug-helpers.h>

namespace TerranEngine 
{
	class ScriptMethod 
	{
	public:
		ScriptMethod(MonoMethod* monoMethod);

		// TODO: should take in an object and parameters
		void Execute();
	private:
		MonoMethod* m_MonoMethod;
	};
} 